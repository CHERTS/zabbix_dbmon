#include "common.h"
#include "sysinfo.h"
#include "log.h"
#include "module.h"
#include "zbxdbmon.h"
#include "oracle_info.h"
#include "dbmon_common.h"

#if defined(HAVE_DBMON)
#if defined(HAVE_ORACLE)

extern char	*CONFIG_ORACLE_USER;
extern char	*CONFIG_ORACLE_PASSWORD;
extern char	*CONFIG_ORACLE_INSTANCE;

#define ORACLE_DEFAULT_USER	"sys"
#define ORACLE_DEFAULT_PASSWORD	"sys"
#define ORACLE_DEFAULT_INSTANCE	"orcl"

#define ORACLE_VERSION_DBS "SELECT version AS VERSION FROM v$instance"

#define ORACLE_V11_DISCOVER_DB_DBS "\
SELECT i.instance_name AS INSTANCE, \
    i.host_name AS HOSTNAME, \
    d.name AS DBNAME \
FROM gv$instance i, gv$database d \
WHERE i.inst_id = d.inst_id"

#define ORACLE_V12_DISCOVER_DB_DBS "\
SELECT i.instance_name AS INSTANCE, \
    i.host_name AS HOSTNAME, \
    d.name AS DBNAME \
FROM gv$instance i, gv$database d \
WHERE i.inst_id = d.inst_id AND \
	d.cdb = 'NO'"

#define ORACLE_V11_DB_INFO_DBS "\
SELECT i.instance_name AS INSTANCE, \
    i.host_name AS HOSTNAME, \
    d.name AS DBNAME \
FROM gv$instance i, gv$database d \
WHERE i.inst_id = d.inst_id \
	AND d.name = '%s'"

ZBX_METRIC	parameters_dbmon_oracle[] =
/*	KEY			FLAG		FUNCTION		TEST PARAMETERS */
{
	{"oracle.ping",			CF_HAVEPARAMS,		ORACLE_PING,			NULL},
	{"oracle.version",		CF_HAVEPARAMS,		ORACLE_VERSION,			NULL},
	{"oracle.db.discovery",	CF_HAVEPARAMS,		ORACLE_DB_DISCOVERY,	NULL},
	{"oracle.db.info",		CF_HAVEPARAMS,		ORACLE_DB_INFO,			NULL},
	{NULL}
};

static int	ORACLE_PING(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int							ret = SYSINFO_RET_FAIL, ping = 0;
	char						*oracle_host, *oracle_str_port, *oracle_str_mode, *oracle_instance;
	unsigned short				oracle_port = 0;
	unsigned int				oracle_mode = ZBX_DB_OCI_DEFAULT;
	struct zbx_db_connection	*oracle_conn;

	if (NULL == CONFIG_ORACLE_USER)
		CONFIG_ORACLE_USER = zbx_strdup(CONFIG_ORACLE_USER, ORACLE_DEFAULT_USER);
	if (NULL == CONFIG_ORACLE_PASSWORD)
		CONFIG_ORACLE_PASSWORD = zbx_strdup(CONFIG_ORACLE_PASSWORD, ORACLE_DEFAULT_PASSWORD);
	if (NULL == CONFIG_ORACLE_INSTANCE)
		CONFIG_ORACLE_INSTANCE = zbx_strdup(CONFIG_ORACLE_INSTANCE, ORACLE_DEFAULT_INSTANCE);

	oracle_host = get_rparam(request, 0);
	oracle_str_port = get_rparam(request, 1);
	oracle_instance = get_rparam(request, 2);
	oracle_str_mode = get_rparam(request, 3);

	if (NULL == oracle_instance || '\0' == *oracle_instance)
	{
		oracle_instance = CONFIG_ORACLE_INSTANCE;
	}

	if (NULL != oracle_str_port && '\0' != *oracle_str_port)
	{
		if (SUCCEED != is_ushort(oracle_str_port, &oracle_port))
		{
			SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid second parameter (port)."));
			return SYSINFO_RET_FAIL;
		}
	}

	if (NULL != oracle_str_mode && '\0' != *oracle_str_mode)
	{
		if (SUCCEED != is_ushort(oracle_str_mode, &oracle_mode))
		{
			SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid third parameter (mode)."));
			return SYSINFO_RET_FAIL;
		}
	}

	oracle_conn = zbx_db_connect_oracle(oracle_host, CONFIG_ORACLE_USER, CONFIG_ORACLE_PASSWORD, oracle_instance, oracle_port, zbx_db_get_oracle_mode(oracle_mode));

	if (oracle_conn != NULL)
	{
		SET_UI64_RESULT(result, 1);
	}
	else
	{
		SET_UI64_RESULT(result, 0);
	}

	zbx_db_close_db(oracle_conn);
	zbx_db_clean_connection(oracle_conn);

	return SYSINFO_RET_OK;
}

