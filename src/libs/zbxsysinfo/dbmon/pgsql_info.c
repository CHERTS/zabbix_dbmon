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
#include "log.h"
#include "module.h"
#include "zbxdbmon.h"
#include "pgsql_info.h"
#include "dbmon_common.h"

#if defined(HAVE_DBMON)
#if defined(HAVE_POSTGRESQL)

#define PG_VERSION_DBS "SELECT VERSION() AS VERSION;"

#define PG_DISCOVER_DBS  "\
SELECT \
    d.oid as oid, \
    d.datname as path, \
    d.datname as database, \
    pg_catalog.pg_encoding_to_char(d.encoding) as encoding, \
    d.datcollate as lc_collate, \
    d.datctype as lc_ctype, \
    pg_catalog.pg_get_userbyid(d.datdba) as owner, \
    t.spcname as tablespace, \
    pg_catalog.shobj_description(d.oid, 'pg_database') as description \
FROM pg_catalog.pg_database d \
    JOIN pg_catalog.pg_tablespace t on d.dattablespace = t.oid \
WHERE \
    d.datallowconn = 't' \
    AND d.datistemplate = 'n' \
ORDER BY 1;"

ZBX_METRIC	parameters_dbmon_pgsql[] =
/*	KEY			FLAG		FUNCTION		TEST PARAMETERS */
{
	{"pg.ping",			CF_HAVEPARAMS,		PGSQL_PING,			NULL},
	{"pg.version",		CF_HAVEPARAMS,		PGSQL_VERSION,			NULL},
	{"pg.version.full",	CF_HAVEPARAMS,		PGSQL_VERSION,			NULL},
	{"pg.db.discovery",	CF_HAVEPARAMS,		PGSQL_DB_DISCOVERY,	NULL},
	{NULL}
};

#if !defined(_WINDOWS) && !defined(__MINGW32__)
static int	pgsql_ping(AGENT_REQUEST *request, AGENT_RESULT *result)
#else
static int	pgsql_ping(AGENT_REQUEST *request, AGENT_RESULT *result, HANDLE timeout_event)
#endif
{
	int							ret = SYSINFO_RET_FAIL, ping = 0;
	char						*pg_conn_string;
	struct zbx_db_connection	*pgsql_conn;

	if (1 < request->nparam)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Too many parameters."));
		return SYSINFO_RET_FAIL;
	}

	if (1 > request->nparam)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Too few parameters."));
		return SYSINFO_RET_FAIL;
	}

	pg_conn_string = get_rparam(request, 0);

	if (NULL == pg_conn_string || '\0' == *pg_conn_string)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid first parameter (pg_conn_string)."));
		return SYSINFO_RET_FAIL;
	}

#if defined(_WINDOWS) && defined(__MINGW32__)
	/* 'timeout_event' argument is here to make the pgsql_ping() prototype as required by */
	/* zbx_execute_threaded_metric() on MS Windows */
	ZBX_UNUSED(timeout_event);
#endif

	pgsql_conn = zbx_db_connect_pgsql(pg_conn_string);

	if (NULL != pgsql_conn)
	{
		SET_UI64_RESULT(result, 1);
	}
	else
	{
		SET_UI64_RESULT(result, 0);
	}

	zbx_db_close_db(pgsql_conn);
	zbx_db_clean_connection(pgsql_conn);

	return SYSINFO_RET_OK;
}

int	PGSQL_PING(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	return zbx_execute_threaded_metric(pgsql_ping, request, result);
}

