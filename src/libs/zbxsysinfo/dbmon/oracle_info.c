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

#define ORACLE_VERSION_DBS "\
SELECT version AS VERSION \
FROM v$instance \
WHERE instance_name = '%s'"

#define ORACLE_INSTANCE_INFO_DBS "\
SELECT to_char(round(((sysdate - startup_time) * 60 * 60 * 24), 0)) AS UPTIME, \
	decode(status, 'STARTED', 1, 'MOUNTED', 2, 'OPEN', 3, 'OPEN MIGRATE', 4, 0) AS STATUS, \
	decode(parallel, 'YES', 1, 'NO', 2, 0) PARALLEL, \
	decode(archiver, 'STOPPED', 1, 'STARTED', 2, 'FAILED', 3, 0) ARCHIVER \
FROM v$instance \
WHERE instance_name = '%s'"

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
	decode(d.log_mode,'NOARCHIVELOG',1,'ARCHIVELOG',2,'MANUAL',3,0) log_mode, \
	to_char(cast(d.resetlogs_time as timestamp WITH time zone),'YYYY-MM-DD_HH24_MI_SS_TZH_TZM') RESETLOGS_TIME, \
	to_char(cast(d.controlfile_created as timestamp WITH time zone),'YYYY-MM-DD_HH24_MI_SS_TZH_TZM') CONTROLFILE_CREATED, \
	decode(d.open_mode, 'MOUNTED', 1, 'READ WRITE', 2, 'READ ONLY', 3, 'READ ONLY WITH APPLY', 4, 0) OPEN_MODE, \
	decode(d.protection_mode, 'MAXIMUM PROTECTION', 1, 'MAXIMUM AVAILABILITY', 2, 'RESYNCHRONIZATION', 3, 'MAXIMUM PERFORMANCE', 4, 'UNPROTECTED', 5, 0) PROTECTION_MODE, \
	SWITCHOVER# AS SWITCHOVER_NUMBER, \
	decode(d.dataguard_broker, 'ENABLED', 1, 'DISABLED', 2, 0) DATAGUARD_BROKER, \
	decode(d.guard_status, 'ALL', 1, 'STANDBY', 2, 'NONE', 3, 0) GUARD_STATUS, \
	decode(d.force_logging, 'YES', 1, 'NO', 2, 0) FORCE_LOGGING, \
	decode(d.flashback_on, 'YES', 1, 'NO', 2, 'RESTORE POINT ONLY', 3, 0) FLASHBACK_ON, \
	decode(d.database_role, 'SNAPSHOT STANDBY', 1, 'LOGICAL STANDBY', 2, 'PHYSICAL STANDBY', 3, 'PRIMARY', 4, 0) DATABASE_ROLE, \
	d.dbid AS DBID, \
	d.name AS DBNAME, \
	d.platform_name AS PLATFORM_NAME \
FROM gv$instance i, gv$database d \
WHERE i.inst_id = d.inst_id \
	AND d.name = '%s'"

#define ORACLE_V12_DB_INFO_DBS "\
SELECT i.instance_name AS INSTANCE, \
    i.host_name AS HOSTNAME, \
	decode(d.log_mode,'NOARCHIVELOG',1,'ARCHIVELOG',2,'MANUAL',3,0) log_mode, \
	to_char(cast(d.resetlogs_time as timestamp WITH time zone),'YYYY-MM-DD_HH24_MI_SS_TZH_TZM') RESETLOGS_TIME, \
	to_char(cast(d.controlfile_created as timestamp WITH time zone),'YYYY-MM-DD_HH24_MI_SS_TZH_TZM') CONTROLFILE_CREATED, \
	decode(d.open_mode, 'MOUNTED', 1, 'READ WRITE', 2, 'READ ONLY', 3, 'READ ONLY WITH APPLY', 4, 0) OPEN_MODE, \
	decode(d.protection_mode, 'MAXIMUM PROTECTION', 1, 'MAXIMUM AVAILABILITY', 2, 'RESYNCHRONIZATION', 3, 'MAXIMUM PERFORMANCE', 4, 'UNPROTECTED', 5, 0) PROTECTION_MODE, \
	SWITCHOVER# AS SWITCHOVER_NUMBER, \
	decode(d.dataguard_broker, 'ENABLED', 1, 'DISABLED', 2, 0) DATAGUARD_BROKER, \
	decode(d.guard_status, 'ALL', 1, 'STANDBY', 2, 'NONE', 3, 0) GUARD_STATUS, \
	decode(d.force_logging, 'YES', 1, 'NO', 2, 0) FORCE_LOGGING, \
	decode(d.flashback_on, 'YES', 1, 'NO', 2, 'RESTORE POINT ONLY', 3, 0) FLASHBACK_ON, \
	decode(d.database_role, 'SNAPSHOT STANDBY', 1, 'LOGICAL STANDBY', 2, 'PHYSICAL STANDBY', 3, 'PRIMARY', 4, 0) DATABASE_ROLE, \
	decode(d.cdb,'YES',1,'NO',2,0) AS CDB, \
	d.dbid AS DBID, \
	d.name AS DBNAME, \
	d.platform_name AS PLATFORM_NAME \
