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
#include "dbmon_config.h"
#include "dbmon_params.h"

#if defined(HAVE_DBMON)
#if defined(HAVE_MYSQL)

extern char	*CONFIG_MYSQL_USER;
extern char	*CONFIG_MYSQL_PASSWORD;

int init_config_done = 1;

#define MYSQL_DEFAULT_USER		"root"
#define MYSQL_DEFAULT_PASSWORD	"password"
#define MYSQL_DEFAULT_DBNAME	"mysql"

#define MYSQL_VERSION_DBS "SELECT /*DBS_001*/ VERSION() AS VERSION;"

#define MYSQL_SERVER_INFO_DBS "\
SELECT /*DBS_002*/ @@server_id AS SERVER_ID, \
	UUID() AS SERVER_UUID, \
	VERSION() AS VERSION, \
	CAST(SUM(TRUNCATE(UNIX_TIMESTAMP()-VARIABLE_VALUE, 0))/COUNT(TRUNCATE(UNIX_TIMESTAMP()-VARIABLE_VALUE, 0)) AS UNSIGNED) AS STARTUPTIME \
FROM %s.global_status \
WHERE VARIABLE_NAME='UPTIME';"

#define MYSQL_GLOBAL_STATUS_DBS "SHOW GLOBAL STATUS;"

#define MYSQL_GLOBAL_VARIABLES_DBS "SHOW GLOBAL VARIABLES;"

#define MYSQL_DB_DISCOVERY_DBS "\
SELECT /*DBS_005*/ SCHEMA_NAME AS DBNAME, \
	DEFAULT_CHARACTER_SET_NAME AS DB_CHARACTER_SET, \
	DEFAULT_COLLATION_NAME AS DB_COLLATION_NAME \
FROM information_schema.schemata \
WHERE SCHEMA_NAME NOT REGEXP '(information_schema|performance_schema)' GROUP BY SCHEMA_NAME;"

#define MYSQL_DB_INFO_DBS "\
SELECT /*DBS_006*/ s.SCHEMA_NAME AS DBNAME,\
	s.DEFAULT_CHARACTER_SET_NAME AS DB_CHARACTER_SET, \
	s.DEFAULT_COLLATION_NAME AS DB_COLLATION_NAME, \
	COALESCE(SUM(t.DATA_FREE),0) AS DB_DATA_FREE, \
	COALESCE(SUM(t.DATA_LENGTH), 0) AS DB_DATA_LENGTH, \
	COALESCE(SUM(t.INDEX_LENGTH), 0) AS DB_INDEX_LENGTH, \
	COALESCE(SUM(t.DATA_LENGTH + t.INDEX_LENGTH), 0) AS DB_SIZE \
FROM information_schema.schemata s INNER JOIN information_schema.tables t ON s.SCHEMA_NAME = t.TABLE_SCHEMA \
WHERE s.SCHEMA_NAME NOT REGEXP '(information_schema|performance_schema)' GROUP BY s.SCHEMA_NAME;"

// SELECT /*DBS_007*/ IF(VARIABLE_VALUE = '', CONCAT(@@datadir,'error.log'), CASE WHEN VARIABLE_VALUE REGEXP '^.\\\\' THEN CONCAT(@@datadir,REPLACE(VARIABLE_VALUE, '.\\', ''))  ELSE VARIABLE_VALUE END) AS LOG_ERROR FROM information_schema.global_variables WHERE VARIABLE_NAME = 'log_error';
#define MYSQL_ERRORLOG_DISCOVERY_DBS "\
SELECT /*DBS_007*/ IF(VARIABLE_VALUE = '', CONCAT(@@datadir,'error.log'), CASE WHEN VARIABLE_VALUE REGEXP '^.\\\\\\\\' THEN CONCAT(@@datadir,REPLACE(VARIABLE_VALUE, '.\\\\', '')) ELSE VARIABLE_VALUE END) AS LOG_ERROR \
FROM %s.global_variables \
WHERE VARIABLE_NAME = 'log_error';"

