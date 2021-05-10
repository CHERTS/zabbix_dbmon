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

package mongodb

import (
	"encoding/json"
	"sort"

	"zabbix.com/pkg/zbxerr"
)

type dbEntity struct {
	DBName string `json:"{#DBNAME}"`
}

// databasesDiscoveryHandler
// https://docs.mongodb.com/manual/reference/command/listDatabases/
func databasesDiscoveryHandler(s Session, _ map[string]string) (interface{}, error) {
	dbs, err := s.DatabaseNames()
	if err != nil {
		return nil, zbxerr.ErrorCannotFetchData.Wrap(err)
	}

	lld := make([]dbEntity, 0)

	sort.Strings(dbs)

	for _, db := range dbs {
		lld = append(lld, dbEntity{DBName: db})
	}

	jsonLLD, err := json.Marshal(lld)
	if err != nil {
		return nil, zbxerr.ErrorCannotMarshalJSON.Wrap(err)
	}

	return string(jsonLLD), nil
}
