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
	"zabbix.com/pkg/conf"
	"zabbix.com/pkg/plugin"
)

// PluginOptions option from config file
type PluginOptions struct {
	// Timeout is the maximum time in seconds for waiting when a connection has to be established.
	// Default value equals to the global timeout.
	Timeout int `conf:"optional,range=1:30"`

	// CallTimeout is the maximum time in seconds for waiting when a request has to be done.
	// Default value equals to the global agent timeout.
	CallTimeout int `conf:"optional,range=1:30"`

	// KeepAlive is a time to wait before unused connections will be closed.
	KeepAlive int `conf:"optional,range=60:900,default=300"`

	// Sessions stores pre-defined named sets of connections settings.
	Sessions map[string]conf.Session `conf:"optional"`
}

// Configure implements the Configurator interface.
// Initializes configuration structures.
func (p *Plugin) Configure(global *plugin.GlobalOptions, options interface{}) {
	if err := conf.Unmarshal(options, &p.options); err != nil {
		p.Errf("cannot unmarshal configuration options: %s", err)
	}

	if p.options.Timeout == 0 {
		p.options.Timeout = global.Timeout
	}

	if p.options.CallTimeout == 0 {
		p.options.CallTimeout = global.Timeout
	}
}

// Validate implements the Configurator interface.
// Returns an error if validation of a plugin's configuration is failed.
func (p *Plugin) Validate(options interface{}) error {
	var opts PluginOptions

	return conf.Unmarshal(options, &opts)
}
