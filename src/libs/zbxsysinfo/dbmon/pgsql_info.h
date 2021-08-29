/*
** Zabbix
** Copyright (C) 2019-2021 Mikhail Grigorev
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

#ifndef ZABBIX_PGINFO_H
#define ZABBIX_PGINFO_H

#include "sysinfo.h"

extern ZBX_METRIC	parameters_dbmon_pgsql[];

int	PGSQL_PING(AGENT_REQUEST *request, AGENT_RESULT *result);
int	PGSQL_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result);
int	PGSQL_DB_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);
int	PGSQL_GET_RESULT(AGENT_REQUEST *request, AGENT_RESULT *result);
int	PGSQL_QUERY(AGENT_REQUEST *request, AGENT_RESULT *result);

#endif /* ZABBIX_PGINFO_H */