static int	ORACLE_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int							ret = SYSINFO_RET_FAIL, ping = 0;
	char						*oracle_host, *oracle_str_port, *oracle_str_mode, *oracle_instance;
	unsigned short				oracle_port = 0;
	unsigned int				oracle_mode = ZBX_DB_OCI_DEFAULT;
	struct zbx_db_connection	*oracle_conn;
	struct zbx_db_result		ora_result;

	if (NULL == CONFIG_ORACLE_USER)
		CONFIG_ORACLE_USER = zbx_strdup(CONFIG_ORACLE_USER, ORACLE_DEFAULT_USER);
	if (NULL == CONFIG_ORACLE_PASSWORD)
		CONFIG_ORACLE_PASSWORD = zbx_strdup(CONFIG_ORACLE_PASSWORD, ORACLE_DEFAULT_PASSWORD);
	if (NULL == CONFIG_ORACLE_INSTANCE)
		CONFIG_ORACLE_INSTANCE = zbx_strdup(CONFIG_ORACLE_INSTANCE, ORACLE_DEFAULT_INSTANCE);

	oracle_host = get_rparam(request, 0);
	oracle_str_port = get_rparam(request, 1);
	oracle_instance = get_rparam(request, 2);
	oracle_str_mode = get_rparam(request, 3);

	if (NULL == oracle_instance || '\0' == *oracle_instance)
	{
		oracle_instance = CONFIG_ORACLE_INSTANCE;
	}

	if (NULL != oracle_str_port && '\0' != *oracle_str_port)
	{
		if (SUCCEED != is_ushort(oracle_str_port, &oracle_port))
		{
			SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid second parameter (port)."));
			return SYSINFO_RET_FAIL;
		}
	}

	if (NULL != oracle_str_mode && '\0' != *oracle_str_mode)
	{
		if (SUCCEED != is_ushort(oracle_str_mode, &oracle_mode))
		{
			SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid third parameter (mode)."));
			return SYSINFO_RET_FAIL;
		}
	}

	oracle_conn = zbx_db_connect_oracle(oracle_host, CONFIG_ORACLE_USER, CONFIG_ORACLE_PASSWORD, oracle_instance, oracle_port, zbx_db_get_oracle_mode(oracle_mode));

	if (oracle_conn != NULL)
	{
		if (ZBX_DB_OK == zbx_db_query_select(oracle_conn, &ora_result, ORACLE_VERSION_DBS))
		{
			ret = make_result(request, result, ora_result);
			zbx_db_clean_result(&ora_result);
		}
		else
		{
			zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error executing query", __func__, request->key);
		}
	}
	else
	{
		zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error connecting to database", __func__, request->key);
	}

	zbx_db_close_db(oracle_conn);
	zbx_db_clean_connection(oracle_conn);

	return ret;
}

