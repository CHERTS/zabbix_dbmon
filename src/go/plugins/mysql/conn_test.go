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

package mysql

import (
	"reflect"
	"testing"

	"git.zabbix.com/ap/plugin-support/tlsconfig"
)

func Test_getTLSDetails(t *testing.T) {
	type args struct {
		ck connKey
	}
	tests := []struct {
		name    string
		args    args
		want    *tlsconfig.Details
		wantErr bool
	}{
		{
			"required",
			args{

				connKey{
					tlsConnect: "required",
					rawUri:     "127.0.0.1",
				},
			},
			&tlsconfig.Details{
				TlsConnect:         "required",
				RawUri:             "127.0.0.1",
				AllowedConnections: map[string]bool{disable: true, require: true, verifyCa: true, verifyFull: true},
			},
			false,
		},
		{
			"verify_ca",
			args{
				connKey{
					tlsConnect: "verify_ca",
					tlsCA:      "path/to/ca",
					rawUri:     "127.0.0.1",
				},
			},
			&tlsconfig.Details{
				TlsConnect:         "verify_ca",
				TlsCaFile:          "path/to/ca",
				RawUri:             "127.0.0.1",
				AllowedConnections: map[string]bool{disable: true, require: true, verifyCa: true, verifyFull: true},
			},
			false,
		},
		{
			"verify_full and check clients certs",
			args{
				connKey{
					tlsConnect: "verify_ca",
					tlsCA:      "path/to/ca",
					tlsCert:    "path/to/cert",
					tlsKey:     "path/to/key",
					rawUri:     "127.0.0.1",
				},
			},
			&tlsconfig.Details{
				TlsConnect:         "verify_ca",
				TlsCaFile:          "path/to/ca",
				TlsCertFile:        "path/to/cert",
				TlsKeyFile:         "path/to/key",
				RawUri:             "127.0.0.1",
				AllowedConnections: map[string]bool{disable: true, require: true, verifyCa: true, verifyFull: true},
			},
			false,
		},
		{
			"missing ca file",
			args{
				connKey{
					tlsConnect: "verify_ca",
					rawUri:     "127.0.0.1",
				},
			},
			nil,
			true,
		},
		{
			"missing client key file",
			args{
				connKey{
					tlsConnect: "verify_ca",
					tlsCA:      "path/to/ca",
					tlsCert:    "path/to/cert",
					rawUri:     "127.0.0.1",
				},
			},
			nil,
			true,
		},
		{
			"missing client cert file",
			args{
				connKey{
					tlsConnect: "verify_ca",
					tlsCA:      "path/to/ca",
					tlsKey:     "path/to/key",
					rawUri:     "127.0.0.1",
				},
			},
			nil,
			true,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got, err := getTLSDetails(tt.args.ck)
			if (err != nil) != tt.wantErr {
				t.Errorf("getTLSDetails() error = %v, wantErr %v", err, tt.wantErr)

				return
			}
			if !reflect.DeepEqual(got, tt.want) {
				t.Errorf("getTLSDetails() = %v, want %v", got, tt.want)
			}
		})
	}
}
