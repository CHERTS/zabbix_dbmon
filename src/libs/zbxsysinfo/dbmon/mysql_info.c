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
#include "mysql_info.h"
#include "dbmon_common.h"

#if defined(HAVE_DBMON)
#if defined(HAVE_MYSQL)

extern char	*CONFIG_MYSQL_USER;
extern char	*CONFIG_MYSQL_PASSWORD;

#define MYSQL_DEFAULT_USER		"root"
#define MYSQL_DEFAULT_PASSWORD	"password"
#define MYSQL_DEFAULT_DBNAME	"mysql"

#define MYSQL_VERSION_DBS "SELECT /*DBS_001*/ VERSION() AS VERSION;"

#define MYSQL_STARTUPTIME_DBS "\
SELECT /*DBS_002*/ UNIX_TIMESTAMP()-VARIABLE_VALUE AS STARTUPTIME FROM information_schema.global_status WHERE VARIABLE_NAME='UPTIME';"

#define MYSQL_V57_V80_STARTUPTIME_DBS "\
SELECT /*DBS_002*/ UNIX_TIMESTAMP()-VARIABLE_VALUE AS STARTUPTIME FROM performance_schema.global_status WHERE VARIABLE_NAME='UPTIME';"

#define MYSQL_DISCOVER_DBS "\
SELECT SCHEMA_NAME AS DBNAME, DEFAULT_CHARACTER_SET_NAME AS DB_CHARACTER_SET, DEFAULT_COLLATION_NAME AS DB_COLLATION_NAME \
FROM information_schema.schemata;"

ZBX_METRIC	parameters_dbmon_mysql[] =
/*	KEY			FLAG		FUNCTION		TEST PARAMETERS */
{
	{"mysql.ping",			CF_HAVEPARAMS,		MYSQL_PING,			NULL},
	{"mysql.version",		CF_HAVEPARAMS,		MYSQL_VERSION,		NULL},
	{"mysql.startuptime",	CF_HAVEPARAMS,		MYSQL_GET_RESULT,	NULL},
	{"mysql.db.discovery",	CF_HAVEPARAMS,		MYSQL_DB_DISCOVERY,	NULL},
	{NULL}
};

#if !defined(_WINDOWS) && !defined(__MINGW32__)
static int	mysql_ping(AGENT_REQUEST *request, AGENT_RESULT *result)
#else
static int	mysql_ping(AGENT_REQUEST *request, AGENT_RESULT *result, HANDLE timeout_event)
#endif
{
	int							ret = SYSINFO_RET_FAIL, ping = 0;
	char						*mysql_host, *mysql_str_port;
	unsigned short				mysql_port = 0;
	struct zbx_db_connection	*mysql_conn;

	if (2 < request->nparam)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Too many parameters."));
		return SYSINFO_RET_FAIL;
	}

	if (2 > request->nparam)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Too few parameters."));
		return SYSINFO_RET_FAIL;
	}

	if (NULL == CONFIG_MYSQL_USER)
		CONFIG_MYSQL_USER = zbx_strdup(CONFIG_MYSQL_USER, MYSQL_DEFAULT_USER);
	if (NULL == CONFIG_MYSQL_PASSWORD)
		CONFIG_MYSQL_PASSWORD = zbx_strdup(CONFIG_MYSQL_PASSWORD, MYSQL_DEFAULT_PASSWORD);

	mysql_host = get_rparam(request, 0);
	mysql_str_port = get_rparam(request, 1);

	if (NULL == mysql_host || '\0' == *mysql_host)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid first parameter (host)."));
		return SYSINFO_RET_FAIL;
	}

	if (NULL != mysql_str_port && '\0' != *mysql_str_port && SUCCEED != is_ushort(mysql_str_port, &mysql_port))
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid second parameter (port)."));
		return SYSINFO_RET_FAIL;
	}

#if defined(_WINDOWS) && defined(__MINGW32__)
	/* 'timeout_event' argument is here to make the mysql_ping() prototype as required by */
	/* zbx_execute_threaded_metric() on MS Windows */
	ZBX_UNUSED(timeout_event);
#endif

	mysql_conn = zbx_db_connect_mysql(mysql_host, CONFIG_MYSQL_USER, CONFIG_MYSQL_PASSWORD, MYSQL_DEFAULT_DBNAME, mysql_port, NULL);

	if (NULL != mysql_conn)
	{
		SET_UI64_RESULT(result, 1);
	}
	else
	{
		SET_UI64_RESULT(result, 0);
	}

	zbx_db_close_db(mysql_conn);
	zbx_db_clean_connection(mysql_conn);

	return SYSINFO_RET_OK;
}

