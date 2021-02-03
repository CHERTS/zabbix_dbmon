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

package oracle

import (
	"context"

	"zabbix.com/pkg/zbxerr"
)

func instanceHandler(ctx context.Context, conn OraClient, params map[string]string, _ ...string) (interface{}, error) {
	var instanceStats string

	row, err := conn.QueryRow(ctx, `
		SELECT
			JSON_OBJECT(
				'instance' VALUE INSTANCE_NAME, 
				'hostname' VALUE HOST_NAME, 
				'version'  VALUE VERSION || '-' || EDITION, 
				'uptime'   VALUE FLOOR((SYSDATE - STARTUP_TIME) * 60 * 60 * 24), 
				'status'   VALUE DECODE(STATUS, 'STARTED', 1, 'MOUNTED', 2, 'OPEN', 3, 'OPEN MIGRATE', 4, 0), 
				'archiver' VALUE DECODE(ARCHIVER, 'STOPPED', 1, 'STARTED', 2, 'FAILED', 3, 0), 
				'role'     VALUE DECODE(INSTANCE_ROLE, 'PRIMARY_INSTANCE', 1, 'SECONDARY_INSTANCE', 2, 0)
			)
		FROM
			V$INSTANCE
	`)
	if err != nil {
		return nil, zbxerr.ErrorCannotFetchData.Wrap(err)
	}

	err = row.Scan(&instanceStats)
	if err != nil {
		return nil, zbxerr.ErrorCannotFetchData.Wrap(err)
	}

	return instanceStats, nil
}
