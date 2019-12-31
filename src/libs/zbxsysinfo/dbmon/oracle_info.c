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

#define ORACLE_CHECK_DB_OPEN_MODE_DBS "\
SELECT decode(d.open_mode, 'MOUNTED', 1, 'READ WRITE', 2, 'READ ONLY', 3, 'READ ONLY WITH APPLY', 4, 0) AS OPEN_MODE \
FROM gv$instance i, gv$database d \
WHERE i.inst_id = d.inst_id \
	AND d.database_role = '%s' \
	AND i.status in('MOUNTED', 'OPEN') \
	AND i.instance_name = '%s' \
	AND d.name = '%s'"

#define ORACLE_CHECK_STANDBY_DB_OPEN_MODE_DBS "\
SELECT decode(d.open_mode, 'MOUNTED', 1, 'READ WRITE', 2, 'READ ONLY', 3, 'READ ONLY WITH APPLY', 4, 0) AS OPEN_MODE \
FROM gv$instance i, gv$database d \
WHERE i.inst_id = d.inst_id \
	AND d.database_role <> 'PRIMARY' \
	AND i.status in('MOUNTED', 'OPEN') \
	AND i.instance_name = '%s' \
	AND d.name = '%s'"

#define ORACLE_CHECK_INST_OPEN_MODE_DBS "\
SELECT decode(d.open_mode, 'MOUNTED', 1, 'READ WRITE', 2, 'READ ONLY', 3, 'READ ONLY WITH APPLY', 4, 0) AS OPEN_MODE \
FROM gv$instance i, gv$database d \
WHERE i.inst_id = d.inst_id \
	AND d.database_role = '%s' \
	AND i.status in('MOUNTED', 'OPEN') \
	AND i.instance_name = '%s'"

#define ORACLE_CHECK_STANDBY_INST_OPEN_MODE_DBS "\
SELECT decode(d.open_mode, 'MOUNTED', 1, 'READ WRITE', 2, 'READ ONLY', 3, 'READ ONLY WITH APPLY', 4, 0) AS OPEN_MODE \
FROM gv$instance i, gv$database d \
WHERE i.inst_id = d.inst_id \
	AND d.database_role <> 'PRIMARY' \
	AND i.status in('MOUNTED', 'OPEN') \
	AND i.instance_name = '%s'"

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

#define ORACLE_LASTPATCH_INFO_DBS "\
SELECT TO_CHAR(ACTION_TIME, 'DD-MON-YYYY HH24:MI:SS') AS LAST_PATCHSET_ACTION_TIME, \
	ACTION AS LAST_PATCHSET_ACTION, \
	VERSION AS LAST_PATCHSET_VERSION, \
	COMMENTS AS LAST_PATCHSET_COMMENTS \
FROM sys.registry$history  \
WHERE namespace = 'SERVER'  \
	AND ACTION IN('APPLY', 'UPGRADE')  \
	AND action_time = (SELECT max(action_time) \
			FROM sys.registry$history \
			WHERE namespace = 'SERVER' \
				AND ACTION IN('APPLY', 'UPGRADE') \
)"

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
	decode(space_limit, 0, 100, (space_used-space_reclaimable)/space_limit*100) AS FRA_USED_PCT, \
	decode(space_limit, 0, 100, 100-(space_used-space_reclaimable)/space_limit*100) AS FRA_FREE_PCT \
FROM v$recovery_file_dest"

#define ORACLE_INSTANCE_REDOLOG_SWITCH_RATE_INFO_DBS "\
WITH a AS(SELECT cnt, thread# , row_number() over(partition by thread# order by cnt desc) rn \
	FROM(SELECT count(*) cnt, a.thread#, a.dest_id \
		FROM v$archived_log a, v$archive_dest_status d \
		WHERE completion_time > sysdate-1/24 \
		AND a.dest_id = d.dest_id AND d.status = 'VALID' AND type = 'LOCAL' \
		GROUP BY a.thread#, a.dest_id \
	) \
) \
SELECT nvl(sum(cnt), 0) AS REDOLOG_SWITCH_RATE \
FROM( \
	SELECT t.thread# as thread, nvl((SELECT cnt FROM a WHERE a.thread# = t.thread# AND rn = 1), 0) cnt FROM v$thread t WHERE t.status = 'OPEN' \
)"