int	MYSQL_PING(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	return zbx_execute_threaded_metric(mysql_ping, request, result);
}

int	MYSQL_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int							ret = SYSINFO_RET_FAIL;
	char						*mysql_host, *mysql_str_port, *mysql_ver;
	unsigned short				mysql_port = 0;
	struct zbx_db_connection	*mysql_conn;
	//struct zbx_db_result		mysql_result;

	if (2 < request->nparam)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Too many parameters."));
		return SYSINFO_RET_FAIL;
	}

	if (2 > request->nparam)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Too few parameters."));
		return SYSINFO_RET_FAIL;
	}

	if (NULL == CONFIG_MYSQL_USER)
		CONFIG_MYSQL_USER = zbx_strdup(CONFIG_MYSQL_USER, MYSQL_DEFAULT_USER);
	if (NULL == CONFIG_MYSQL_PASSWORD)
		CONFIG_MYSQL_PASSWORD = zbx_strdup(CONFIG_MYSQL_PASSWORD, MYSQL_DEFAULT_PASSWORD);

	mysql_host = get_rparam(request, 0);
	mysql_str_port = get_rparam(request, 1);

	if (NULL == mysql_host || '\0' == *mysql_host)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid first parameter (host)."));
		return SYSINFO_RET_FAIL;
	}

	if (NULL != mysql_str_port && '\0' != *mysql_str_port && SUCCEED != is_ushort(mysql_str_port, &mysql_port))
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid second parameter (port)."));
		return SYSINFO_RET_FAIL;
	}

	mysql_conn = zbx_db_connect_mysql(mysql_host, CONFIG_MYSQL_USER, CONFIG_MYSQL_PASSWORD, MYSQL_DEFAULT_DBNAME, mysql_port, NULL);

	if (NULL != mysql_conn)
	{
		mysql_ver = zbx_db_version(mysql_conn);

		if (NULL != mysql_ver)
		{
			zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): MySQL version: %s", __func__, request->key, mysql_ver);
			SET_TEXT_RESULT(result, zbx_strdup(NULL, mysql_ver));
			ret = SYSINFO_RET_OK;
		}
		else
		{
			zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get MySQL version", __func__, request->key);
			SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get MySQL version"));
			ret = SYSINFO_RET_FAIL;
		}
		/*if (ZBX_DB_OK == zbx_db_query_select(mysql_conn, &mysql_result, MYSQL_VERSION_DBS))
		{
			ret = make_result(request, result, mysql_result);
			zbx_db_clean_result(&mysql_result);
		}
		else
		{
			zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error executing query", __func__, request->key);
			SET_MSG_RESULT(result, zbx_strdup(NULL, "Error executing query"));
			ret = SYSINFO_RET_FAIL;
		}*/
	}
	else
	{
		zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error connecting to MySQL database", __func__, request->key);
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Error connecting to database"));
		ret = SYSINFO_RET_FAIL;
	}

	zbx_db_close_db(mysql_conn);
	zbx_db_clean_connection(mysql_conn);

	return ret;
}

int	mysql_get_discovery(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query)
{
	int							ret = SYSINFO_RET_FAIL;
	char						*mysql_host, *mysql_str_port, *c = NULL;
	unsigned short				mysql_port = 0;
	struct zbx_db_connection	*mysql_conn;
	struct zbx_db_result		mysql_result;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);

	if (2 < request->nparam)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Too many parameters."));
		return SYSINFO_RET_FAIL;
	}

	if (2 > request->nparam)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Too few parameters."));
		return SYSINFO_RET_FAIL;
	}

	if (NULL == CONFIG_MYSQL_USER)
		CONFIG_MYSQL_USER = zbx_strdup(CONFIG_MYSQL_USER, MYSQL_DEFAULT_USER);
	if (NULL == CONFIG_MYSQL_PASSWORD)
		CONFIG_MYSQL_PASSWORD = zbx_strdup(CONFIG_MYSQL_PASSWORD, MYSQL_DEFAULT_PASSWORD);

	mysql_host = get_rparam(request, 0);
	mysql_str_port = get_rparam(request, 1);

	if (NULL == mysql_host || '\0' == *mysql_host)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid first parameter (host)."));
		return SYSINFO_RET_FAIL;
	}

	if (NULL != mysql_str_port && '\0' != *mysql_str_port && SUCCEED != is_ushort(mysql_str_port, &mysql_port))
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid second parameter (port)."));
		return SYSINFO_RET_FAIL;
	}

	mysql_conn = zbx_db_connect_mysql(mysql_host, CONFIG_MYSQL_USER, CONFIG_MYSQL_PASSWORD, MYSQL_DEFAULT_DBNAME, mysql_port, NULL);

	if (NULL != mysql_conn)
	{
		if (ZBX_DB_OK == zbx_db_query_select(mysql_conn, &mysql_result, query))
		{
			ret = make_discovery_result(request, result, mysql_result);
			zbx_db_clean_result(&mysql_result);
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

	zbx_db_close_db(mysql_conn);
	zbx_db_clean_connection(mysql_conn);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __func__, request->key);

	return ret;
}

