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

#include "common.h"
#include "sysinfo.h"
#include "module.h"
#include "dbmon_params.h"

int dbmon_param_len(DBMONparams params)
{
	int			len = 0;
	DBMONparams	c = NULL;

	if (NULL == params)
		return 0;

	for (c = params; *c; c++) 
		len++;

	return len;
}

char **dbmon_param_append(DBMONparams params, char *s)
{
	int len = 0;

	// never append nulls or empty strings
	if(NULL == s || '\0' == *s)
		return params;

	// allocate new array
	if(NULL == params)
	{
		params = zbx_malloc(params, sizeof(DBMONparams) * 2);
		params[0] = strdup(s);
		params[1] = NULL;
		return params;
	} 

	// extend array and append
	len = dbmon_param_len(params);
	params = zbx_realloc(params, sizeof(DBMONparams) * (len + 2));
	params[len] = strdup(s); // dup so we can free everything later
	params[len + 1] = NULL;

	return params;
}

void dbmon_param_free(DBMONparams params)
{
	DBMONparams p = NULL;

	if (NULL == params)
		return;
    
	for (p = params; *p; p++)
		zbx_free(*p);

	zbx_free(params);
}