static int	pgsql_version(AGENT_REQUEST *request, AGENT_RESULT *result, unsigned int ver_mode)
{
	int							ret = SYSINFO_RET_FAIL;
	char						*pg_conn_string;
	struct zbx_db_connection	*pgsql_conn;
	struct zbx_db_result		pgsql_result;
	char						pgsql_ver[10];
	unsigned long				version;

	if (1 < request->nparam)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Too many parameters."));
		return SYSINFO_RET_FAIL;
	}

	if (1 > request->nparam)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Too few parameters."));
		return SYSINFO_RET_FAIL;
	}

	pg_conn_string = get_rparam(request, 0);

	if (NULL == pg_conn_string || '\0' == *pg_conn_string)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid first parameter (pg_conn_string)."));
		return SYSINFO_RET_FAIL;
	}

	pgsql_conn = zbx_db_connect_pgsql(pg_conn_string);

	if (NULL != pgsql_conn)
	{
		if (0 == ver_mode)
		{
			/*
			version 9.5.2	=> 90502
			version 9.6.0	=> 90600
			version 9.6.16	=> 90616
			version 10.5	=> 100500
			version 10.11	=> 100011
			version 12.1	=> 120001
			*/
			version = zbx_db_version(pgsql_conn);

			if (version < 100000)
			{
				zbx_snprintf(pgsql_ver, sizeof(pgsql_ver), "%d.%d.%d", version / 10000, (version % 10000) / 100, (version % 10000) % 100);
			}
			else
			{
				zbx_snprintf(pgsql_ver, sizeof(pgsql_ver), "%d.%d.%d", version / 10000, ((version % 10000) / 100) == 0 ? (version % 10000) % 100 : (version % 10000) / 100, ((version % 10000) / 100) == 0 ? (version % 10000) / 100 : (version % 10000) % 100);
			}

			if (NULL != pgsql_ver)
			{
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): PgSQL version: %s", __func__, request->key, pgsql_ver);
				SET_TEXT_RESULT(result, zbx_strdup(NULL, pgsql_ver));
				ret = SYSINFO_RET_OK;
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL version", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL version"));
				ret = SYSINFO_RET_FAIL;
			}
		}
		else
		{
			if (ZBX_DB_OK == zbx_db_query_select(pgsql_conn, &pgsql_result, PG_VERSION_DBS))
			{
				ret = make_result(request, result, pgsql_result);
				zbx_db_clean_result(&pgsql_result);
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error executing query", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error executing query"));
				ret = SYSINFO_RET_FAIL;
			}
		}
	}
	else
	{
		zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error connecting to database", __func__, request->key);
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Error connecting to database"));
		ret = SYSINFO_RET_FAIL;
	}

	zbx_db_close_db(pgsql_conn);
	zbx_db_clean_connection(pgsql_conn);

	return ret;
}

int	PGSQL_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int ret = SYSINFO_RET_FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);

	if (0 == strcmp(request->key, (const char*)"pg.version"))
	{
		ret = pgsql_version(request, result, 0);
	}
	else if (0 == strcmp(request->key, (const char*)"pg.version.full"))
	{
		ret = pgsql_version(request, result, 1);
	}
	else
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Unknown request key"));
		ret = SYSINFO_RET_FAIL;
	}

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __func__, request->key);

	return ret;
}

static int	pgsql_get_discovery(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query)
{
	int							ret = SYSINFO_RET_FAIL;
	char						*pgsql_conn_string, *c = NULL;
	struct zbx_db_connection	*pgsql_conn;
	struct zbx_db_result		pgsql_result;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);

	if (1 < request->nparam)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Too many parameters."));
		return SYSINFO_RET_FAIL;
	}

	if (1 > request->nparam)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Too few parameters."));
		return SYSINFO_RET_FAIL;
	}

	pgsql_conn_string = get_rparam(request, 0);

	if (NULL == pgsql_conn_string || '\0' == *pgsql_conn_string)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid first parameter (pg_conn_string)."));
		return SYSINFO_RET_FAIL;
	}

	pgsql_conn = zbx_db_connect_pgsql(pgsql_conn_string);

	if (NULL != pgsql_conn)
	{
		if (ZBX_DB_OK == zbx_db_query_select(pgsql_conn, &pgsql_result, query))
		{
			ret = make_discovery_result(request, result, pgsql_result);
			zbx_db_clean_result(&pgsql_result);
		}
		else
		{
			zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error executing query", __func__, request->key);
			SET_MSG_RESULT(result, zbx_strdup(NULL, "Error executing query"));
			ret = SYSINFO_RET_FAIL;
		}
	}
	else
	{
		zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error connecting to database", __func__, request->key);
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Error connecting to database"));
		ret = SYSINFO_RET_FAIL;
	}

	zbx_db_close_db(pgsql_conn);
	zbx_db_clean_connection(pgsql_conn);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __func__, request->key);

	return ret;
}

int	PGSQL_DB_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int ret = SYSINFO_RET_FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __func__);

	ret = pgsql_get_discovery(request, result, PG_DISCOVER_DBS);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __func__);

	return ret;
}

#endif
#endif