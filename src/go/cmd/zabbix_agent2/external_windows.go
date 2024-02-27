/*
** Zabbix
** Copyright (C) 2001-2024 Zabbix SIA
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
	"fmt"
	"net"

	"github.com/Microsoft/go-winio"
	"zabbix.com/plugins/external"
)

func getListener(socket string) (listener net.Listener, err error) {
	listener, err = winio.ListenPipe(socket, nil)
	if err != nil {
		err = fmt.Errorf(
			"failed to create plugin listener with socket path, %s, %s", socket, err.Error(),
		)

		return
	}

	return
}

func cleanUpExternal() {}

func checkExternalExits() error {
	return nil
}

func checkExternalExit() error {
	return nil
}

func listenOnPluginFail(p *external.Plugin, name string) {}