#define MYSQL_TOP10_TABLE_BY_SIZE_DBS "\
SELECT /*DBS_008*/ CAST(@rn:=@rn+1 AS DECIMAL(0)) AS TOP, TABLE_SCHEMA AS DB_NAME, TABLE_NAME, ENGINE AS TABLE_ENGINE, IFNULL(TABLE_ROWS,0) AS TABLE_ROWS, DATA_FREE AS DATA_FREE_SIZE, DATA_LENGTH AS DATA_SIZE, INDEX_LENGTH AS INDEX_SIZE, (DATA_LENGTH + INDEX_LENGTH) AS TOTAL_SIZE \
FROM( \
    SELECT t.TABLE_SCHEMA, t.TABLE_NAME, t.ENGINE, t.TABLE_ROWS, t.DATA_FREE, t.DATA_LENGTH, t.INDEX_LENGTH, (t.DATA_LENGTH + t.INDEX_LENGTH) AS TOTAL_SIZE \
	FROM information_schema.schemata s INNER JOIN information_schema.tables t ON s.SCHEMA_NAME = t.TABLE_SCHEMA \
	WHERE s.SCHEMA_NAME NOT REGEXP '(information_schema|performance_schema|mysql)' \
	ORDER BY (t.DATA_LENGTH + t.INDEX_LENGTH) DESC LIMIT %s \
) t1, (SELECT @rn:=0) t2;"

#define MYSQL_TOP10_TABLE_BY_ROWS_DBS "\
SELECT /*DBS_008*/ CAST(@rn:=@rn+1 AS DECIMAL(0)) AS TOP, TABLE_SCHEMA AS DB_NAME, TABLE_NAME, ENGINE AS TABLE_ENGINE, IFNULL(TABLE_ROWS,0) AS TABLE_ROWS, DATA_FREE AS DATA_FREE_SIZE, DATA_LENGTH AS DATA_SIZE, INDEX_LENGTH AS INDEX_SIZE, (DATA_LENGTH + INDEX_LENGTH) AS TOTAL_SIZE \
FROM( \
    SELECT t.TABLE_SCHEMA, t.TABLE_NAME, t.ENGINE, t.TABLE_ROWS, t.DATA_FREE, t.DATA_LENGTH, t.INDEX_LENGTH, (t.DATA_LENGTH + t.INDEX_LENGTH) AS TOTAL_SIZE \
	FROM information_schema.schemata s INNER JOIN information_schema.tables t ON s.SCHEMA_NAME = t.TABLE_SCHEMA \
	WHERE s.SCHEMA_NAME NOT REGEXP '(information_schema|performance_schema|mysql)' \
	ORDER BY t.TABLE_ROWS DESC LIMIT %s \
) t1, (SELECT @rn:=0) t2;"

#define MYSQL_SLAVE_STATUS_DBS "SHOW SLAVE STATUS;"

