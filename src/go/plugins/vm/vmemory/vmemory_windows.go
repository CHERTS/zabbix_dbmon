/*
** Zabbix
** Copyright (C) 2001-2022 Zabbix SIA
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

package vmemory

import (
	"zabbix.com/pkg/plugin"
	"zabbix.com/pkg/win32"
	"zabbix.com/pkg/zbxerr"
)

// Plugin -
type Plugin struct {
	plugin.Base
}

const percent = 100

var impl Plugin

// Export -
func (p *Plugin) Export(key string, params []string, ctx plugin.ContextProvider) (result interface{}, err error) {
	if len(params) > 1 {
		return nil, zbxerr.ErrorTooManyParameters
	}

	switch key {
	case "vm.vmemory.size":
		var mode string
		if len(params) > 0 {
			mode = params[0]
		}

		return p.exportVMVMemorySize(mode)
	default:
		return nil, plugin.UnsupportedMetricError
	}
}

func init() {
	plugin.RegisterMetrics(&impl, "VMemory",
		"vm.vmemory.size", "Returns virtual memory size in bytes or in percentage.",
	)
}

func (p *Plugin) exportVMVMemorySize(mode string) (result interface{}, err error) {
	mem, err := win32.GlobalMemoryStatusEx()
	if err != nil {
		return nil, err
	}
	switch mode {
	case "", "total":
		return mem.TotalPageFile, nil
	case "available":
		return mem.AvailPageFile, nil
	case "used":
		return mem.TotalPageFile - mem.AvailPageFile, nil
	case "pused":
		return float64(mem.TotalPageFile-mem.AvailPageFile) / float64(mem.TotalPageFile) * percent, nil
	case "pavailable":
		return float64(mem.AvailPageFile) / float64(mem.TotalPageFile) * percent, nil
	default:
		return nil, zbxerr.ErrorInvalidParams
	}
}
