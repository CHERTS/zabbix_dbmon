/*
** Zabbix
** Copyright (C) 2001-2020 Zabbix SIA
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

package smart

import (
	"encoding/json"
	"strings"

	"zabbix.com/pkg/conf"
	"zabbix.com/pkg/plugin"
	"zabbix.com/pkg/zbxerr"
)

// Options -
type Options struct {
	Timeout int    `conf:"optional,range=1:30"`
	Path    string `conf:"optional"`
}

// Plugin -
type Plugin struct {
	plugin.Base
	options Options
}

var impl Plugin

// Configure -
func (p *Plugin) Configure(global *plugin.GlobalOptions, options interface{}) {
	if err := conf.Unmarshal(options, &p.options); err != nil {
		p.Errf("cannot unmarshal configuration options: %s", err)
	}

	if p.options.Timeout == 0 {
		p.options.Timeout = global.Timeout
	}
}

// Validate -
func (p *Plugin) Validate(options interface{}) error {
	var o Options
	return conf.Unmarshal(options, &o)
}

// Export -
func (p *Plugin) Export(key string, params []string, ctx plugin.ContextProvider) (result interface{}, err error) {
	if len(params) > 0 {
		return nil, zbxerr.ErrorTooManyParameters
	}

	if err = p.checkVersion(); err != nil {
		return
	}

	var jsonArray []byte

	switch key {
	case "smart.disk.discovery":
		out := []device{}

		r, err := p.execute(false)
		if err != nil {
			return nil, err
		}

		for _, dev := range r.devices {
			out = append(out, device{
				Name:       cutPrefix(dev.Info.Name),
				DeviceType: strings.ToUpper(getType(dev.Info.DevType, dev.RotationRate)),
				Model:      dev.ModelName, SerialNumber: dev.SerialNumber,
			})
		}

		jsonArray, err = json.Marshal(out)
		if err != nil {
			return nil, zbxerr.ErrorCannotMarshalJSON.Wrap(err)
		}

	case "smart.disk.get":
		r, err := p.execute(true)
		if err != nil {
			return nil, err
		}

		fields, err := setDiskFields(r.jsonDevices)
		if err != nil {
			return nil, err
		}

		if fields == nil {
			jsonArray, err = json.Marshal([]string{})
			if err != nil {
				return nil, zbxerr.ErrorCannotMarshalJSON.Wrap(err)
			}

			break
		}

		jsonArray, err = json.Marshal(fields)
		if err != nil {
			return nil, zbxerr.ErrorCannotMarshalJSON.Wrap(err)
		}

	case "smart.attribute.discovery":
		out := []attribute{}

		r, err := p.execute(false)
		if err != nil {
			return nil, err
		}

		for _, dev := range r.devices {
			var t string
			if dev.RotationRate == 0 {
				t = "SSD"
			} else {
				t = "HDD"
			}

			for _, attr := range dev.SmartAttributes.Table {
				out = append(
					out, attribute{
						Name:       cutPrefix(dev.Info.Name),
						DeviceType: t,
						ID:         attr.ID,
						Attrname:   attr.Attrname,
						Thresh:     attr.Thresh,
					})
			}
		}

		jsonArray, err = json.Marshal(out)
		if err != nil {
			return nil, zbxerr.ErrorCannotMarshalJSON.Wrap(err)
		}

	default:
		return nil, zbxerr.ErrorUnsupportedMetric
	}

	return string(jsonArray), nil
}

// setDiskFields goes through provided device json map and sets disk_name
// disk_type and returns the devices in a slice.
// It returns an error if there is an issue with unmarshal for the provided input JSON map
func setDiskFields(deviceJsons map[string]jsonDevice) (out []interface{}, err error) {
	for k, v := range deviceJsons {
		b := make(map[string]interface{})
		if err = json.Unmarshal([]byte(v.jsonData), &b); err != nil {
			return out, zbxerr.ErrorCannotUnmarshalJSON.Wrap(err)
		}

		b["disk_name"] = cutPrefix(k)

		var devType string

		if dev, ok := b["device"]; ok {
			s, ok := dev.(string)
			if ok {
				info := make(map[string]string)
				if err = json.Unmarshal([]byte(s), &info); err != nil {
					return out, zbxerr.ErrorCannotUnmarshalJSON.Wrap(err)
				}

				devType = info["type"]
			}
		}

		rateInt := -1

		if rate, ok := b["rotation_rate"]; ok {
			switch r := rate.(type) {
			case int:
				rateInt = r
			case float64:
				rateInt = int(r)
			}
		}

		b["disk_type"] = getType(devType, rateInt)
		out = append(out, b)
	}

	return
}

func getType(devType string, rate int) (out string) {
	out = "unknown"
	if devType == "nvme" {
		out = "nvme"
	} else {
		if rate == 0 {
			out = "ssd"
		} else if rate > 0 {
			out = "hdd"
		}
	}

	return
}

func init() {
	plugin.RegisterMetrics(&impl, "Smart",
		"smart.disk.discovery", "Returns JSON array of smart devices.",
		"smart.disk.get", "Returns JSON data of smart device.",
		"smart.attribute.discovery", "Returns JSON array of smart device attributes.",
	)
}