ZBX_METRIC	parameters_dbmon_mysql[] =
/*	KEY								FLAG				FUNCTION			TEST PARAMETERS */
{
	{"mysql.ping",					CF_HAVEPARAMS,		MYSQL_PING,			NULL},
	{"mysql.version",				CF_HAVEPARAMS,		MYSQL_VERSION,		NULL},
	{"mysql.version.full",			CF_HAVEPARAMS,		MYSQL_VERSION,		NULL},
	{"mysql.server.info",			CF_HAVEPARAMS,		MYSQL_GET_RESULT,	NULL},
	{"mysql.global.status",			CF_HAVEPARAMS,		MYSQL_GET_RESULT,	NULL},
	{"mysql.global.variables",		CF_HAVEPARAMS,		MYSQL_GET_RESULT,	NULL},
	{"mysql.db.discovery",			CF_HAVEPARAMS,		MYSQL_DISCOVERY,	NULL},
	{"mysql.db.info",				CF_HAVEPARAMS,		MYSQL_GET_RESULT,	NULL},
	{"mysql.errorlog.discovery",	CF_HAVEPARAMS,		MYSQL_DISCOVERY,	NULL},
	{"mysql.top10_table_by_size",	CF_HAVEPARAMS,		MYSQL_GET_RESULT,	NULL},
	{"mysql.top10_table_by_rows",	CF_HAVEPARAMS,		MYSQL_GET_RESULT,	NULL},
	{"mysql.slave.discovery",		CF_HAVEPARAMS,		MYSQL_DISCOVERY,	NULL},
	{"mysql.slave.status",			CF_HAVEPARAMS,		MYSQL_GET_RESULT,	NULL},
	{"mysql.query",					CF_HAVEPARAMS,		MYSQL_QUERY,		NULL},
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

static int	mysql_version(AGENT_REQUEST *request, AGENT_RESULT *result, unsigned int ver_mode)
{
	int							ret = SYSINFO_RET_FAIL;
	char						*mysql_host, *mysql_str_port;
	unsigned short				mysql_port = 0;
	struct zbx_db_connection	*mysql_conn;
	struct zbx_db_result		mysql_result;
	char						mysql_ver[10];
	unsigned long				version;

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
		if (0 == ver_mode)
		{
			version = zbx_db_version(mysql_conn);

			if (0 != version)
			{
				zbx_snprintf(mysql_ver, sizeof(mysql_ver), "%lu.%lu.%lu", version / 10000, (version % 10000) / 100, (version % 10000) % 100);
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
		}
		else
		{
			if (ZBX_DB_OK == zbx_db_query_select(mysql_conn, &mysql_result, MYSQL_VERSION_DBS))
			{
				ret = make_result(request, result, mysql_result, ZBX_DB_RES_TYPE_NOJSON);
				zbx_db_clean_result(&mysql_result);
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
		zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error connecting to MySQL database", __func__, request->key);
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Error connecting to database"));
		ret = SYSINFO_RET_FAIL;
	}

	zbx_db_close_db(mysql_conn);
	zbx_db_clean_connection(mysql_conn);

	return ret;
}

int	MYSQL_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int ret = SYSINFO_RET_FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);

	if (0 == strcmp(request->key, "mysql.version"))
	{
		ret = mysql_version(request, result, 0);
	}
	else if (0 == strcmp(request->key, "mysql.version.full"))
	{
		ret = mysql_version(request, result, 1);
	}
	else
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Unknown request key"));
		ret = SYSINFO_RET_FAIL;
	}

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s): %s", __func__, request->key, zbx_sysinfo_ret_string(ret));

	return ret;
}

#if !defined(_WINDOWS) && !defined(__MINGW32__)
static int	mysql_get_discovery(AGENT_REQUEST *request, AGENT_RESULT *result)
#else
static int	mysql_get_discovery(AGENT_REQUEST *request, AGENT_RESULT *result, HANDLE timeout_event)
#endif
{
	int							ret = SYSINFO_RET_FAIL;
	char						*mysql_host, *mysql_str_port, *c = NULL;
	unsigned short				mysql_port = 0;
	struct zbx_db_connection	*mysql_conn;
	struct zbx_db_result		mysql_result;
	const char					*query = MYSQL_DB_DISCOVERY_DBS, *mysql_schema;
	unsigned long				mysql_version;

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

#if defined(_WINDOWS) && defined(__MINGW32__)
	/* 'timeout_event' argument is here to make the mysql_get_discovery() prototype as required by */
	/* zbx_execute_threaded_metric() on MS Windows */
	ZBX_UNUSED(timeout_event);
#endif

	mysql_conn = zbx_db_connect_mysql(mysql_host, CONFIG_MYSQL_USER, CONFIG_MYSQL_PASSWORD, MYSQL_DEFAULT_DBNAME, mysql_port, NULL);

	if (NULL != mysql_conn)
	{
		mysql_version = zbx_db_version(mysql_conn);

		if (mysql_version > 50700 && mysql_version < 100000)
			mysql_schema = "performance_schema";
		else
			mysql_schema = "information_schema";

		if (0 == strcmp(request->key, "mysql.db.discovery"))
		{
			ret = zbx_db_query_select(mysql_conn, &mysql_result, MYSQL_DB_DISCOVERY_DBS);
		}
		else if (0 == strcmp(request->key, "mysql.errorlog.discovery"))
		{
			ret = zbx_db_query_select(mysql_conn, &mysql_result, MYSQL_ERRORLOG_DISCOVERY_DBS, mysql_schema);
		}
		else if (0 == strcmp(request->key, "mysql.slave.discovery"))
		{
			ret = zbx_db_query_select(mysql_conn, &mysql_result, MYSQL_SLAVE_STATUS_DBS);
		}
		else
		{
			SET_MSG_RESULT(result, zbx_strdup(NULL, "Unknown discovery request key."));
			ret = SYSINFO_RET_FAIL;
			goto out;
		}

		if (ZBX_DB_OK == ret)
		{
			ret = make_result(request, result, mysql_result, ZBX_DB_RES_TYPE_DISCOVERY);
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
out:
	zbx_db_close_db(mysql_conn);
	zbx_db_clean_connection(mysql_conn);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s): %s", __func__, request->key, zbx_sysinfo_ret_string(ret));

	return ret;
}

int	MYSQL_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	return zbx_execute_threaded_metric(mysql_get_discovery, request, result);
	//return mysql_get_discovery(request, result, NULL);
}