int	oracle_get_discovery(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int							ret = SYSINFO_RET_FAIL, ping = 0;
	char						*oracle_host, *oracle_str_port, *oracle_str_mode, *oracle_instance, *ora_version, *sql = NULL;
	unsigned short				oracle_port = 0;
	unsigned int				oracle_mode = ZBX_DB_OCI_DEFAULT;
	struct zbx_db_connection	*oracle_conn;
	struct zbx_db_result		ora_result;
	const char					*query = ORACLE_V11_DISCOVER_DB_DBS;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);

	if (NULL == CONFIG_ORACLE_USER)
		CONFIG_ORACLE_USER = zbx_strdup(CONFIG_ORACLE_USER, ORACLE_DEFAULT_USER);
	if (NULL == CONFIG_ORACLE_PASSWORD)
		CONFIG_ORACLE_PASSWORD = zbx_strdup(CONFIG_ORACLE_PASSWORD, ORACLE_DEFAULT_PASSWORD);
	if (NULL == CONFIG_ORACLE_INSTANCE)
		CONFIG_ORACLE_INSTANCE = zbx_strdup(CONFIG_ORACLE_INSTANCE, ORACLE_DEFAULT_INSTANCE);

	oracle_host = get_rparam(request, 0);
	oracle_str_port = get_rparam(request, 1);
	oracle_instance = get_rparam(request, 2);
	oracle_str_mode = get_rparam(request, 3);

	if (NULL == oracle_instance || '\0' == *oracle_instance)
	{
		oracle_instance = CONFIG_ORACLE_INSTANCE;
	}

	if (NULL != oracle_str_port && '\0' != *oracle_str_port)
	{
		if (SUCCEED != is_ushort(oracle_str_port, &oracle_port))
		{
			SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid second parameter (port)."));
			return SYSINFO_RET_FAIL;
		}
	}

	if (NULL != oracle_str_mode && '\0' != *oracle_str_mode)
	{
		if (SUCCEED != is_ushort(oracle_str_mode, &oracle_mode))
		{
			SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid third parameter (mode)."));
			return SYSINFO_RET_FAIL;
		}
	}

	oracle_conn = zbx_db_connect_oracle(oracle_host, CONFIG_ORACLE_USER, CONFIG_ORACLE_PASSWORD, oracle_instance, oracle_port, zbx_db_get_oracle_mode(oracle_mode));

	if (oracle_conn != NULL)
	{
		if (ZBX_DB_OK == zbx_db_query_select(oracle_conn, &ora_result, ORACLE_VERSION_DBS))
		{
			ora_version = get_str_one_result(request, result, 0, 0, ora_result);

			if (NULL != ora_version)
			{
				zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Oracle version: %s", __func__, request->key, ora_version);

				if (-1 == zbx_db_compare_version(ora_version, "12.0.0.0.0"))
				{
					query = ORACLE_V11_DISCOVER_DB_DBS;
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Oracle version < 12, use query '%s'", __func__, request->key, query);
				}
				else
				{
					query = ORACLE_V12_DISCOVER_DB_DBS;
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Oracle version >= 12, use query '%s'", __func__, request->key, query);
				}
			}

			zbx_db_clean_result(&ora_result);
		}
		else
		{
			zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error executing query", __func__, request->key);
		}

		zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Oracle instance '%s'", __func__, request->key, oracle_instance);

		if (zbx_db_query_select(oracle_conn, &ora_result, query) == ZBX_DB_OK)
		{
			ret = make_discovery_result(request, result, ora_result);
			zbx_db_clean_result(&ora_result);
		}
		else
		{
			zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error executing query", __func__, request->key);
		}
	}
	else
	{
		zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error connecting to database", __func__, request->key);
	}

	zbx_db_close_db(oracle_conn);
	zbx_db_clean_connection(oracle_conn);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __func__, request->key);

	return ret;
}

static int	ORACLE_DB_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int ret = SYSINFO_RET_FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __func__);

	ret = oracle_get_discovery(request, result);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __func__);

	return ret;
}