FROM gv$instance i, gv$database d \
WHERE i.inst_id = d.inst_id \
	AND d.name = '%s'"

#define ORACLE_DB_INCARNATION_INFO_DBS "\
SELECT nvl(incarnation#, 0) AS INCARNATION \
FROM v$database_incarnation \
WHERE status = 'CURRENT'"

#define ORACLE_DB_SIZE_INFO_DBS "\
SELECT nvl(sum(bytes), 0) AS DBSIZE \
FROM v$datafile"

#define ORACLE_PDB_SIZE_INFO_DBS "\
SELECT nvl(sum(d.bytes), 0) AS PDB_SIZE \
FROM v$datafile d, v$pdbs p \
WHERE d.con_id = p.con_id \
AND p.name = '%s'"

#define ORACLE_INSTANCE_PARAMETER_INFO_DBS "\
SELECT p.name AS PARAMETER, p.value AS PVALUE \
FROM gv$instance i, gv$parameter p \
WHERE i.instance_number = p.inst_id \
AND i.instance_name = '%s' \
AND p.name in('db_files', 'processes', 'sessions')"

#define ORACLE_INSTANCE_RESOURCE_INFO_DBS "\
SELECT r.resource_name AS RNAME, \
	nvl(r.current_utilization, 0) AS RVALUE \
FROM gv$instance i, gv$resource_limit r \
WHERE i.instance_number = r.inst_id \
	AND i.instance_name = '%s' \
	AND r.resource_name in('processes', 'sessions')"

#define ORACLE_INSTANCE_DB_FILES_CURRENT_DBS "\
SELECT nvl(count(*), 0) AS DB_FILES_CURRENT \
FROM gv$instance i, gv$datafile d \
WHERE i.instance_number = d.inst_id \
	AND i.instance_name = '%s'"

#define ORACLE_INSTANCE_RESUMABLE_COUNT_DBS "\
SELECT nvl(count(rr.instance_id),0) AS RESUMABLE_COUNT \
FROM(SELECT r.instance_id FROM dba_resumable r WHERE r.STATUS = 'SUSPENDED') rr \
RIGHT JOIN gv$instance i ON i.inst_id = rr.instance_id \
GROUP BY i.INSTANCE_NAME"

#define ORACLE_INSTANCE_COUNT_BAD_PROCESSES_DBS "\
SELECT nvl(count(pp.addr), 0) AS COUNT_BAD_PROCESSES FROM \
(SELECT p.addr, inst_id \
	FROM gv$process p \
	WHERE p.program <> 'PSEUDO' \
	AND p.program NOT LIKE '%(D00%' AND p.program NOT LIKE '%(S0%' \
	AND p.program NOT LIKE '%(S1%' AND p.program NOT LIKE '%(P0%' \
	AND p.program NOT LIKE '%(P1%' AND p.program NOT LIKE '%(P2%' \
	AND p.program NOT LIKE '%(P3%' AND p.program NOT LIKE '%(P4%' \
	AND p.program NOT LIKE '%(P5%' AND p.program NOT LIKE '%(P6%' \
	AND p.program NOT LIKE '%(P7%' AND p.program NOT LIKE '%(P8%' \
	AND p.program NOT LIKE '%(P9%' \
	AND p.program NOT LIKE '%(J0%' \
	MINUS SELECT paddr, inst_id FROM gv$session \
	MINUS SELECT paddr, inst_id FROM gv$bgprocess) pp \
RIGHT JOIN gv$instance i ON i.INST_ID = pp.INST_ID  \
GROUP BY i.INSTANCE_NAME"

#define ORACLE_INSTANCE_FRA_INFO_DBS "\
SELECT name AS FRA_LOCATION_NAME, number_of_files AS FRA_FILE_NUM, space_limit AS FRA_SPACE_LIMIT, \
	space_used AS FRA_SPACE_USED, space_reclaimable AS FRA_SPACE_RECLAIMABLE, \
	decode(space_limit, 0, 100, 100-(space_used-space_reclaimable)/space_limit*100) AS FRA_FREE_PCT \
FROM v$recovery_file_dest"

ZBX_METRIC	parameters_dbmon_oracle[] =
/*	KEY										FLAG				FUNCTION						TEST PARAMETERS */
{
	{"oracle.instance.ping",				CF_HAVEPARAMS,		ORACLE_INSTANCE_PING,			NULL},
	{"oracle.instance.version",				CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.info",				CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.parameter",			CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.resource",			CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.dbfiles",				CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.resumable",			CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.bad_processes",		CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.fra",					CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.db.discovery",					CF_HAVEPARAMS,		ORACLE_DB_DISCOVERY,			NULL},
	{"oracle.db.info",						CF_HAVEPARAMS,		ORACLE_DB_INFO,					NULL},
	{"oracle.db.incarnation",				CF_HAVEPARAMS,		ORACLE_DB_INCARNATION,			NULL},
	{"oracle.db.size",						CF_HAVEPARAMS,		ORACLE_DB_SIZE,					NULL},
	{NULL}
};

static int	ORACLE_INSTANCE_PING(AGENT_REQUEST *request, AGENT_RESULT *result)
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

int	oracle_make_result(AGENT_REQUEST *request, AGENT_RESULT *result, char *query, unsigned int result_type)
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
		if (ZBX_DB_OK == zbx_db_query_select(oracle_conn, &ora_result, query, oracle_instance))
		{
			switch (result_type)
			{
				case ZBX_DB_RES_TYPE_ONEROW:
					ret = make_onerow_json_result(request, result, ora_result);
					break;
				case ZBX_DB_RES_TYPE_MULTIROW:
					ret = make_multirow_json_result(request, result, ora_result);
					break;
				default:
					ret = make_result(request, result, ora_result);
					break;
			}
			zbx_db_clean_result(&ora_result);
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

	zbx_db_close_db(oracle_conn);
	zbx_db_clean_connection(oracle_conn);

	return ret;
}

static int	ORACLE_GET_INSTANCE_RESULT(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	int ret = SYSINFO_RET_FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);

	if (0 == strcmp((const char*)"oracle.instance.version", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_VERSION_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp((const char*)"oracle.instance.info", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_INFO_DBS, ZBX_DB_RES_TYPE_ONEROW);
	}
	else if (0 == strcmp((const char*)"oracle.instance.parameter", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_PARAMETER_INFO_DBS, ZBX_DB_RES_TYPE_MULTIROW);
	}
	else if (0 == strcmp((const char*)"oracle.instance.resource", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_RESOURCE_INFO_DBS, ZBX_DB_RES_TYPE_MULTIROW);
	}
	else if (0 == strcmp((const char*)"oracle.instance.dbfiles", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_DB_FILES_CURRENT_DBS, ZBX_DB_RES_TYPE_ONEROW);
	}
	else if (0 == strcmp((const char*)"oracle.instance.resumable", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_RESUMABLE_COUNT_DBS, ZBX_DB_RES_TYPE_ONEROW);
	}
	else if (0 == strcmp((const char*)"oracle.instance.bad_processes", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_COUNT_BAD_PROCESSES_DBS, ZBX_DB_RES_TYPE_ONEROW);
	}
	else if (0 == strcmp((const char*)"oracle.instance.fra", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_FRA_INFO_DBS, ZBX_DB_RES_TYPE_ONEROW);
	}
	else
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Unknown request key"));
		ret = SYSINFO_RET_FAIL;
	}

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __func__, request->key);

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
		if (ZBX_DB_OK == zbx_db_query_select(oracle_conn, &ora_result, ORACLE_VERSION_DBS, oracle_instance))
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
	char						*oracle_host, *oracle_str_port, *oracle_str_mode, *oracle_instance, *ora_version, *oracle_dbname;
	unsigned short				oracle_port = 1521;
	unsigned int				oracle_mode;
	struct zbx_db_connection	*oracle_conn;
	struct zbx_db_result		ora_result;
	const char					*query = ORACLE_V11_DB_INFO_DBS;

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
		if (ZBX_DB_OK == zbx_db_query_select(oracle_conn, &ora_result, ORACLE_VERSION_DBS, oracle_instance))
		{
			ora_version = get_str_one_result(request, result, 0, 0, ora_result);

			if (NULL != ora_version)
			{
				zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Oracle version: %s", __func__, request->key, ora_version);

				if (-1 == zbx_db_compare_version(ora_version, "12.0.0.0.0"))
				{
					query = ORACLE_V11_DB_INFO_DBS;
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Oracle version < 12, use query '%s'", __func__, request->key, query);
				}
				else
				{
					query = ORACLE_V12_DB_INFO_DBS;
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Oracle version >= 12, use query '%s'", __func__, request->key, query);
				}
			}

			zbx_db_clean_result(&ora_result);
		}
		else
		{
			zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error executing query", __func__, request->key);
		}

		if (ZBX_DB_OK == zbx_db_query_select(oracle_conn, &ora_result, query, oracle_dbname))
		{
			ret = make_onerow_json_result(request, result, ora_result);
			zbx_db_clean_result(&ora_result);
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

	zbx_db_close_db(oracle_conn);
	zbx_db_clean_connection(oracle_conn);

	return ret;
}

static int	ORACLE_DB_INCARNATION(AGENT_REQUEST *request, AGENT_RESULT *result)
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
		if (ZBX_DB_OK == zbx_db_query_select(oracle_conn, &ora_result, ORACLE_DB_INCARNATION_INFO_DBS))
		{
			ret = make_onerow_json_result(request, result, ora_result);
			zbx_db_clean_result(&ora_result);
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

	zbx_db_close_db(oracle_conn);
	zbx_db_clean_connection(oracle_conn);

	return ret;
}

static int	ORACLE_DB_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result)
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
		if (ZBX_DB_OK == zbx_db_query_select(oracle_conn, &ora_result, ORACLE_DB_SIZE_INFO_DBS))
		{
			ret = make_onerow_json_result(request, result, ora_result);
			zbx_db_clean_result(&ora_result);
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

	zbx_db_close_db(oracle_conn);
	zbx_db_clean_connection(oracle_conn);

	return ret;
}
#endif
#endif