static int	mysql_make_result(AGENT_REQUEST *request, AGENT_RESULT *result, char *query, zbx_db_result_type result_type)
{
	int							ret = SYSINFO_RET_FAIL, db_ret = ZBX_DB_ERROR;
	char						*mysql_host, *mysql_str_port, *mysql_top_table_num_str;
	unsigned short				mysql_port = 0, mysql_top_table_num = 0;
	struct zbx_db_connection	*mysql_conn;
	struct zbx_db_result		mysql_result;
	unsigned long				mysql_version;
	const char					*mysql_schema;

	if (NULL == CONFIG_MYSQL_USER)
		CONFIG_MYSQL_USER = zbx_strdup(CONFIG_MYSQL_USER, MYSQL_DEFAULT_USER);
	if (NULL == CONFIG_MYSQL_PASSWORD)
		CONFIG_MYSQL_PASSWORD = zbx_strdup(CONFIG_MYSQL_PASSWORD, MYSQL_DEFAULT_PASSWORD);

	mysql_host = get_rparam(request, 0);
	mysql_str_port = get_rparam(request, 1);

	if (0 == strcmp(request->key, "mysql.top10_table_by_size") || 0 == strcmp(request->key, "mysql.top10_table_by_rows"))
	{
		mysql_top_table_num_str = get_rparam(request, 2);
		if (NULL == mysql_top_table_num_str || '\0' == *mysql_top_table_num_str)
			mysql_top_table_num_str = "10";
		if (SUCCEED != is_ushort(mysql_top_table_num_str, &mysql_top_table_num))
			mysql_top_table_num_str = "10";
	}

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
		mysql_version = zbx_db_version(mysql_conn);

		if (mysql_version > 50700 && mysql_version < 100000)
			mysql_schema = "performance_schema";
		else
			mysql_schema = "information_schema";

		if (0 == strcmp(request->key, "mysql.server.info"))
		{
			db_ret = zbx_db_query_select(mysql_conn, &mysql_result, query, mysql_schema);
		}
		else if (0 == strcmp(request->key, "mysql.top10_table_by_size") || 0 == strcmp(request->key, "mysql.top10_table_by_rows"))
		{
			db_ret = zbx_db_query_select(mysql_conn, &mysql_result, query, mysql_top_table_num_str);
		}
		else
		{
			db_ret = zbx_db_query_select(mysql_conn, &mysql_result, query);
		}

		if (ZBX_DB_OK == db_ret)
		{
			ret = make_result(request, result, mysql_result, result_type);
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
	/* 'timeout_event' argument is here to make the mysql_get_result() prototype as required by */
	/* zbx_execute_threaded_metric() on MS Windows */
	ZBX_UNUSED(timeout_event);
#endif

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);

	if (0 == strcmp(request->key, "mysql.server.info"))
	{
		ret = mysql_make_result(request, result, MYSQL_SERVER_INFO_DBS, ZBX_DB_RES_TYPE_ONEROW);
	}
	else if (0 == strcmp(request->key, "mysql.global.status"))
	{
		ret = mysql_make_result(request, result, MYSQL_GLOBAL_STATUS_DBS, ZBX_DB_RES_TYPE_TWOCOLL);
	}
	else if (0 == strcmp(request->key, "mysql.global.variables"))
	{
		ret = mysql_make_result(request, result, MYSQL_GLOBAL_VARIABLES_DBS, ZBX_DB_RES_TYPE_TWOCOLL);
	}
	else if (0 == strcmp(request->key, "mysql.db.info"))
	{
		ret = mysql_make_result(request, result, MYSQL_DB_INFO_DBS, ZBX_DB_RES_TYPE_MULTIROW);
	}
	else if (0 == strcmp(request->key, "mysql.top10_table_by_size"))
	{
		ret = mysql_make_result(request, result, MYSQL_TOP10_TABLE_BY_SIZE_DBS, ZBX_DB_RES_TYPE_MULTIROW);
	}
	else if (0 == strcmp(request->key, "mysql.top10_table_by_rows"))
	{
		ret = mysql_make_result(request, result, MYSQL_TOP10_TABLE_BY_ROWS_DBS, ZBX_DB_RES_TYPE_MULTIROW);
	}
	else if (0 == strcmp(request->key, "mysql.slave.status"))
	{
		ret = mysql_make_result(request, result, MYSQL_SLAVE_STATUS_DBS, ZBX_DB_RES_TYPE_MULTIROW);
	}
	else
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Unknown request key"));
		ret = SYSINFO_RET_FAIL;
	}

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s): %s", __func__, request->key, zbx_sysinfo_ret_string(ret));

	return ret;
}