static int	ORACLE_DB_INFO(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int							ret = SYSINFO_RET_FAIL, ping = 0;
	char						*oracle_host, *oracle_str_port, *oracle_str_mode, *oracle_instance, *oracle_dbname;
	unsigned short				oracle_port = 1521;
	unsigned int				oracle_mode;
	struct zbx_db_connection	*oracle_conn;
	struct zbx_db_result		ora_result;

	if (NULL == CONFIG_ORACLE_USER)
		CONFIG_ORACLE_USER = zbx_strdup(CONFIG_ORACLE_USER, ORACLE_DEFAULT_USER);
	if (NULL == CONFIG_ORACLE_PASSWORD)
		CONFIG_ORACLE_PASSWORD = zbx_strdup(CONFIG_ORACLE_PASSWORD, ORACLE_DEFAULT_PASSWORD);
	if (NULL == CONFIG_ORACLE_INSTANCE)
		CONFIG_ORACLE_INSTANCE = zbx_strdup(CONFIG_ORACLE_INSTANCE, ORACLE_DEFAULT_INSTANCE);

	if (5 < request->nparam)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Too many parameters."));
		return SYSINFO_RET_FAIL;
	}

	oracle_host = get_rparam(request, 0);
	oracle_str_port = get_rparam(request, 1);
	oracle_instance = get_rparam(request, 2);
	oracle_str_mode = get_rparam(request, 3);
	oracle_dbname = get_rparam(request, 4);

	zabbix_log(LOG_LEVEL_TRACE, "Is %s(%s): request->nparam = %d", __func__, request->key, request->nparam);
	zabbix_log(LOG_LEVEL_TRACE, "Is %s(%s): nparam[0] = %s", __func__, request->key, oracle_host);
	zabbix_log(LOG_LEVEL_TRACE, "Is %s(%s): nparam[1] = %s", __func__, request->key, oracle_str_port);
	zabbix_log(LOG_LEVEL_TRACE, "Is %s(%s): nparam[2] = %s", __func__, request->key, oracle_instance);
	zabbix_log(LOG_LEVEL_TRACE, "Is %s(%s): nparam[3] = %s", __func__, request->key, oracle_str_mode);
	zabbix_log(LOG_LEVEL_TRACE, "Is %s(%s): nparam[4] = %s", __func__, request->key, oracle_dbname);

	if (NULL == oracle_instance || '\0' == *oracle_instance)
	{
		oracle_instance = CONFIG_ORACLE_INSTANCE;
	}

	if (NULL != oracle_str_port && '\0' != *oracle_str_port)
	{
		if (SUCCEED != is_ushort(oracle_str_port, &oracle_port))
		{
			SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid second parameter (port)."));
			return SYSINFO_RET_FAIL;
		}
	}

	if (NULL != oracle_str_mode && '\0' != *oracle_str_mode)
	{
		if (SUCCEED != is_ushort(oracle_str_mode, &oracle_mode))
		{
			SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid third parameter (mode)."));
			return SYSINFO_RET_FAIL;
		}
	}

	if (NULL == oracle_dbname || '\0' == *oracle_dbname)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Invalid fourth parameter (dbname)."));
		return SYSINFO_RET_FAIL;
	}

	oracle_conn = zbx_db_connect_oracle(oracle_host, CONFIG_ORACLE_USER, CONFIG_ORACLE_PASSWORD, oracle_instance, oracle_port, zbx_db_get_oracle_mode(oracle_mode));

	if (oracle_conn != NULL)
	{
		if (ZBX_DB_OK == zbx_db_query_select(oracle_conn, &ora_result, ORACLE_V11_DB_INFO_DBS, oracle_dbname))
		{
			ret = make_onerow_json_result(request, result, ora_result);
			zbx_db_clean_result(&ora_result);
		}
		else
		{
			zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error executing query", __func__, request->key);
		}
	}
	else
	{
		zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error connecting to database", __func__, request->key);
	}

	zbx_db_close_db(oracle_conn);
	zbx_db_clean_connection(oracle_conn);

	return ret;
}

#endif
#endif