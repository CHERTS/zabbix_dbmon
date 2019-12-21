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

#define MYSQL_DISCOVER_DBS "SELECT SCHEMA_NAME AS DBNAME, DEFAULT_CHARACTER_SET_NAME AS DB_CHARACTER_SET, DEFAULT_COLLATION_NAME AS DB_COLLATION_NAME FROM information_schema.SCHEMATA;"
#define MYSQL_VERSION_DBS "SELECT VERSION() AS VERSION;"

ZBX_METRIC	parameters_dbmon_mysql[] =
/*	KEY			FLAG		FUNCTION		TEST PARAMETERS */
{
	{"mysql.ping",		CF_HAVEPARAMS,		MYSQL_PING,	NULL},
	{"mysql.version",	CF_HAVEPARAMS,		MYSQL_VERSION,	NULL},
	{"mysql.db.discovery",	CF_HAVEPARAMS,		MYSQL_DB_DISCOVERY,	NULL},
	{NULL}
};

static int	MYSQL_PING(AGENT_REQUEST *request, AGENT_RESULT *result)
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

static int	MYSQL_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int							ret = SYSINFO_RET_FAIL;
	char						*mysql_host, *mysql_str_port;
	unsigned short				mysql_port = 0;
	struct zbx_db_connection	*mysql_conn;
	struct zbx_db_result		mysql_result;

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
		if (ZBX_DB_OK == zbx_db_query_select(mysql_conn, &mysql_result, MYSQL_VERSION_DBS))
		{
			ret = make_result(request, result, mysql_result);
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

static int	MYSQL_DB_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int ret = SYSINFO_RET_FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __func__);

	ret = mysql_get_discovery(request, result, MYSQL_DISCOVER_DBS);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __func__);

	return ret;
}

#endif
#endif