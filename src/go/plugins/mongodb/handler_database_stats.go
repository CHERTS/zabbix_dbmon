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

	"gopkg.in/mgo.v2/bson"
	"zabbix.com/pkg/zbxerr"
)

// databaseStatsHandler
// https://docs.mongodb.com/manual/reference/command/dbStats/index.html
func databaseStatsHandler(s Session, params map[string]string) (interface{}, error) {
	dbStats := &bson.M{}
	err := s.DB(params["Database"]).Run(&bson.D{
		bson.DocElem{
			Name:  "dbStats",
			Value: 1,
		},
		bson.DocElem{
			Name:  "maxTimeMS",
			Value: s.GetMaxTimeMS(),
		},
	}, dbStats)

	if err != nil {
		return nil, zbxerr.ErrorCannotFetchData.Wrap(err)
	}

	jsonRes, err := json.Marshal(dbStats)
	if err != nil {
		return nil, zbxerr.ErrorCannotMarshalJSON.Wrap(err)
	}

	return string(jsonRes), nil
}
