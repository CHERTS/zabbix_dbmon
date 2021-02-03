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

package mysql

import (
	"context"

	"zabbix.com/pkg/metric"
	"zabbix.com/pkg/plugin"
	"zabbix.com/pkg/uri"
)

const (
	keyDatabasesDiscovery     = "mysql.db.discovery"
	keyDatabaseSize           = "mysql.db.size"
	keyPing                   = "mysql.ping"
	keyReplicationDiscovery   = "mysql.replication.discovery"
	keyReplicationSlaveStatus = "mysql.replication.get_slave_status"
	keyStatusVars             = "mysql.get_status_variables"
	keyVersion                = "mysql.version"
)

// handlerFunc defines an interface must be implemented by handlers.
type handlerFunc func(ctx context.Context, conn MyClient,
	params map[string]string, extraParams ...string) (res interface{}, err error)

// getHandlerFunc returns a handlerFunc related to a given key.
func getHandlerFunc(key string) handlerFunc {
	switch key {
	case keyDatabasesDiscovery:
		return databasesDiscoveryHandler
	case keyDatabaseSize:
		return databaseSizeHandler
	case keyPing:
		return pingHandler
	case keyReplicationDiscovery:
		return replicationDiscoveryHandler
	case keyReplicationSlaveStatus:
		return replicationSlaveStatusHandler
	case keyStatusVars:
		return statusVarsHandler
	case keyVersion:
		return versionHandler

	default:
		return nil
	}
}

var uriDefaults = &uri.Defaults{Scheme: "tcp", Port: "3306"}

// Common params: [URI|Session][,User][,Password]
var (
	paramURI = metric.NewConnParam("URI", "URI to connect or session name.").
			WithDefault(uriDefaults.Scheme + "://localhost:" + uriDefaults.Port).WithSession().
			WithValidator(uri.URIValidator{Defaults: uriDefaults, AllowedSchemes: []string{"tcp", "unix"}})
	paramUsername = metric.NewConnParam("User", "MySQL user.").WithDefault("root")
	paramPassword = metric.NewConnParam("Password", "User's password.").WithDefault("")
)

var metrics = metric.MetricSet{
	keyDatabasesDiscovery: metric.New("Returns list of databases in LLD format.",
		[]*metric.Param{paramURI, paramUsername, paramPassword}, false),

	keyDatabaseSize: metric.New("Returns size of given database in bytes.",
		[]*metric.Param{paramURI, paramUsername, paramPassword,
			metric.NewParam("Database", "Database name.").SetRequired(),
		}, false),

	keyPing: metric.New("Tests if connection is alive or not.",
		[]*metric.Param{paramURI, paramUsername, paramPassword}, false),

	keyReplicationDiscovery: metric.New("Returns replication information in LLD format.",
		[]*metric.Param{paramURI, paramUsername, paramPassword}, false),

	keyReplicationSlaveStatus: metric.New("Returns replication status.",
		[]*metric.Param{paramURI, paramUsername, paramPassword,
			metric.NewParam("Master", "Master host."),
		}, false),

	keyStatusVars: metric.New("Returns values of global status variables.",
		[]*metric.Param{paramURI, paramUsername, paramPassword}, false),

	keyVersion: metric.New("Returns MySQL version.",
		[]*metric.Param{paramURI, paramUsername, paramPassword}, false),
}

func init() {
	plugin.RegisterMetrics(&impl, pluginName, metrics.List()...)
}
