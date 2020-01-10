/*
** Zabbix
** Copyright (C) 2019 Mikhail Grigorev
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

int make_discovery_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result);
int make_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result);
int make_onerow_json_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result);
int make_multirow_twocoll_json_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result);
int make_multi_json_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result);
char *get_str_one_result(AGENT_REQUEST *request, AGENT_RESULT *result, const unsigned int row, const unsigned int col, struct zbx_db_result db_result);
unsigned int get_int_one_result(AGENT_REQUEST *request, AGENT_RESULT *result, const unsigned int row, const unsigned int col, struct zbx_db_result db_result);
int zbx_db_compare_version(char *version1, char *version2);

#endif /* ZABBIX_DBMON_COMMON_H */
