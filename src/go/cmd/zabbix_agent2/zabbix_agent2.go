/*
** Zabbix
** Copyright (C) 2001-2021 Zabbix SIA
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/

package main

import (
	"errors"
	"flag"
	"fmt"
	"os"
	"os/signal"
	"strings"
	"syscall"
	"time"

	_ "zabbix.com/plugins"

	"zabbix.com/internal/agent"
	"zabbix.com/internal/agent/keyaccess"
	"zabbix.com/internal/agent/remotecontrol"
	"zabbix.com/internal/agent/resultcache"
	"zabbix.com/internal/agent/scheduler"
	"zabbix.com/internal/agent/serverconnector"
	"zabbix.com/internal/agent/serverlistener"
	"zabbix.com/internal/agent/statuslistener"
	"zabbix.com/internal/monitor"
	"zabbix.com/pkg/conf"
	"zabbix.com/pkg/log"
	"zabbix.com/pkg/pidfile"
	"zabbix.com/pkg/tls"
	"zabbix.com/pkg/version"
	"zabbix.com/pkg/zbxlib"
)

const remoteCommandSendingTimeout = time.Second

var manager *scheduler.Manager
var listeners []*serverlistener.ServerListener
var serverConnectors []*serverconnector.Connector
var closeChan = make(chan bool)
var stopChan = make(chan bool)

func processLoglevelIncreaseCommand(c *remotecontrol.Client) (err error) {
	if log.IncreaseLogLevel() {
		message := fmt.Sprintf("Increased log level to %s", log.Level())
		log.Infof(message)
		err = c.Reply(message)
		return
	}
	err = fmt.Errorf("Cannot increase log level above %s", log.Level())
	log.Infof(err.Error())

	return
}

func processLoglevelDecreaseCommand(c *remotecontrol.Client) (err error) {
	if log.DecreaseLogLevel() {
		message := fmt.Sprintf("Decreased log level to %s", log.Level())
		log.Infof(message)
		err = c.Reply(message)
		return
	}
	err = fmt.Errorf("Cannot decrease log level below %s", log.Level())
	log.Infof(err.Error())

	return
}

func processMetricsCommand(c *remotecontrol.Client) (err error) {
	data := manager.Query("metrics")
	return c.Reply(data)
}

func processVersionCommand(c *remotecontrol.Client) (err error) {
	data := version.Long()
	return c.Reply(data)
}

func processHelpCommand(c *remotecontrol.Client) (err error) {
	help := `Remote control interface, available commands:
	log_level_increase - Increase log level
	log_level_decrease - Decrease log level
	metrics - List available metrics
	version - Display Agent version
	help - Display this help message`
	return c.Reply(help)
}

func processRemoteCommand(c *remotecontrol.Client) (err error) {
	params := strings.Fields(c.Request())
	switch len(params) {
	case 0:
		return errors.New("Empty command")
	case 2:
		return errors.New("Too many commands")
	default:
	}

	switch params[0] {
	case "log_level_increase":
		err = processLoglevelIncreaseCommand(c)
	case "log_level_decrease":
		err = processLoglevelDecreaseCommand(c)
	case "help":
		err = processHelpCommand(c)
	case "metrics":
		err = processMetricsCommand(c)
	case "version":
		err = processVersionCommand(c)
	default:
		return errors.New("Unknown command")
	}
	return
}

var pidFile *pidfile.File

func run() (err error) {
	sigs := make(chan os.Signal, 1)
	signal.Notify(sigs, syscall.SIGINT, syscall.SIGTERM)

	var control *remotecontrol.Conn
	if control, err = remotecontrol.New(agent.Options.ControlSocket, remoteCommandSendingTimeout); err != nil {
		return
	}
	confirmService()
	control.Start()

loop:
	for {
		select {
		case sig := <-sigs:
			switch sig {
			case syscall.SIGINT, syscall.SIGTERM:
				sendServiceStop()
				break loop
			}
		case client := <-control.Client():
			if rerr := processRemoteCommand(client); rerr != nil {
				if rerr = client.Reply("error: " + rerr.Error()); rerr != nil {
					log.Warningf("cannot reply to remote command: %s", rerr)
				}
			}
			sendServiceStop()
			client.Close()
		case serviceStop := <-closeChan:
			if serviceStop {
				break loop
			}
		}
	}
	control.Stop()
	return nil
}

var (
	confDefault string

	argConfig  bool
	argTest    bool
	argPrint   bool
	argVersion bool
	argVerbose bool
)

func main() {
	var confFlag string
	const (
		confDescription = "Path to the configuration file"
	)
	flag.StringVar(&confFlag, "config", confDefault, confDescription)
	flag.StringVar(&confFlag, "c", confDefault, confDescription+" (shorthand)")

	var foregroundFlag bool
	const (
		foregroundDefault     = true
		foregroundDescription = "Run Zabbix agent in foreground"
	)
	flag.BoolVar(&foregroundFlag, "foreground", foregroundDefault, foregroundDescription)
	flag.BoolVar(&foregroundFlag, "f", foregroundDefault, foregroundDescription+" (shorthand)")

	var testFlag string
	const (
		testDefault     = ""
		testDescription = "Test specified item and exit"
	)
	flag.StringVar(&testFlag, "test", testDefault, testDescription)
	flag.StringVar(&testFlag, "t", testDefault, testDescription+" (shorthand)")

	var printFlag bool
	const (
		printDefault     = false
		printDescription = "Print known items and exit"
	)
	flag.BoolVar(&printFlag, "print", printDefault, printDescription)
	flag.BoolVar(&printFlag, "p", printDefault, printDescription+" (shorthand)")

	var verboseFlag bool
	const (
		verboseDefault     = false
		verboseDescription = "Enable verbose output for metric testing or printing"
	)
	flag.BoolVar(&verboseFlag, "verbose", verboseDefault, verboseDescription)
	flag.BoolVar(&verboseFlag, "v", verboseDefault, verboseDescription+" (shorthand)")

	var versionFlag bool
	const (
		versionDefault     = false
		versionDescription = "Print program version and exit"
	)
	flag.BoolVar(&versionFlag, "version", versionDefault, versionDescription)
	flag.BoolVar(&versionFlag, "V", versionDefault, versionDescription+" (shorthand)")

	var remoteCommand string
	const remoteDefault = ""
	var remoteDescription = "Perform administrative functions (send 'help' for available commands) " +
		"(" + remoteCommandSendingTimeout.String() + " timeout)"
	flag.StringVar(&remoteCommand, "R", remoteDefault, remoteDescription)

	var helpFlag bool
	const (
		helpDefault     = false
		helpDescription = "Display this help message"
	)
	flag.BoolVar(&helpFlag, "help", helpDefault, helpDescription)
	flag.BoolVar(&helpFlag, "h", helpDefault, helpDescription+" (shorthand)")

	loadOSDependentFlags()
	flag.Parse()
	setServiceRun(foregroundFlag)

	if helpFlag {
		flag.Usage()
		os.Exit(0)
	}

	var argConfig, argTest, argPrint, argVersion, argVerbose bool

	// Need to manually check if the flag was specified, as default flag package
	// does not offer automatic detection. Consider using third party package.
	flag.Visit(func(f *flag.Flag) {
		switch f.Name {
		case "c", "config":
			argConfig = true
		case "t", "test":
			argTest = true
		case "p", "print":
			argPrint = true
		case "V", "version":
			argVersion = true
		case "v", "verbose":
			argVerbose = true
		}
	})

	if argVersion {
		version.Display()
		os.Exit(0)
	}

	if err := openEventLog(); err != nil {
		fatalExit("", err)
	}

	if err := validateExclusiveFlags(); err != nil {
		if eerr := eventLogErr(err); eerr != nil {
			err = fmt.Errorf("%s and %s", err, eerr)
		}
		fatalExit("", err)
	}

	if err := conf.Load(confFlag, &agent.Options); err != nil {
		if argConfig || !(argTest || argPrint) {
			if eerr := eventLogErr(err); eerr != nil {
				err = fmt.Errorf("%s and %s", err, eerr)
			}
			fatalExit("", err)
		}
		// create default configuration for testing options
		if !argConfig {
			_ = conf.Unmarshal([]byte{}, &agent.Options)
		}
	}

	if err := agent.ValidateOptions(agent.Options); err != nil {
		if eerr := eventLogErr(err); eerr != nil {
			err = fmt.Errorf("%s and %s", err, eerr)
		}
		fatalExit("cannot validate configuration", err)
	}

	if err := handleWindowsService(confFlag); err != nil {
		if eerr := eventLogErr(err); eerr != nil {
			err = fmt.Errorf("%s and %s", err, eerr)
		}
		fatalExit("", err)
	}

	if err := log.Open(log.Console, log.Warning, "", 0); err != nil {
		fatalExit("cannot initialize logger", err)
	}

	if argTest || argPrint {
		var level int
		if argVerbose {
			level = log.Trace
		} else {
			level = log.None
		}
		if err := log.Open(log.Console, level, "", 0); err != nil {
			fatalExit("cannot initialize logger", err)
		}

		if err := keyaccess.LoadRules(agent.Options.AllowKey, agent.Options.DenyKey); err != nil {
			fatalExit("failed to load key access rules", err)
		}

		var err error
		if err = agent.InitUserParameterPlugin(agent.Options.UserParameter, agent.Options.UnsafeUserParameters); err != nil {
			fatalExit("cannot initialize user parameters", err)
		}

		var m *scheduler.Manager
		if m, err = scheduler.NewManager(&agent.Options); err != nil {
			fatalExit("cannot create scheduling manager", err)
		}
		m.Start()

		if err = configUpdateItemParameters(m, &agent.Options); err != nil {
			fatalExit("cannot process configuration", err)
		}

		if argTest {
			checkMetric(m, testFlag)
		} else {
			checkMetrics(m)
		}

		m.Stop()
		monitor.Wait(monitor.Scheduler)
		os.Exit(0)
	}

	if argVerbose {
		fatalExit("", errors.New("verbose parameter can be specified only with test or print parameters"))
	}

	if remoteCommand != "" {
		if agent.Options.ControlSocket == "" {
			log.Errf("Cannot send remote command: ControlSocket configuration parameter is not defined")
			os.Exit(0)
		}

		if reply, err := remotecontrol.SendCommand(agent.Options.ControlSocket, remoteCommand,
			remoteCommandSendingTimeout); err != nil {
			log.Errf("Cannot send remote command: %s", err)
		} else {
			log.Infof(reply)
		}
		os.Exit(0)
	}

	var logType, logLevel int
	switch agent.Options.LogType {
	case "system":
		logType = log.System
	case "console":
		logType = log.Console
	case "file":
		logType = log.File
	}
	switch agent.Options.DebugLevel {
	case 0:
		logLevel = log.Info
	case 1:
		logLevel = log.Crit
	case 2:
		logLevel = log.Err
	case 3:
		logLevel = log.Warning
	case 4:
		logLevel = log.Debug
	case 5:
		logLevel = log.Trace
	}

	if err := log.Open(logType, logLevel, agent.Options.LogFile, agent.Options.LogFileSize); err != nil {
		fatalExit("cannot initialize logger", err)
	}

	zbxlib.SetLogLevel(logLevel)

	greeting := fmt.Sprintf("Starting Zabbix Agent 2 [%s]. (%s)", agent.Options.Hostname, version.Long())
	log.Infof(greeting)

	addresses, err := serverconnector.ParseServerActive()
	if err != nil {
		fatalExit("cannot parse the \"ServerActive\" parameter", err)
	}

	if err = resultcache.Prepare(&agent.Options, addresses); err != nil {
		fatalExit("cannot prepare result cache", err)
	}

	if tlsConfig, err := agent.GetTLSConfig(&agent.Options); err != nil {
		fatalExit("cannot use encryption configuration", err)
	} else {
		if tlsConfig != nil {
			if err := tls.Init(tlsConfig); err != nil {
				fatalExit("cannot configure encryption", err)
			}
		}
	}

	if pidFile, err = pidfile.New(agent.Options.PidFile); err != nil {
		fatalExit("cannot initialize PID file", err)
	}
	defer pidFile.Delete()

	log.Infof("using configuration file: %s", confFlag)

	if err := keyaccess.LoadRules(agent.Options.AllowKey, agent.Options.DenyKey); err != nil {
		log.Errf("Failed to load key access rules: %s", err.Error())
		os.Exit(1)
	}

	if err = agent.InitUserParameterPlugin(agent.Options.UserParameter, agent.Options.UnsafeUserParameters); err != nil {
		fatalExit("cannot initialize user parameters", err)
	}

	if manager, err = scheduler.NewManager(&agent.Options); err != nil {
		fatalExit("cannot create scheduling manager", err)
	}

	// replacement of deprecated StartAgents
	if 0 != len(agent.Options.Server) {
		var listenIPs []string
		if listenIPs, err = serverlistener.ParseListenIP(&agent.Options); err != nil {
			fatalExit("cannot parse \"ListenIP\" parameter", err)
		}
		for i := 0; i < len(listenIPs); i++ {
			listener := serverlistener.New(i, manager, listenIPs[i], &agent.Options)
			listeners = append(listeners, listener)
		}
	}
	if err = loadOSDependentItems(); err != nil {
		fatalExit("cannot load os dependent items", err)
	}

	if foregroundFlag {
		if agent.Options.LogType != "console" {
			fmt.Println(greeting)
		}
		fmt.Println("Press Ctrl+C to exit.")
	}

	manager.Start()

	if err = configUpdateItemParameters(manager, &agent.Options); err != nil {
		fatalExit("cannot process configuration", err)
	}

	serverConnectors = make([]*serverconnector.Connector, len(addresses))

	for i := 0; i < len(serverConnectors); i++ {
		if serverConnectors[i], err = serverconnector.New(manager, addresses[i], &agent.Options); err != nil {
			fatalExit("cannot create server connector", err)
		}
		serverConnectors[i].Start()
	}

	for _, listener := range listeners {
		if err = listener.Start(); err != nil {
			fatalExit("cannot start server listener", err)
		}
	}

	if agent.Options.StatusPort != 0 {
		if err = statuslistener.Start(manager, confFlag); err != nil {
			fatalExit("cannot start HTTP listener", err)
		}
	}

	if err == nil {
		err = run()
	}
	if err != nil {
		log.Errf("cannot start agent: %s", err.Error())
	}

	if agent.Options.StatusPort != 0 {
		statuslistener.Stop()
	}
	for _, listener := range listeners {
		listener.Stop()
	}
	for i := 0; i < len(serverConnectors); i++ {
		serverConnectors[i].StopConnector()
	}
	monitor.Wait(monitor.Input)

	manager.Stop()
	monitor.Wait(monitor.Scheduler)

	// split shutdown in two steps to ensure that result cache is still running while manager is
	// being stopped, because there might be pending exporters that could block if result cache
	// is stopped and its input channel is full.
	for i := 0; i < len(serverConnectors); i++ {
		serverConnectors[i].StopCache()
	}
	monitor.Wait(monitor.Output)
	farewell := fmt.Sprintf("Zabbix Agent 2 stopped. (%s)", version.Long())
	log.Infof(farewell)

	if foregroundFlag && agent.Options.LogType != "console" {
		fmt.Println(farewell)
	}
	waitServiceClose()
}

func fatalExit(message string, err error) {
	fatalCloseOSItems()
	if len(message) == 0 {
		message = err.Error()
	} else {
		message = fmt.Sprintf("%s: %s", message, err.Error())
	}

	if agent.Options.LogType == "file" {
		log.Critf("%s", message)
	}

	fmt.Fprintf(os.Stderr, "zabbix_agent2 [%d]: ERROR: %s\n", os.Getpid(), message)
	os.Exit(1)
}
