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

#ifndef ZABBIX_DBMON_CONFIG_H
#define ZABBIX_DBMON_CONFIG_H

#include "sysinfo.h"

const char *get_query_by_name(const char *key);
int init_dbmon_config();
int uninit_dbmon_config();

#endif /* ZABBIX_DBMON_CONFIG_H */
