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
	{"pg.ping",		CF_HAVEPARAMS,		PG_PING,	NULL},
	{"pg.version",	CF_HAVEPARAMS,		PG_VERSION,	NULL},
	{"pg.db.discovery",	CF_HAVEPARAMS,		PG_DB_DISCOVERY,	NULL},
	{NULL}
};

static int	PG_PING(AGENT_REQUEST *request, AGENT_RESULT *result)
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

static int	PG_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int							ret = SYSINFO_RET_FAIL;
	char						*pg_conn_string;
	struct zbx_db_connection	*pgsql_conn;
	struct zbx_db_result		pgsql_result;

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
		if (ZBX_DB_OK == zbx_db_query_select(pgsql_conn, &pgsql_result, PG_VERSION_DBS))
		{
			ret = make_result(request, result, pgsql_result);
			zbx_db_clean_result(&pgsql_result);
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

	zbx_db_close_db(pgsql_conn);
	zbx_db_clean_connection(pgsql_conn);

	return ret;
}

int	pg_get_discovery(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query)
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
		}
	}
	else
	{
		zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error connecting to database", __func__, request->key);
	}

	zbx_db_close_db(pgsql_conn);
	zbx_db_clean_connection(pgsql_conn);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __func__, request->key);

	return ret;
}

static int	PG_DB_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int ret = SYSINFO_RET_FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __func__);

	ret = pg_get_discovery(request, result, PG_DISCOVER_DBS);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __func__);

	return ret;
}

#endif
#endif