int	MYSQL_GET_RESULT(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	return zbx_execute_threaded_metric(mysql_get_result, request, result);
}

/*
 * Custom key mysql.query[]
 *
 * Returns the value of the specified SQL query.
 *
 * Parameters:
 *   0:  mysql host address
 *   1:  mysql port
 *   3:  query type (NOJSON, ONEROW, TWOCOLL, MULTIROW, DISCOVERY)
 *   3:  scalar SQL query to execute
 *   n:  query parameters
 *
 * Returns: string
 *
 */
int	MYSQL_QUERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int					ret = SYSINFO_RET_FAIL;
	const char			*query_key = NULL, *query = NULL;
	char				*query_result_type_str;
	zbx_db_result_type	query_result_type = ZBX_DB_RES_TYPE_NOJSON;
	int					i = 0;
	DBMONparams			params = NULL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);

	if (init_config_done != 0)
		init_config_done = init_dbmon_config();

	// Get the user SQL query result type parameter
	query_result_type_str = get_rparam(request, 2);
	if (strisnull(query_result_type_str))
	{
		dbmon_log_result(result, LOG_LEVEL_ERR, "No query result type specified");
		goto out;
	}
	else
	{
		if (0 == strcmp(query_result_type_str, "NOJSON"))
		{
			query_result_type = ZBX_DB_RES_TYPE_NOJSON;
		}
		else if (0 == strcmp(query_result_type_str, "ONEROW"))
		{
			query_result_type = ZBX_DB_RES_TYPE_ONEROW;
		}
		else if (0 == strcmp(query_result_type_str, "TWOCOLL"))
		{
			query_result_type = ZBX_DB_RES_TYPE_TWOCOLL;
		}
		else if (0 == strcmp(query_result_type_str, "MULTIROW"))
		{
			query_result_type = ZBX_DB_RES_TYPE_MULTIROW;
		}
		else if (0 == strcmp(query_result_type_str, "DISCOVERY"))
		{
			query_result_type = ZBX_DB_RES_TYPE_DISCOVERY;
		}
		else
		{
			dbmon_log_result(result, LOG_LEVEL_ERR, "Unsupported query type: %s", query_result_type_str);
			goto out;
		}
	}

	// Get the user SQL query parameter
	query_key = get_rparam(request, 3);
	if (strisnull(query_key)) {
		dbmon_log_result(result, LOG_LEVEL_ERR, "No query or query-key specified");
		goto out;
	}

	// Check if query comes from configs
	query = get_query_by_name(query_key);
	if (NULL == query) {
		dbmon_log_result(result, LOG_LEVEL_DEBUG, "No query found for '%s'", query_key);
		goto out;
		//query = query_key;
	}

	// parse user params
	dbmon_log_result(result, LOG_LEVEL_DEBUG, "Appending %i params to query", request->nparam - 3);
	for (i = 4; i < request->nparam; i++) {
		params = dbmon_param_append(params, get_rparam(request, i));
	}

	dbmon_log_result(result, LOG_LEVEL_TRACE, "Execute query: %s", query);

	ret = mysql_make_result(request, result, (char *)query, query_result_type);

	dbmon_param_free(params);
out:
	uninit_dbmon_config();
	init_config_done = 1;

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s): %s", __func__, request->key, zbx_sysinfo_ret_string(ret));
	return ret;
}

#endif
#endif