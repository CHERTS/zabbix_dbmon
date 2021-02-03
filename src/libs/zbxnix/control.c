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

#include "control.h"
#include "zbxdiag.h"

static int	parse_log_level_options(const char *opt, size_t len, unsigned int *scope, unsigned int *data)
{
	unsigned short	num = 0;
	const char	*rtc_options;

	rtc_options = opt + len;

	if ('\0' == *rtc_options)
	{
		*scope = ZBX_RTC_LOG_SCOPE_FLAG | ZBX_RTC_LOG_SCOPE_PID;
		*data = 0;
	}
	else if ('=' != *rtc_options)
	{
		zbx_error("invalid runtime control option: %s", opt);
		return FAIL;
	}
	else if (0 != isdigit(*(++rtc_options)))
	{
		/* convert PID */
		if (FAIL == is_ushort(rtc_options, &num) || 0 == num)
		{
			zbx_error("invalid log level control target: invalid or unsupported process identifier");
			return FAIL;
		}

		*scope = ZBX_RTC_LOG_SCOPE_FLAG | ZBX_RTC_LOG_SCOPE_PID;
		*data = num;
	}
	else
	{
		char	*proc_name = NULL, *proc_num;
		int	proc_type;

		if ('\0' == *rtc_options)
		{
			zbx_error("invalid log level control target: unspecified process identifier or type");
			return FAIL;
		}

		proc_name = zbx_strdup(proc_name, rtc_options);

		if (NULL != (proc_num = strchr(proc_name, ',')))
			*proc_num++ = '\0';

		if ('\0' == *proc_name)
		{
			zbx_error("invalid log level control target: unspecified process type");
			zbx_free(proc_name);
			return FAIL;
		}

		if (ZBX_PROCESS_TYPE_UNKNOWN == (proc_type = get_process_type_by_name(proc_name)))
		{
			zbx_error("invalid log level control target: unknown process type \"%s\"", proc_name);
			zbx_free(proc_name);
			return FAIL;
		}

		if (NULL != proc_num)
		{
			if ('\0' == *proc_num)
			{
				zbx_error("invalid log level control target: unspecified process number");
				zbx_free(proc_name);
				return FAIL;
			}

			/* convert Zabbix process number (e.g. "2" in "poller,2") */
			if (FAIL == is_ushort(proc_num, &num) || 0 == num)
			{
				zbx_error("invalid log level control target: invalid or unsupported process number"
						" \"%s\"", proc_num);
				zbx_free(proc_name);
				return FAIL;
			}
		}

		zbx_free(proc_name);

		*scope = ZBX_RTC_LOG_SCOPE_PROC | (unsigned int)proc_type;
		*data = num;
	}

	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Function: parse_rtc_options                                                *
 *                                                                            *
 * Purpose: parse runtime control options and create a runtime control        *
 *          message                                                           *
 *                                                                            *
 * Parameters: opt          - [IN] the command line argument                  *
 *             program_type - [IN] the program type                           *
 *             message      - [OUT] the message containing options for log    *
 *                                  level change or cache reload              *
 *                                                                            *
 * Return value: SUCCEED - the message was created successfully               *
 *               FAIL    - an error occurred                                  *
 *                                                                            *
 ******************************************************************************/
int	parse_rtc_options(const char *opt, unsigned char program_type, int *message)
{
	unsigned int	scope, data, command;

	if (0 == strncmp(opt, ZBX_LOG_LEVEL_INCREASE, ZBX_CONST_STRLEN(ZBX_LOG_LEVEL_INCREASE)))
	{
		command = ZBX_RTC_LOG_LEVEL_INCREASE;

		if (SUCCEED != parse_log_level_options(opt, ZBX_CONST_STRLEN(ZBX_LOG_LEVEL_INCREASE), &scope, &data))
			return FAIL;
	}
	else if (0 == strncmp(opt, ZBX_LOG_LEVEL_DECREASE, ZBX_CONST_STRLEN(ZBX_LOG_LEVEL_DECREASE)))
	{
		command = ZBX_RTC_LOG_LEVEL_DECREASE;

		if (SUCCEED != parse_log_level_options(opt, ZBX_CONST_STRLEN(ZBX_LOG_LEVEL_DECREASE), &scope, &data))
			return FAIL;
	}
	else if (0 != (program_type & (ZBX_PROGRAM_TYPE_SERVER | ZBX_PROGRAM_TYPE_PROXY)) &&
			0 == strcmp(opt, ZBX_CONFIG_CACHE_RELOAD))
	{
		command = ZBX_RTC_CONFIG_CACHE_RELOAD;
		scope = 0;
		data = 0;
	}
	else if (0 != (program_type & (ZBX_PROGRAM_TYPE_SERVER | ZBX_PROGRAM_TYPE_PROXY)) &&
			0 == strcmp(opt, ZBX_HOUSEKEEPER_EXECUTE))
	{
		command = ZBX_RTC_HOUSEKEEPER_EXECUTE;
		scope = 0;
		data = 0;
	}
	else if (0 != (program_type & (ZBX_PROGRAM_TYPE_SERVER | ZBX_PROGRAM_TYPE_PROXY)) &&
			0 == strcmp(opt, ZBX_SNMP_CACHE_RELOAD))
	{
#ifdef HAVE_NETSNMP
		command = ZBX_RTC_SNMP_CACHE_RELOAD;
		/* Scope is ignored for SNMP. R/U pollers, trapper, discoverer and taskmanager always get targeted. */
		scope = 0;
		data = 0;
#else
		zbx_error("invalid runtime control option: no SNMP support enabled");
		return FAIL;
#endif
	}
	else if (0 != (program_type & (ZBX_PROGRAM_TYPE_SERVER | ZBX_PROGRAM_TYPE_PROXY)) &&
			0 == strncmp(opt, ZBX_DIAGINFO, ZBX_CONST_STRLEN(ZBX_DIAGINFO)))
	{
		command = ZBX_RTC_DIAGINFO;
		data = 0;
		scope = ZBX_DIAGINFO_ALL;

		if ('=' == opt[ZBX_CONST_STRLEN(ZBX_DIAGINFO)])
		{
			const char	*section = opt + ZBX_CONST_STRLEN(ZBX_DIAGINFO) + 1;

			if (0 == strcmp(section, ZBX_DIAG_HISTORYCACHE))
			{
				scope = ZBX_DIAGINFO_HISTORYCACHE;
			}
			else if (0 == strcmp(section, ZBX_DIAG_PREPROCESSING))
			{
				scope = ZBX_DIAGINFO_PREPROCESSING;
			}
			else if (0 == strcmp(section, ZBX_DIAG_LOCKS))
			{
				scope = ZBX_DIAGINFO_LOCKS;
			}
			else if (0 != (program_type & (ZBX_PROGRAM_TYPE_SERVER)))
			{
				if (0 == strcmp(section, ZBX_DIAG_VALUECACHE))
					scope = ZBX_DIAGINFO_VALUECACHE;
				else if (0 == strcmp(section, ZBX_DIAG_LLD))
					scope = ZBX_DIAGINFO_LLD;
				else if (0 == strcmp(section, ZBX_DIAG_ALERTING))
					scope = ZBX_DIAGINFO_ALERTING;
			}

			if (0 == scope)
			{
				zbx_error("invalid diaginfo section: %s", section);
				return FAIL;
			}
		}
		else if ('\0' != opt[ZBX_CONST_STRLEN(ZBX_DIAGINFO)])
		{
			zbx_error("invalid runtime control option: %s", opt);
			return FAIL;
		}
	}
	else
	{
		zbx_error("invalid runtime control option: %s", opt);
		return FAIL;
	}

	*message = (int)ZBX_RTC_MAKE_MESSAGE(command, scope, data);

	return SUCCEED;
}
