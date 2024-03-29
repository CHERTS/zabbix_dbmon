/*
** Zabbix
** Copyright (C) 2019-2022 Mikhail Grigorev
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

#ifndef ZABBIX_DBMON_PARAMS_H
#define ZABBIX_DBMON_PARAMS_H

#include "sysinfo.h"

typedef char** DBMONparams;

int dbmon_param_len(DBMONparams params);
char **dbmon_param_append(DBMONparams params, char *s);
void dbmon_param_free(DBMONparams params);

#endif /* ZABBIX_DBMON_CONFIG_H */
