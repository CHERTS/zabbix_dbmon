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

#ifndef ZABBIX_DBMON_COMMON_H
#define ZABBIX_DBMON_COMMON_H

#include "sysinfo.h"

typedef enum
{
	ZBX_DB_RES_TYPE_UNDEFINED = 0,
	ZBX_DB_RES_TYPE_NOJSON,
	ZBX_DB_RES_TYPE_ONEROW,
	ZBX_DB_RES_TYPE_TWOCOLL,
	ZBX_DB_RES_TYPE_MULTIROW,
	ZBX_DB_RES_TYPE_DISCOVERY
}
zbx_db_result_type;

int make_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result, zbx_db_result_type mode);
char *get_str_one_result(AGENT_REQUEST *request, AGENT_RESULT *result, const unsigned int row, const unsigned int col, struct zbx_db_result db_result);
unsigned int get_int_one_result(AGENT_REQUEST *request, AGENT_RESULT *result, const unsigned int row, const unsigned int col, struct zbx_db_result db_result);
int dbmon_log_result(AGENT_RESULT *result, int level, const char *format, ...);

#endif /* ZABBIX_DBMON_COMMON_H */