int	MYSQL_DB_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int ret = SYSINFO_RET_FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __func__);

	ret = mysql_get_discovery(request, result, MYSQL_DISCOVER_DBS);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __func__);

	return ret;
}

static int	mysql_make_result(AGENT_REQUEST *request, AGENT_RESULT *result, char *query, zbx_db_result_type result_type)
{
	int							ret = SYSINFO_RET_FAIL, ping = 0;
	char						*mysql_host, *mysql_str_port;
	unsigned short				mysql_port = 0;
	struct zbx_db_connection	*mysql_conn;
	struct zbx_db_result		mysql_result;

	if (NULL == CONFIG_MYSQL_USER)
		CONFIG_MYSQL_USER = zbx_strdup(CONFIG_MYSQL_USER, MYSQL_DEFAULT_USER);
	if (NULL == CONFIG_MYSQL_PASSWORD)
		CONFIG_MYSQL_PASSWORD = zbx_strdup(CONFIG_MYSQL_PASSWORD, MYSQL_DEFAULT_PASSWORD);

	mysql_host = get_rparam(request, 0);
	mysql_str_port = get_rparam(request, 1);

	if (NULL == mysql_host || '\0' == *mysql_host)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid first parameter (host)."));
		return SYSINFO_RET_FAIL;
	}

	if (NULL != mysql_str_port && '\0' != *mysql_str_port && SUCCEED != is_ushort(mysql_str_port, &mysql_port))
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid second parameter (port)."));
		return SYSINFO_RET_FAIL;
	}

	mysql_conn = zbx_db_connect_mysql(mysql_host, CONFIG_MYSQL_USER, CONFIG_MYSQL_PASSWORD, MYSQL_DEFAULT_DBNAME, mysql_port, NULL);

	if (NULL != mysql_conn)
	{
		if (ZBX_DB_OK == zbx_db_query_select(mysql_conn, &mysql_result, query))
		{
			switch (result_type)
			{
			case ZBX_DB_RES_TYPE_ONEROW:
				ret = make_onerow_json_result(request, result, mysql_result);
				break;
			case ZBX_DB_RES_TYPE_TWOCOLL:
				ret = make_multirow_twocoll_json_result(request, result, mysql_result);
				break;
			case ZBX_DB_RES_TYPE_MULTIROW:
				ret = make_multi_json_result(request, result, mysql_result);
				break;
			default:
				ret = make_result(request, result, mysql_result);
				break;
			}

			zbx_db_clean_result(&mysql_result);
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
		zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error connecting to MySQL database", __func__, request->key);
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Error connecting to database"));
		ret = SYSINFO_RET_FAIL;
	}

	zbx_db_close_db(mysql_conn);
	zbx_db_clean_connection(mysql_conn);

	return ret;
}

#if !defined(_WINDOWS) && !defined(__MINGW32__)
static int	mysql_get_result(AGENT_REQUEST *request, AGENT_RESULT *result)
#else
static int	mysql_get_result(AGENT_REQUEST *request, AGENT_RESULT *result, HANDLE timeout_event)
#endif
{
	int ret = SYSINFO_RET_FAIL;

#if defined(_WINDOWS) && defined(__MINGW32__)
	/* 'timeout_event' argument is here to make the oracle_get_instance_result() prototype as required by */
	/* zbx_execute_threaded_metric() on MS Windows */
	ZBX_UNUSED(timeout_event);
#endif

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);

	if (0 == strcmp(request->key, (const char*)"mysql.version"))
	{
		ret = mysql_make_result(request, result, MYSQL_VERSION_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, (const char*)"mysql.startuptime"))
	{
		ret = mysql_make_result(request, result, MYSQL_STARTUPTIME_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Unknown request key"));
		ret = SYSINFO_RET_FAIL;
	}

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __func__, request->key);

	return ret;
}

int	MYSQL_GET_RESULT(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	return zbx_execute_threaded_metric(mysql_get_result, request, result);
}

#endif
#endif