#define ORACLE_INSTANCE_REDOLOG_SIZE_INFO_DBS "\
SELECT NVL(ROUND(SUM(blocks * block_size)), 0) AS REDOLOG_SIZE_IN_BYTE \
FROM v$archived_log a, v$archive_dest_status d \
WHERE first_time >= sysdate-1/24 \
	AND a.dest_id = d.dest_id \
	AND d.status = 'VALID' \
	AND type = 'LOCAL'"

#define ORACLE_INSTANCE_ARCHIVE_LOG_BACKUP_INFO_DBS "\
SELECT nvl((SELECT to_char(round((sysdate-(max(c.start_time)))*24*60*60, 0)) \
FROM v$backup_set c, v$backup_piece p \
WHERE c.backup_type = 'L' \
	AND c.set_stamp = p.set_stamp \
	AND c.set_count = p.set_count), \
to_char((cast(SYS_EXTRACT_UTC(SYSTIMESTAMP) as date)-to_date('01011970', 'ddmmyyyy'))*24*60*60)) AS LAST_ARCHIVE_LOG_BACKUP FROM dual"

#define ORACLE_INSTANCE_FULL_BACKUP_INFO_DBS "\
SELECT nvl((SELECT to_char(round((sysdate-(min(completion_time)))*24*60*60,0)) \
FROM( \
	SELECT CASE \
	WHEN(e.enabled = 'READ ONLY' AND e.checkpoint_time < greatest(nvl(a.completion_time, to_date('1970-01-01', 'YYYY-MM-DD')), nvl(b.completion_time, to_date('1970-01-01', 'YYYY-MM-DD')))) then sysdate \
	WHEN(e.enabled = 'READ ONLY' AND e.checkpoint_time >= greatest(nvl(a.completion_time, to_date('1970-01-01', 'YYYY-MM-DD')), nvl(b.completion_time, to_date('1970-01-01', 'YYYY-MM-DD')))) then greatest(nvl(a.completion_time, to_date('1970-01-01', 'YYYY-MM-DD')), nvl(b.completion_time, to_date('1970-01-01', 'YYYY-MM-DD'))) \
	ELSE nvl(a.completion_time, to_date('1970-01-01', 'YYYY-MM-DD')) \
	END completion_time, \
	a.FILE# \
	FROM( \
		SELECT max(c.completion_time) as completion_time, c.FILE# as FILE# \
		FROM v$backup_datafile c, v$backup_piece p \
		WHERE c.FILE# > 0 AND(c.incremental_level = 0 or c.incremental_level is null) \
		AND  c.set_stamp = p.set_stamp  AND c.set_count = p.set_count \
		GROUP BY c.FILE# \
	) a, \
	(SELECT max(c.completion_time) as completion_time, c.FILE# as FILE# \
		FROM v$backup_datafile c, v$backup_piece p \
		WHERE c.file#  > 0 AND c.incremental_level > 0 \
		AND  c.set_stamp = p.set_stamp  AND c.set_count = p.set_count \
		GROUP BY c.FILE#) b, \
	v$datafile e \
	WHERE a.FILE#(+) = e.file# \
	AND  b.FILE#(+) = e.file# \
	AND e.creation_time < ( \
		SELECT max(c.completion_time) as completion_time \
		FROM v$backup_datafile c, v$backup_piece p \
		WHERE(c.incremental_level = 0 or c.incremental_level is null) AND c.file#  > 0 AND  c.set_stamp = p.set_stamp  AND c.set_count = p.set_count \
		))), \
to_char((cast(SYS_EXTRACT_UTC(SYSTIMESTAMP) as date)-to_date('01011970', 'ddmmyyyy'))*24*60*60)) AS LAST_FULL_BACKUP FROM dual"

#define ORACLE_INSTANCE_INCR_BACKUP_FILE_NUM_DBS "\
SELECT to_char(count(*)) AS LAST_INCR_BACKUP_FILE_NUM \
FROM v$backup_datafile c, v$backup_piece p \
WHERE incremental_level > 0 \
AND  c.set_stamp = p.set_stamp \
AND c.set_count = p.set_count \
AND c.completion_time > (SELECT min(nvl(completion_time, to_date('1970-01-01', 'YYYY-MM-DD'))) \
	FROM(SELECT max(c.completion_time) as completion_time, c.FILE# as FILE# \
		FROM  v$backup_datafile c, v$backup_piece p \
		WHERE c.FILE# > 0 AND(c.incremental_level = 0 or c.incremental_level is null) \
		AND  c.set_stamp = p.set_stamp  AND c.set_count = p.set_count \
		GROUP BY c.FILE# \
	) a, v$datafile e \
	WHERE a.FILE#(+) = e.file# \
	AND e.creation_time < (SELECT max(completion_time) \
		FROM(SELECT max(c.completion_time) as completion_time, c.FILE# as FILE# \
			FROM  v$backup_datafile c, v$backup_piece p \
			WHERE c.FILE# > 0 AND(c.incremental_level = 0 or c.incremental_level is null) \
			AND c.set_stamp = p.set_stamp  \
			AND c.set_count = p.set_count \
			GROUP BY c.FILE# \
		) \
		) \
)"

#define ORACLE_INSTANCE_INCR_BACKUP_INFO_DBS "\
SELECT nvl((SELECT to_char(round((sysdate-(min(completion_time)))*24*60*60, 0)) \
FROM( \
	SELECT CASE \
	WHEN(e.enabled = 'READ ONLY' AND e.checkpoint_time < greatest(nvl(a.completion_time, to_date('1970-01-01', 'YYYY-MM-DD')), nvl(b.completion_time, to_date('1970-01-01', 'YYYY-MM-DD')))) then sysdate \
	WHEN(e.enabled = 'READ ONLY' AND e.checkpoint_time >= greatest(nvl(a.completion_time, to_date('1970-01-01', 'YYYY-MM-DD')), nvl(b.completion_time, to_date('1970-01-01', 'YYYY-MM-DD')))) then greatest(nvl(a.completion_time, to_date('1970-01-01', 'YYYY-MM-DD')), nvl(b.completion_time, to_date('1970-01-01', 'YYYY-MM-DD'))) \
	ELSE nvl(b.completion_time, to_date('1970-01-01', 'YYYY-MM-DD')) \
	END completion_time, \
	a.FILE# \
	FROM( \
		SELECT max(c.completion_time) as completion_time, c.FILE# as FILE# \
		FROM v$backup_datafile c, v$backup_piece p \
		WHERE c.FILE# > 0 AND(c.incremental_level = 0 OR c.incremental_level is null) \
		AND  c.set_stamp = p.set_stamp  \
		AND c.set_count = p.set_count \
		GROUP BY c.FILE# \
	) a, \
	(SELECT max(c.completion_time) as completion_time, c.FILE# as FILE# \
		FROM v$backup_datafile c, v$backup_piece p \
		WHERE c.file#  > 0 AND c.incremental_level > 0 \
		AND  c.set_stamp = p.set_stamp  \
		AND c.set_count = p.set_count \
		GROUP BY c.FILE#) b, \
	v$datafile e \
	WHERE a.FILE#(+) = e.file# \
	AND  b.FILE#(+) = e.file# \
	AND e.creation_time < ( \
		SELECT max(c.completion_time) as completion_time \
		FROM v$backup_datafile c, v$backup_piece p \
		WHERE(c.incremental_level = 0 or c.incremental_level is null) AND c.file#  > 0 AND c.set_stamp = p.set_stamp AND c.set_count = p.set_count \
		))), \
to_char((cast(SYS_EXTRACT_UTC(SYSTIMESTAMP) as date)-to_date('01011970', 'ddmmyyyy'))*24*60*60)) AS LAST_INCR_BACKUP FROM dual"

#define ORACLE_INSTANCE_CF_BACKUP_INFO_DBS "\
SELECT nvl((SELECT to_char(round((sysdate-(max(c.start_time)))*24*60*60, 0)) \
	FROM v$backup_set c, v$backup_piece p \
	WHERE c.controlfile_included in('YES', 'SBY') \
	AND c.set_stamp = p.set_stamp \
	AND c.set_count = p.set_count), \
to_char((cast(SYS_EXTRACT_UTC(SYSTIMESTAMP) as date)-to_date('01011970', 'ddmmyyyy'))*24*60*60)) AS LAST_CF_BACKUP FROM dual"

#define ORACLE_V11_DISCOVER_STANDBY_DBS "\
SELECT i.instance_name AS INSTANCE, \
    i.host_name AS HOSTNAME, \
    d.name AS DBNAME \
FROM gv$instance i, gv$database d \
WHERE i.inst_id = d.inst_id \
	AND d.database_role <> 'PRIMARY'"

#define ORACLE_V12_DISCOVER_STANDBY_DBS "\
SELECT i.instance_name AS INSTANCE, \
    i.host_name AS HOSTNAME, \
    d.name AS DBNAME \
FROM gv$instance i, gv$database d \
WHERE i.inst_id = d.inst_id \
	AND d.database_role <> 'PRIMARY' \
	AND d.cdb = 'NO'"

#define ORACLE_STANDBY_LAG_DBS "\
SELECT decode(name, 'apply lag', 'APPLY_LAG', 'transport lag', 'TRANSPORT_LAG', 'NONE') AS PARAM_NAME, \
	nvl(to_char(extract(day from to_dsinterval(value))*24*60*60+extract(hour from to_dsinterval(value))*60*60+extract(minute from to_dsinterval(value))*60+extract(second from to_dsinterval(value))), to_char(0)) AS LAG_VALUE \
FROM v$dataguard_stats \
WHERE name in('apply lag', 'transport lag')"

#define ORACLE_STANDBY_MRP_STATUS_DBS "\
SELECT b.status AS MRP_STATUS, b.mrp_cnt AS MRP_CNT \
FROM(SELECT a.status, a.mrp_cnt, max(a.status) over() mx \
	FROM(SELECT decode(nvl(status, 'NULL'), 'UNUSED', 1, 'ALLOCATED', 2, 'CONNECTED', 3, 'ATTACHED', 4, 'IDLE', 5, 'ERROR', 6, 'OPENING', 7, 'CLOSING', 8, 'WRITING', 9, 'RECEIVING', 10, 'ANNOUNCING', 11, 'REGISTERING', 12, 'WAIT_FOR_LOG', 13, 'WAIT_FOR_GAP', 14, 'APPLYING_LOG', 15, 0) AS status, count(*) AS mrp_cnt \
		FROM v$managed_standby \
		WHERE process LIKE 'MRP%%' \
		GROUP by process, status \
		UNION ALL \
		SELECT 0, 0 FROM dual) a) b \
	WHERE mx <= status"

#define ORACLE_DISCOVER_ARLDEST_DBS "\
SELECT i.INSTANCE_NAME AS INSTANCE, \
	db.name AS DBNAME, \
	bt.dest_name AS ARLDEST \
FROM gv$instance i \
JOIN gv$database db ON(db.INST_ID = i.INST_ID) \
JOIN gv$archive_dest bt ON(bt.INST_ID = i.INST_ID) \
WHERE bt.status != 'INACTIVE' \
	AND db.log_mode = 'ARCHIVELOG'"

#define ORACLE_ARLDEST_INFO_DBS "\
SELECT i.INSTANCE_NAME AS INSTANCE, \
	db.name AS DBNAME, \
	d.dest_name AS ARLDEST, \
	decode(d.status, 'VALID', 1, 'INACTIVE', 2, 'DEFERRED', 3, 'ERROR', 4, 'DISABLED', 5, 'BAD PARAM', 6, 'ALTERNATE', 7, 'FULL', 8, 0) AS LOG_STATUS, \
	decode(d.target, 'PRIMARY', 1, 'STANDBY', 2, 'LOCAL', 3, 'REMOTE', 4, 0) AS LOG_TARGET, \
	decode(d.archiver, 'ARCH', 1, 'FOREGROUND', 2, 'LGWR', 3, 'RFS', 4, 0) AS LOG_ARCHIVER, \
	nvl(d.log_sequence, 0) AS LOG_SEQUENCE, \
	replace(d.error, '\"', '|') AS LOG_ERROR \
FROM gv$archive_dest d, gv$database db, gv$instance i \
WHERE d.status != 'INACTIVE' \
	AND d.inst_id = i.inst_id \
	AND db.log_mode = 'ARCHIVELOG'"

#define ORACLE_INSTANCE_PARAMETERS_INFO_DBS "\
SELECT i.instance_name AS INSTANCE, \
	p.name AS PARAMETER, \
	to_char(p.value) AS PVALUE \
FROM gv$instance i, gv$parameter p \
WHERE i.instance_number = p.inst_id \
	AND p.type IN(3, 6) \
	AND p.isdefault = 'FALSE' \
	AND p.name NOT IN('db_files', 'processes', 'sessions')"

#define ORACLE_V11_INSTANCE_SERVICES_DISCOVERY_DBS "\
SELECT i.instance_name AS INSTANCE, \
	s.name AS SERVICE_NAME \
FROM gv$services s join gv$instance i on(s.inst_id = i.inst_id)"

#define ORACLE_V12_INSTANCE_SERVICES_DISCOVERY_DBS "\
SELECT s.pdb AS PDB, \
	i.instance_name AS INSTANCE, \
	s.name AS SERVICE_NAME \
FROM gv$services s join gv$instance i on(s.inst_id = i.inst_id)"

ZBX_METRIC	parameters_dbmon_oracle[] =
/*	KEY											FLAG				FUNCTION						TEST PARAMETERS */
{
	{"oracle.instance.ping",					CF_HAVEPARAMS,		ORACLE_INSTANCE_PING,			NULL},
	{"oracle.instance.version",					CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.info",					CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.parameter",				CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.patch_info",				CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.resource",				CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.dbfiles",					CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.resumable",				CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.bad_processes",			CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.fra",						CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.redolog_switch_rate",		CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.redolog_size_per_hour",	CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.backup.archivelog",				CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.backup.full",						CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.backup.incr",						CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.backup.incr_file_num",				CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.backup.cf",						CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.db.discovery",						CF_HAVEPARAMS,		ORACLE_DISCOVERY,				NULL},
	{"oracle.db.info",							CF_HAVEPARAMS,		ORACLE_DB_INFO,					NULL},
	{"oracle.db.incarnation",					CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.db.size",							CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.standby.discovery",				CF_HAVEPARAMS,		ORACLE_DISCOVERY,				NULL},
	{"oracle.standby.lag",						CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.standby.mrp_status",				CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.archlogdest.discovery",			CF_HAVEPARAMS,		ORACLE_DISCOVERY,				NULL},
	{"oracle.archlogdest.info",					CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
	{"oracle.instance.parameters",				CF_HAVEPARAMS,		ORACLE_GET_INSTANCE_RESULT,		NULL},
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

int	oracle_make_result(AGENT_REQUEST *request, AGENT_RESULT *result, char *query, zbx_db_result_type result_type, zbx_db_oracle_db_role oracle_need_db_role, unsigned int oracle_need_open_mode)
{
	int							ret = SYSINFO_RET_FAIL, ping = 0;
	char						*oracle_host, *oracle_str_port, *oracle_str_mode, *oracle_instance, *oracle_dbname;
	unsigned short				oracle_port = 0;
	unsigned int				oracle_mode = ZBX_DB_OCI_DEFAULT, oracle_db_open_mode = 0;
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
	oracle_dbname = get_rparam(request, 4);

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

	if (NULL != oracle_conn)
	{
		if (NULL == oracle_dbname || '\0' == *oracle_dbname)
		{
			if (ORA_ANY != oracle_need_db_role)
			{
				if (ORA_STANDBY == oracle_need_db_role)
				{
					if (ZBX_DB_OK == zbx_db_query_select(oracle_conn, &ora_result, ORACLE_CHECK_STANDBY_INST_OPEN_MODE_DBS, oracle_instance))
					{
						oracle_db_open_mode = get_int_one_result(request, result, 0, 0, ora_result);

						zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Instance: %s, Need_Role: %s, Need_Open_Mode: %u, Current_Open_Mode: %u", __func__, request->key, oracle_instance, ORA_DB_ROLE[oracle_need_db_role], oracle_need_open_mode, oracle_db_open_mode);

						if (oracle_db_open_mode > oracle_need_open_mode)
						{
							goto exec_inst_query;
						}
						else
						{
							SET_MSG_RESULT(result, zbx_strdup(NULL, "Database closed for reading information (may be not standby or open mode not mounted or read-only or read-only-with-apply)"));
							goto out;
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
					if (ZBX_DB_OK == zbx_db_query_select(oracle_conn, &ora_result, ORACLE_CHECK_INST_OPEN_MODE_DBS, ORA_DB_ROLE[oracle_need_db_role], oracle_instance))
					{
						oracle_db_open_mode = get_int_one_result(request, result, 0, 0, ora_result);

						zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Instance: %s, Need_Role: %s, Need_Open_Mode: %u, Current_Open_Mode: %u", __func__, request->key, oracle_instance, ORA_DB_ROLE[oracle_need_db_role], oracle_need_open_mode, oracle_db_open_mode);

						if (oracle_db_open_mode > oracle_need_open_mode)
						{
							goto exec_inst_query;
						}
						else
						{
							SET_MSG_RESULT(result, zbx_strdup(NULL, "Instance closed for reading information (may be database not primary or not open with read-only or read-write)"));
							goto out;
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
			}
			else
			{
exec_inst_query:
				if (ZBX_DB_OK == zbx_db_query_select(oracle_conn, &ora_result, query, oracle_instance))
				{
					switch (result_type)
					{
						case ZBX_DB_RES_TYPE_ONEROW:
							ret = make_onerow_json_result(request, result, ora_result);
							break;
						case ZBX_DB_RES_TYPE_TWOCOLL:
							ret = make_multirow_twocoll_json_result(request, result, ora_result);
							break;
						case ZBX_DB_RES_TYPE_MULTIROW:
							ret = make_multi_json_result(request, result, ora_result);
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
		}
		else
		{
			if (ORA_ANY != oracle_need_db_role)
			{
				if (ORA_STANDBY == oracle_need_db_role)
				{
					if (ZBX_DB_OK == zbx_db_query_select(oracle_conn, &ora_result, ORACLE_CHECK_STANDBY_DB_OPEN_MODE_DBS, oracle_instance, oracle_dbname))
					{
						oracle_db_open_mode = get_int_one_result(request, result, 0, 0, ora_result);

						zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Instance: %s, Database: %s, Need_Role: %s, Need_Open_Mode: %u, Current_Open_Mode: %u", __func__, request->key, oracle_instance, oracle_dbname, ORA_DB_ROLE[oracle_need_db_role], oracle_need_open_mode, oracle_db_open_mode);

						if (oracle_db_open_mode > oracle_need_open_mode)
						{
							goto exec_db_query;
						}
						else
						{
							SET_MSG_RESULT(result, zbx_strdup(NULL, "Database closed for reading information (may be not standby or open mode not mounted or read-only or read-only-with-apply)"));
							goto out;
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
					if (ZBX_DB_OK == zbx_db_query_select(oracle_conn, &ora_result, ORACLE_CHECK_DB_OPEN_MODE_DBS, ORA_DB_ROLE[oracle_need_db_role], oracle_instance, oracle_dbname))
					{
						oracle_db_open_mode = get_int_one_result(request, result, 0, 0, ora_result);

						zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Instance: %s, Database: %s, Need_Role: %s, Need_Open_Mode: %u, Current_Open_Mode: %u", __func__, request->key, oracle_instance, oracle_dbname, ORA_DB_ROLE[oracle_need_db_role], oracle_need_open_mode, oracle_db_open_mode);

						if (oracle_db_open_mode > oracle_need_open_mode)
						{
							goto exec_db_query;
						}
						else
						{
							SET_MSG_RESULT(result, zbx_strdup(NULL, "Database closed for reading information (may be not primary or not open with read-only or read-write)"));
							goto out;
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
			}
			else
			{
exec_db_query:
				if (ZBX_DB_OK == zbx_db_query_select(oracle_conn, &ora_result, query, oracle_instance, oracle_dbname))
				{
					switch (result_type)
					{
						case ZBX_DB_RES_TYPE_ONEROW:
							ret = make_onerow_json_result(request, result, ora_result);
							break;
						case ZBX_DB_RES_TYPE_TWOCOLL:
							ret = make_multirow_twocoll_json_result(request, result, ora_result);
							break;
						case ZBX_DB_RES_TYPE_MULTIROW:
							ret = make_multi_json_result(request, result, ora_result);
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
		}
	}
	else
	{
		zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error connecting to database", __func__, request->key);
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Error connecting to database"));
		ret = SYSINFO_RET_FAIL;
	}

out:
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
		ret = oracle_make_result(request, result, ORACLE_VERSION_DBS, ZBX_DB_RES_TYPE_NOJSON, ORA_ANY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.instance.info", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_INFO_DBS, ZBX_DB_RES_TYPE_ONEROW, ORA_ANY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.instance.parameter", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_PARAMETER_INFO_DBS, ZBX_DB_RES_TYPE_TWOCOLL, ORA_ANY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.instance.patch_info", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_LASTPATCH_INFO_DBS, ZBX_DB_RES_TYPE_ONEROW, ORA_PRIMARY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.instance.resource", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_RESOURCE_INFO_DBS, ZBX_DB_RES_TYPE_TWOCOLL, ORA_ANY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.instance.dbfiles", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_DB_FILES_CURRENT_DBS, ZBX_DB_RES_TYPE_NOJSON, ORA_ANY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.instance.resumable", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_RESUMABLE_COUNT_DBS, ZBX_DB_RES_TYPE_NOJSON, ORA_PRIMARY, 1);
	}
	else if (0 == strcmp((const char*)"oracle.instance.bad_processes", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_COUNT_BAD_PROCESSES_DBS, ZBX_DB_RES_TYPE_NOJSON, ORA_ANY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.instance.fra", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_FRA_INFO_DBS, ZBX_DB_RES_TYPE_ONEROW, ORA_ANY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.instance.redolog_switch_rate", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_REDOLOG_SWITCH_RATE_INFO_DBS, ZBX_DB_RES_TYPE_NOJSON, ORA_PRIMARY, 1);
	}
	else if (0 == strcmp((const char*)"oracle.instance.redolog_size_per_hour", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_REDOLOG_SIZE_INFO_DBS, ZBX_DB_RES_TYPE_NOJSON, ORA_PRIMARY, 1);
	}
	else if (0 == strcmp((const char*)"oracle.backup.archivelog", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_ARCHIVE_LOG_BACKUP_INFO_DBS, ZBX_DB_RES_TYPE_NOJSON, ORA_ANY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.backup.full", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_FULL_BACKUP_INFO_DBS, ZBX_DB_RES_TYPE_NOJSON, ORA_ANY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.backup.incr", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_INCR_BACKUP_INFO_DBS, ZBX_DB_RES_TYPE_NOJSON, ORA_ANY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.backup.incr_file_num", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_INCR_BACKUP_FILE_NUM_DBS, ZBX_DB_RES_TYPE_NOJSON, ORA_ANY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.backup.cf", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_CF_BACKUP_INFO_DBS, ZBX_DB_RES_TYPE_NOJSON, ORA_ANY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.db.incarnation", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_DB_INCARNATION_INFO_DBS, ZBX_DB_RES_TYPE_NOJSON, ORA_ANY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.db.size", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_DB_SIZE_INFO_DBS, ZBX_DB_RES_TYPE_NOJSON, ORA_ANY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.standby.lag", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_STANDBY_LAG_DBS, ZBX_DB_RES_TYPE_TWOCOLL, ORA_STANDBY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.standby.mrp_status", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_STANDBY_MRP_STATUS_DBS, ZBX_DB_RES_TYPE_ONEROW, ORA_STANDBY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.archlogdest.info", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_ARLDEST_INFO_DBS, ZBX_DB_RES_TYPE_MULTIROW, ORA_ANY, 0);
	}
	else if (0 == strcmp((const char*)"oracle.instance.parameters", request->key))
	{
		ret = oracle_make_result(request, result, ORACLE_INSTANCE_PARAMETERS_INFO_DBS, ZBX_DB_RES_TYPE_MULTIROW, ORA_ANY, 0);
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
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Oracle version < 12, use query '%s'", __func__, request->key, query);

					if (0 == strcmp((const char*)"oracle.standby.discovery", request->key))
					{
						query = ORACLE_V11_DISCOVER_STANDBY_DBS;
						ret = ZBX_DB_OK;
					}
					else if (0 == strcmp((const char*)"oracle.db.discovery", request->key))
					{
						query = ORACLE_V11_DISCOVER_DB_DBS;
						ret = ZBX_DB_OK;
					}
					else if (0 == strcmp((const char*)"oracle.archlogdest.discovery", request->key))
					{
						query = ORACLE_DISCOVER_ARLDEST_DBS;
						ret = ZBX_DB_OK;
					}
					else
					{
						SET_MSG_RESULT(result, zbx_strdup(NULL, "Unknown discovery request key"));
						ret = SYSINFO_RET_FAIL;
					}
				}
				else
				{
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Oracle version >= 12, use query '%s'", __func__, request->key, query);

					if (0 == strcmp((const char*)"oracle.standby.discovery", request->key))
					{
						query = ORACLE_V12_DISCOVER_STANDBY_DBS;
						ret = ZBX_DB_OK;
					}
					else if (0 == strcmp((const char*)"oracle.db.discovery", request->key))
					{
						query = ORACLE_V12_DISCOVER_DB_DBS;
						ret = ZBX_DB_OK;
					}
					else if (0 == strcmp((const char*)"oracle.archlogdest.discovery", request->key))
					{
						query = ORACLE_DISCOVER_ARLDEST_DBS;
						ret = ZBX_DB_OK;
					}
					else
					{
						SET_MSG_RESULT(result, zbx_strdup(NULL, "Unknown discovery request key"));
						ret = SYSINFO_RET_FAIL;
					}
				}
			}
			else
			{
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get Oracle version in discovery procedure"));
				ret = SYSINFO_RET_FAIL;
			}

			zbx_db_clean_result(&ora_result);
		}
		else
		{
			zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error executing query", __func__, request->key);
			SET_MSG_RESULT(result, zbx_strdup(NULL, "Error executing query"));
			ret = SYSINFO_RET_FAIL;
		}

		zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Oracle instance '%s'", __func__, request->key, oracle_instance);

		if (ZBX_DB_OK == ret)
		{
			if (zbx_db_query_select(oracle_conn, &ora_result, query) == ZBX_DB_OK)
			{
				ret = make_discovery_result(request, result, ora_result);
				zbx_db_clean_result(&ora_result);
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error executing discovery query", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error executing discovery query"));
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

	zbx_db_close_db(oracle_conn);
	zbx_db_clean_connection(oracle_conn);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __func__, request->key);

	return ret;
}

static int	ORACLE_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
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

#endif
#endif