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

#define PGSQL_VERSION_DBS "SELECT VERSION() AS VERSION;"

#define PGSQL_VERSION_INT_DBS "SELECT current_setting('server_version_num')::int;"

#define PGSQL_SERVER_INFO_DBS "\
SELECT  VERSION() AS VERSION, \
	date_part('epoch', pg_postmaster_start_time())::int AS STARTUPTIME, \
	date_part('epoch', now() - pg_postmaster_start_time())::int AS UPTIME;"

#define PGSQL_DB_DISCOVERY_DBS  "\
SELECT \
    d.oid AS OID, \
    d.datname AS DBNAME, \
    pg_catalog.pg_encoding_to_char(d.encoding) AS ENCODING, \
    d.datcollate AS LC_COLLATE, \
    d.datctype AS LC_CTYPE, \
    pg_catalog.pg_get_userbyid(d.datdba) AS OWNER, \
    t.spcname AS TABLESPACE, \
    pg_catalog.shobj_description(d.oid, 'pg_database') AS DESCRIPTION, \
    age(d.datfrozenxid) AS AGE, \
    d.datconnlimit AS CONNLIM \
FROM pg_catalog.pg_database d \
    JOIN pg_catalog.pg_tablespace t on d.dattablespace = t.oid \
WHERE \
    d.datallowconn = 't' \
    AND d.datistemplate = 'n' \
ORDER BY 1;"

#define PGSQL_DB_INFO_DBS "\
SELECT \
	d.oid AS oid, \
	d.datname AS DBNAME, \
	pg_catalog.pg_encoding_to_char(d.encoding) AS ENCODING, \
	d.datcollate AS LC_COLLATE, \
	d.datctype AS LC_CTYPE, \
	pg_catalog.pg_get_userbyid(d.datdba) AS OWNER, \
	t.spcname AS TABLESPACE, \
	pg_catalog.shobj_description(d.oid, 'pg_database') AS DESCRIPTION, \
	age(d.datfrozenxid) AS AGE, \
	d.datconnlimit AS CONNLIM, \
	pg_database_size(d.datname::text) AS DBSIZE \
FROM pg_catalog.pg_database d \
	JOIN pg_catalog.pg_tablespace t on d.dattablespace = t.oid \
WHERE \
	d.datallowconn = 't' \
	AND d.datistemplate = 'n' \
ORDER BY 1;"

#define PGSQL_DB_LOCKS_DBS "\
WITH T AS \
(SELECT db.datname AS DBNAME, \
	lower(replace(Q.mode, 'Lock', '')) AS MODE, \
	coalesce(T.qty, 0) val \
	FROM pg_database db \
	JOIN( \
		VALUES('AccessShareLock'), ('RowShareLock'), ('RowExclusiveLock'), ('ShareUpdateExclusiveLock'), ('ShareLock'), ('ShareRowExclusiveLock'), ('ExclusiveLock'), ('AccessExclusiveLock')) Q(MODE) ON TRUE NATURAL \
	LEFT JOIN \
	(SELECT datname, \
		MODE, \
		count(MODE) qty \
		FROM pg_locks lc \
		RIGHT JOIN pg_database db ON db.oid = lc.database \
		GROUP BY 1, 2) T \
	WHERE NOT db.datistemplate \
	ORDER BY 1, 2) \
	SELECT json_object_agg(dbname, row_to_json(T2)) \
	FROM \
	(SELECT dbname, \
		sum(val) AS TOTAL, \
		sum(CASE \
			WHEN MODE = 'accessexclusive' THEN val \
			END) AS ACCESSEXCLUSIVE, \
		sum(CASE \
			WHEN MODE = 'accessshare' THEN val \
			END) AS ACCESSSHARE, \
		sum(CASE \
			WHEN MODE = 'exclusive' THEN val \
			END) AS EXCLUSIVE, \
		sum(CASE \
			WHEN MODE = 'rowexclusive' THEN val \
			END) AS ROWEXCLUSIVE, \
		sum(CASE \
			WHEN MODE = 'rowshare' THEN val \
			END) AS ROWSHARE, \
		sum(CASE \
			WHEN MODE = 'share' THEN val \
			END) AS SHARE, \
		sum(CASE \
			WHEN MODE = 'sharerowexclusive' THEN val \
			END) AS SHAREROWEXCLUSIVE, \
		sum(CASE \
			WHEN MODE = 'shareupdateexclusive' THEN val \
			END) AS SHAREUPDATEEXCLUSIVE \
		FROM T \
		GROUP BY dbname) T2"

#define PGSQL_DB_STAT_SUM_DBS "\
SELECT row_to_json(T) \
FROM( \
	SELECT \
	sum(numbackends) as numbackends \
	, sum(xact_commit) as xact_commit \
	, sum(xact_rollback) as xact_rollback \
	, sum(blks_read) as blks_read \
	, sum(blks_hit) as blks_hit \
	, sum(tup_returned) as tup_returned \
	, sum(tup_fetched) as tup_fetched \
	, sum(tup_inserted) as tup_inserted \
	, sum(tup_updated) as tup_updated \
	, sum(tup_deleted) as tup_deleted \
	, sum(conflicts) as conflicts \
	, sum(temp_files) as temp_files \
	, sum(temp_bytes) as temp_bytes \
	, sum(deadlocks) as deadlocks \
	, %s as checksum_failures \
	, sum(blk_read_time) as blk_read_time \
	, sum(blk_write_time) as blk_write_time \
	FROM pg_catalog.pg_stat_database \
) T;"

#define PGSQL_DB_STAT_DBS "\
SELECT json_object_agg(coalesce(datname, 'null'), row_to_json(T)) \
FROM( \
	SELECT \
	datname \
	, numbackends as numbackends \
	, xact_commit as xact_commit \
	, xact_rollback as xact_rollback \
	, blks_read as blks_read \
	, blks_hit as blks_hit \
	, tup_returned as tup_returned \
	, tup_fetched as tup_fetched \
	, tup_inserted as tup_inserted \
	, tup_updated as tup_updated \
	, tup_deleted as tup_deleted \
	, conflicts as conflicts \
	, temp_files as temp_files \
	, temp_bytes as temp_bytes \
	, deadlocks as deadlocks \
	, %s as checksum_failures \
	, blk_read_time as blk_read_time \
	, blk_write_time as blk_write_time \
	FROM pg_catalog.pg_stat_database \
	WHERE datname is not null \
) T;"

#define PGSQL_CONNECTIONS_INFO_DBS "\
SELECT row_to_json(T) \
FROM( \
	SELECT \
	sum(CASE WHEN state = 'active' THEN 1 ELSE 0 END) AS active, \
	sum(CASE WHEN state = 'idle' THEN 1 ELSE 0 END) AS idle, \
	sum(CASE WHEN state = 'idle in transaction' THEN 1 ELSE 0 END) AS idle_in_transaction, \
	sum(CASE WHEN state = 'idle in transaction (aborted)' THEN 1 ELSE 0 END) AS idle_in_transaction_aborted, \
	sum(CASE WHEN state = 'fastpath function call' THEN 1 ELSE 0 END) AS fastpath_function_call, \
	sum(CASE WHEN state = 'disabled' THEN 1 ELSE 0 END) AS disabled, \
	count(*) AS total, \
	count(*) * 100 / (SELECT current_setting('max_connections')::int) AS total_pct, \
	sum(%s) AS waiting, \
	(SELECT count(*) FROM pg_prepared_xacts) AS prepared \
	FROM pg_stat_activity WHERE datid is not NULL) \
T;"

#define PGSQL_TRANSACTIONS_INFO_DBS "\
SELECT row_to_json(T) \
FROM( \
	SELECT \
	coalesce(extract(epoch FROM max(CASE WHEN state = 'idle in transaction' THEN age(now(), xact_start) END)), 0) AS idle, \
	coalesce(extract(epoch FROM max(CASE WHEN state = 'active' THEN age(now(), xact_start) END)), 0) AS active, \
	coalesce(extract(epoch FROM max(CASE WHEN %s THEN age(now(), xact_start) END)), 0) AS waiting, \
	(SELECT coalesce(extract(epoch FROM max(age(now(), prepared))), 0) \
		FROM pg_prepared_xacts) AS prepared \
	FROM pg_stat_activity) T;"

#define PGSQL_WAL_STAT_DBS "\
SELECT row_to_json(T) \
FROM( \
	SELECT pg_wal_lsn_diff(pg_current_wal_lsn(), '0/00000000') AS WRITE, \
	count(*) * 16 * 1024 * 1024 AS TOTAL_SIZE, \
	count(*) \
	FROM pg_ls_waldir() AS COUNT \
) T;"

#define PGSQL_XLOG_STAT_DBS "\
SELECT row_to_json(T) \
FROM( \
	SELECT pg_xlog_location_diff(pg_current_xlog_location(), '0/00000000') AS WRITE, \
	count(*) * 16 * 1024 * 1024 AS TOTAL_SIZE, \
	count(*) \
	FROM pg_ls_dir('pg_xlog') AS t(fname) WHERE fname <> 'archive_status' \
) T;"

#define PGSQL_OLDEST_XID_DBS "\
SELECT greatest(max(age(backend_xmin)), max(age(backend_xid))) \
FROM pg_catalog.pg_stat_activity;"

#define PGSQL_CACHE_HIT_DBS "\
SELECT round(sum(blks_hit) * 100 / sum(blks_hit + blks_read), 2) \
FROM pg_catalog.pg_stat_database;"

#define PGSQL_BGWRITER_DBS "\
SELECT row_to_json(T) \
FROM( \
	SELECT \
	checkpoints_timed \
	, checkpoints_req \
	, checkpoint_write_time \
	, checkpoint_sync_time \
	, buffers_checkpoint \
	, buffers_clean \
	, maxwritten_clean \
	, buffers_backend \
	, buffers_backend_fsync \
	, buffers_alloc \
	, stats_reset \
	, date_part('epoch', stats_reset)::int AS stats_reset_unix \
	FROM pg_catalog.pg_stat_bgwriter \
) T;"

#define PGSQL_AUTOVACUUM_COUNT_DBS "\
SELECT count(*) \
FROM pg_catalog.pg_stat_activity \
WHERE query like '%%autovacuum%%' \
	AND state <> 'idle' \
	AND pid <> pg_catalog.pg_backend_pid();"

#define PGSQL_ARCHIVE_COUNT_DBS "\
SELECT row_to_json(T) \
FROM( \
	SELECT archived_count, failed_count \
	FROM pg_stat_archiver \
) T;"

#define PGSQL_ARCHIVE_SIZE_DBS "\
SELECT row_to_json(T) \
FROM( \
	SELECT count(name) AS count_files, \
	coalesce(sum((pg_stat_file('./%s/' || rtrim(ready.name, '.ready'))).size), 0) AS size_files \
	FROM( \
		SELECT name \
		FROM pg_ls_dir('./%s/archive_status') name \
		WHERE right(name, 6) = '.ready' \
	) ready \
) T;"

#define PGSQL_PREPARED_COUNT_DBS "\
SELECT row_to_json(T) \
FROM( \
	SELECT COUNT(*) as count_prepared, coalesce(ROUND(MAX(EXTRACT(EPOCH FROM(now() - prepared)))), 0)::bigint AS oldest_prepared \
	FROM pg_catalog.pg_prepared_xacts \
) T;"

#define PGSQL_NOT_DEFAULT_SETTINGS_DBS "\
SELECT json_build_object( \
	'extensions', ( \
		SELECT array_agg(extname) FROM ( \
			SELECT extname FROM pg_extension ORDER BY extname \
		) AS e \
	), \
	'settings', ( \
		SELECT json_object(array_agg(name), array_agg(setting)) FROM ( \
			SELECT name, setting FROM pg_settings WHERE (source NOT IN ('default', 'session') OR name = 'server_version_num') AND name NOT IN ('application_name') ORDER BY name \
		) AS s \
	) \
);"

#define PGSQL_REPLICATION_STANDBY_DISCOVERY_DBS "\
SELECT client_addr AS STANDBY FROM pg_stat_replication;"

// Query to track lag in bytes
// sending_lag could indicate heavy load on primary
// receiving_lag could indicate network issues or replica under heavy load
// replaying_lag could indicate replica under heavy load
#define PGSQL_REPLICATION_STAT_DBS "\
SELECT \
  client_addr as ip_address, \
  usename as user_name, \
  application_name as app_name, \
  date_part('epoch', backend_start)::int AS backend_start_unix, \
  state, sync_state, \
  (pg_wal_lsn_diff(pg_current_wal_lsn(), sent_lsn)/1024)::int as sending_lag, \
  (pg_wal_lsn_diff(sent_lsn, flush_lsn)/1024)::int as receiving_lag, \
  (pg_wal_lsn_diff(flush_lsn, replay_lsn)/1024)::int as replaying_lag, \
  (pg_wal_lsn_diff(pg_current_wal_lsn(), replay_lsn)/1024)::int as total_lag \
FROM pg_stat_replication;"

// Get replication role (0 - Master, 1 - Standby)
#define PGSQL_REPLICATION_ROLE_DBS "\
SELECT pg_is_in_recovery()::int AS REPLICATION_ROLE;"

// Get replication standby count
#define PGSQL_REPLICATION_STANDBY_COUNT_DBS "\
SELECT count(*) AS REPLICATION_STANDBY_COUNT FROM pg_stat_replication;"

// Get replication status (0 - Down, 1 - Up, 2 - Master, 3 - Not supported)
#define PGSQL_REPLICATION_STATUS_DBS "\
SELECT \
	CASE \
		WHEN (SELECT pg_is_in_recovery()::int) = 0 THEN 2 \
		ELSE (SELECT COUNT(*) AS REPLICATION_RECEIVER_COUNT FROM pg_stat_wal_receiver) \
	END AS REPLICATION_STATUS;"

// Get replication replay paused status from PostgreSQL <= 9.6 (0 - Running, 1 - Paused)
#define PGSQL_REPLICATION_REPLAY_PAUSED_STATUS_V96_DBS "SELECT pg_is_xlog_replay_paused()::int;"

// Get replication replay paused status from PostgreSQL >= 10.0 (0 - Running, 1 - Paused)
#define PGSQL_REPLICATION_REPLAY_PAUSED_STATUS_V10_DBS "SELECT pg_is_wal_replay_paused()::int;"

// Get lag in second from PostgreSQL < 10.0
#define PGSQL_REPLICATION_LAG_IN_SEC_V9_DBS "\
SELECT \
	CASE \
		WHEN pg_last_xlog_receive_location() = pg_last_xlog_replay_location() THEN 0 \
		ELSE COALESCE(EXTRACT(EPOCH FROM now() - pg_last_xact_replay_timestamp())::integer, 0) \
	END AS LAG_IN_SEC;"

// Get lag in second from PostgreSQL >= 10.0
#define PGSQL_REPLICATION_LAG_IN_SEC_V10_DBS "\
SELECT \
	CASE \
		WHEN pg_last_wal_receive_lsn() = pg_last_wal_replay_lsn() THEN 0 \
		ELSE COALESCE(EXTRACT(EPOCH FROM now() - pg_last_xact_replay_timestamp())::integer, 0) \
	END AS LAG_IN_SEC;"

// Get lag in byte from PostgreSQL < 10.0
#define PGSQL_REPLICATION_LAG_IN_BYTE_V9_DBS "\
SELECT \
	CASE \
		WHEN (SELECT pg_is_in_recovery()::int) = 0 THEN 0 \
		ELSE (SELECT pg_catalog.pg_xlog_location_diff(received_lsn, pg_last_xlog_replay_location())::int FROM pg_stat_wal_receiver) \
	END AS LAG_IN_BYTE;"

// Get lag in byte from PostgreSQL >= 10.0
#define PGSQL_REPLICATION_LAG_IN_BYTE_V10_DBS "\
SELECT \
	CASE \
		WHEN (SELECT pg_is_in_recovery()::int) = 0 THEN 0 \
		ELSE (SELECT pg_catalog.pg_wal_lsn_diff(received_lsn, pg_last_wal_replay_lsn())::int FROM pg_stat_wal_receiver) \
	END AS LAG_IN_BYTE;"

// Get replication slot info from PostgreSQL < 9.6
#define PGSQL_REPLICATION_SLOTS_INFO_V95_DBS "\
SELECT slot_name, COALESCE(plugin, '') AS plugin, slot_type, COALESCE(database, '') AS database, (active)::int, xmin, catalog_xmin, restart_lsn, \
	round((restart_lsn-redo_location), 0)::int AS behind \
FROM pg_control_checkpoint(), pg_replication_slots \
ORDER BY slot_name ASC;"

// Get replication slot info from PostgreSQL = 9.6
#define PGSQL_REPLICATION_SLOTS_INFO_V96_DBS "\
SELECT slot_name, COALESCE(plugin, '') AS plugin, slot_type, COALESCE(database, '') AS database, (active)::int, xmin, catalog_xmin, restart_lsn, confirmed_flush_lsn, \
	round((restart_lsn-redo_location), 0)::int AS behind \
FROM pg_control_checkpoint(), pg_replication_slots \
ORDER BY slot_name ASC;"

// Get replication slot info from PostgreSQL >= 10.0 and < 13.0
#define PGSQL_REPLICATION_SLOTS_INFO_V10_DBS "\
SELECT slot_name, COALESCE(plugin, '') AS plugin, slot_type, COALESCE(database, '') AS database, (temporary)::int, (active)::int, xmin, catalog_xmin, restart_lsn, \
	confirmed_flush_lsn, round((restart_lsn-redo_lsn),0)::int AS behind \
FROM pg_control_checkpoint(), pg_replication_slots \
ORDER BY slot_name ASC;"

// Get replication slot info from PostgreSQL >= 13.0
#define PGSQL_REPLICATION_SLOTS_INFO_V13_DBS "\
SELECT slot_name, COALESCE(plugin, '') AS plugin, slot_type, COALESCE(database, '') AS database, (temporary)::int, (active)::int, xmin, catalog_xmin, restart_lsn, \
	confirmed_flush_lsn, wal_status, safe_wal_size, round((restart_lsn-redo_lsn), 0)::int AS behind \
FROM pg_control_checkpoint(), pg_replication_slots \
ORDER BY slot_name ASC;"

// Get exclusive (non-exclusive) backup status
#define PGSQL_IS_IN_BACKUP_DBS " \
SELECT row_to_json(T) \
FROM(\
	SELECT pg_is_in_backup()::int as ISINBACKUP, \
	COALESCE(date_part('epoch', pg_backup_start_time())::int, 0) AS BACKUPSTARTTIME, \
	COALESCE(date_part('epoch', now() - pg_backup_start_time())::int, 0) AS BACKUPDURATION \
) T;"

ZBX_METRIC	parameters_dbmon_pgsql[] =
/*	KEY			FLAG		FUNCTION		TEST PARAMETERS */
{
	{"pgsql.ping",					CF_HAVEPARAMS,		PGSQL_PING,			NULL},
	{"pgsql.version",				CF_HAVEPARAMS,		PGSQL_VERSION,		NULL},
	{"pgsql.version.full",			CF_HAVEPARAMS,		PGSQL_VERSION,		NULL},
	{"pgsql.server.info",			CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.db.discovery",			CF_HAVEPARAMS,		PGSQL_DB_DISCOVERY,	NULL},
	{"pgsql.db.info",				CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.db.locks",				CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.db.stat.sum",			CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.db.stat",				CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.connections",			CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.transactions",			CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.wal.stat",				CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.oldest.xid",			CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.cache.hit",				CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.bgwriter",				CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.autovacuum.count",		CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.archive.count",			CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.archive.size",			CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.prepared.transactions",	CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.settings",				CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.replication.info",		CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.replication.stat",		CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.replication.role",		CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.replication.count",		CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.replication.status",	CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.replication.replay",	CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.replication.lag_byte",	CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.replication.lag_sec",	CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.replication.slots",		CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{"pgsql.backup.exclusive",		CF_HAVEPARAMS,		PGSQL_GET_RESULT,	NULL},
	{NULL}
};

#if !defined(_WINDOWS) && !defined(__MINGW32__)
static int	pgsql_ping(AGENT_REQUEST *request, AGENT_RESULT *result)
#else
static int	pgsql_ping(AGENT_REQUEST *request, AGENT_RESULT *result, HANDLE timeout_event)
#endif
{
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
	/* zbx_execute_dbmon_threaded_metric() on MS Windows */
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
	return zbx_execute_dbmon_threaded_metric(pgsql_ping, request, result);
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
			version 9.4.2	=> 90412 ?
			version 9.5.2	=> 90502
			version 9.6.0	=> 90600
			version 9.6.16	=> 90616
			version 10.5	=> 100500
			version 10.11	=> 100011
			version 12.1	=> 120001
			*/
			version = zbx_db_version(pgsql_conn);

			if (0 != version)
			{
				if (version < 100000)
				{
					zbx_snprintf(pgsql_ver, sizeof(pgsql_ver), "%lu.%lu.%lu", version / 10000, (version % 10000) / 100, (version % 10000) % 100);
				}
				else
				{
					zbx_snprintf(pgsql_ver, sizeof(pgsql_ver), "%lu.%lu.%lu", version / 10000, ((version % 10000) / 100) == 0 ? (version % 10000) % 100 : (version % 10000) / 100, ((version % 10000) / 100) == 0 ? (version % 10000) / 100 : (version % 10000) % 100);
				}

				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): PgSQL version: %s", __func__, request->key, pgsql_ver);
				SET_TEXT_RESULT(result, zbx_strdup(NULL, pgsql_ver));
				ret = SYSINFO_RET_OK;
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL version.", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL version."));
				ret = SYSINFO_RET_FAIL;
			}
		}
		else
		{
			if (ZBX_DB_OK == zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_VERSION_DBS))
			{
				ret = make_result(request, result, pgsql_result, ZBX_DB_RES_TYPE_NOJSON);
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

	if (0 == strcmp(request->key, "pgsql.version"))
	{
		ret = pgsql_version(request, result, 0);
	}
	else if (0 == strcmp(request->key, "pgsql.version.full"))
	{
		ret = pgsql_version(request, result, 1);
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
static int	pgsql_get_discovery(AGENT_REQUEST *request, AGENT_RESULT *result)
#else
static int	pgsql_get_discovery(AGENT_REQUEST *request, AGENT_RESULT *result, HANDLE timeout_event)
#endif
{
	int							ret = SYSINFO_RET_FAIL;
	char						*pgsql_conn_string;
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

#if defined(_WINDOWS) && defined(__MINGW32__)
	/* 'timeout_event' argument is here to make the pgsql_get_discovery() prototype as required by */
	/* zbx_execute_dbmon_threaded_metric() on MS Windows */
	ZBX_UNUSED(timeout_event);
#endif

	pgsql_conn = zbx_db_connect_pgsql(pgsql_conn_string);

	if (NULL != pgsql_conn)
	{
		if (0 == strcmp(request->key, "pgsql.db.discovery"))
		{
			ret = zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_DB_DISCOVERY_DBS);
		}
		else
		{
			SET_MSG_RESULT(result, zbx_strdup(NULL, "Unknown discovery request key."));
			ret = SYSINFO_RET_FAIL;
			goto out;
		}

		if (ZBX_DB_OK == ret)
		{
			ret = make_result(request, result, pgsql_result, ZBX_DB_RES_TYPE_DISCOVERY);
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
out:
	zbx_db_close_db(pgsql_conn);
	zbx_db_clean_connection(pgsql_conn);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s): %s", __func__, request->key, zbx_sysinfo_ret_string(ret));

	return ret;
}

int	PGSQL_DB_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	return zbx_execute_dbmon_threaded_metric(pgsql_get_discovery, request, result);
	//return pgsql_get_discovery(request, result, NULL);
}

static int	pgsql_make_result(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query, zbx_db_result_type result_type)
{
	int							ret = SYSINFO_RET_FAIL, db_ret = ZBX_DB_ERROR;
	char						*pg_conn_string;
	struct zbx_db_connection	*pgsql_conn;
	struct zbx_db_result		pgsql_result;
	unsigned long				version;
	const char					*pg_db_sum, *pg_wait_event, *pg_wal_dir;
	unsigned int				pg_replication_role = 0; // 0 - Master, 1 - Standby

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
		if (0 == strcmp(request->key, "pgsql.db.stat.sum"))
		{
			version = zbx_db_version(pgsql_conn);

			if (0 != version)
			{
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): PgSQL version: %lu", __func__, request->key, version);

				if (version >= 120000)
				{
					pg_db_sum = "sum(checksum_failures)";
				}
				else
				{
					pg_db_sum = "null";
				}

				db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, query, pg_db_sum);
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL version.", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL version."));
				ret = SYSINFO_RET_FAIL;
				goto out;
			}
		}
		else if (0 == strcmp(request->key, "pgsql.db.stat"))
		{
			version = zbx_db_version(pgsql_conn);

			if (0 != version)
			{
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): PgSQL version: %lu", __func__, request->key, version);

				if (version >= 120000)
				{
					pg_db_sum = "checksum_failures";
				}
				else
				{
					pg_db_sum = "null";
				}

				db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, query, pg_db_sum);
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL version.", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL version."));
				ret = SYSINFO_RET_FAIL;
				goto out;
			}
		}
		else if (0 == strcmp(request->key, "pgsql.connections"))
		{
			version = zbx_db_version(pgsql_conn);

			if (0 != version)
			{
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): PgSQL version: %lu", __func__, request->key, version);

				if (version >= 90600)
				{
					pg_wait_event = "CASE WHEN wait_event IS NOT NULL THEN 1 ELSE 0 END";
				}
				else
				{
					pg_wait_event = "CASE WHEN waiting = 't' THEN 1 ELSE 0 END";
				}

				db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, query, pg_wait_event);
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL version.", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL version."));
				ret = SYSINFO_RET_FAIL;
				goto out;
			}
		}
		else if (0 == strcmp(request->key, "pgsql.transactions"))
		{
			version = zbx_db_version(pgsql_conn);

			if (0 != version)
			{
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): PgSQL version: %lu", __func__, request->key, version);

				if (version >= 90600)
				{
					pg_wait_event = "wait_event IS NOT NULL";
				}
				else
				{
					pg_wait_event = "waiting = 't'";
				}

				db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, query, pg_wait_event);
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL version.", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL version."));
				ret = SYSINFO_RET_FAIL;
				goto out;
			}
		}
		else if (0 == strcmp(request->key, "pgsql.wal.stat"))
		{
			pg_replication_role = 1;

			if (ZBX_DB_OK == zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_ROLE_DBS))
			{
				pg_replication_role = get_int_one_result(request, result, 0, 0, pgsql_result);
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Replication role: %u", __func__, request->key, pg_replication_role);
				zbx_db_clean_result(&pgsql_result);
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL replication recovery role.", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL replication recovery role."));
				ret = SYSINFO_RET_FAIL;
				goto out;
			}

			// Only Master
			if (0 == pg_replication_role)
			{
				version = zbx_db_version(pgsql_conn);

				if (0 != version)
				{
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): PgSQL version: %lu", __func__, request->key, version);

					if (version >= 100000)
					{
						query = PGSQL_WAL_STAT_DBS;
					}
					else
					{
						query = PGSQL_XLOG_STAT_DBS;
					}

					db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", query);
				}
				else
				{
					zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL version.", __func__, request->key);
					SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL version."));
					ret = SYSINFO_RET_FAIL;
					goto out;
				}
			}
			else
			{
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): This is a replica, getting information is available only on the master.", __func__, request->key);
				SET_TEXT_RESULT(result, zbx_strdup(NULL, "{\"write\":0,\"total_size\":0,\"count\":0}"));
				ret = SYSINFO_RET_OK;
				goto out;
			}
		}
		else if (0 == strcmp(request->key, "pgsql.archive.size"))
		{
			version = zbx_db_version(pgsql_conn);

			if (0 != version)
			{
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): PgSQL version: %lu", __func__, request->key, version);

				if (version >= 100000)
				{
					pg_wal_dir = "pg_wal";
				}
				else
				{
					pg_wal_dir = "pg_xlog";
				}

				db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, query, pg_wal_dir, pg_wal_dir);
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL version.", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL version."));
				ret = SYSINFO_RET_FAIL;
				goto out;
			}
		}
		else if ( (0 == strcmp(request->key, "pgsql.replication.info")) || (0 == strcmp(request->key, "pgsql.replication.stat")) )
		{
			pg_replication_role = 1;

			if (ZBX_DB_OK == zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_ROLE_DBS))
			{
				pg_replication_role = get_int_one_result(request, result, 0, 0, pgsql_result);
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Replication role: %u", __func__, request->key, pg_replication_role);
				zbx_db_clean_result(&pgsql_result);
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL replication recovery role.", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL replication recovery role."));
				ret = SYSINFO_RET_FAIL;
				goto out;
			}

			// Only master
			if (0 == pg_replication_role)
			{
				db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", query);
			}
			else
			{
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): This is a replica, getting information is available only on the master.", __func__, request->key);
				SET_TEXT_RESULT(result, zbx_strdup(NULL, "[]"));
				ret = SYSINFO_RET_OK;
				goto out;
			}
		}
		else if (0 == strcmp(request->key, "pgsql.replication.status"))
		{
			version = zbx_db_version(pgsql_conn);

			if (0 != version)
			{
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): PgSQL version: %lu", __func__, request->key, version);

				if (version < 90600)
				{
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): This version of PostgreSQL does not support streaming replication.", __func__, request->key);
					SET_UI64_RESULT(result, 3); //Not supported
					ret = SYSINFO_RET_OK;
					goto out;
				}
				else
				{
					db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", query);
				}
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL version.", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL version."));
				ret = SYSINFO_RET_FAIL;
				goto out;
			}
		}
		else if (0 == strcmp(request->key, "pgsql.replication.replay"))
		{
			if (ZBX_DB_OK == zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_ROLE_DBS))
			{
				pg_replication_role = get_int_one_result(request, result, 0, 0, pgsql_result);
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Replication role: %u", __func__, request->key, pg_replication_role);
				zbx_db_clean_result(&pgsql_result);
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL replication recovery role.", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL replication recovery role."));
				ret = SYSINFO_RET_FAIL;
				goto out;
			}

			// Only standby
			if (1 == pg_replication_role)
			{
				version = zbx_db_version(pgsql_conn);

				if (0 != version)
				{
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): PgSQL version: %lu", __func__, request->key, version);

					if (version < 90600)
					{
						zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): This version of PostgreSQL does not support streaming replication.", __func__, request->key);
						SET_UI64_RESULT(result, 2); //Not supported
						ret = SYSINFO_RET_OK;
						goto out;
					}
					else if (version >= 90600 && version < 100000)
					{
						db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_REPLAY_PAUSED_STATUS_V96_DBS);
					}
					else
					{
						db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_REPLAY_PAUSED_STATUS_V10_DBS);
					}
				}
				else
				{
					zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL version.", __func__, request->key);
					SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL version."));
					ret = SYSINFO_RET_FAIL;
					goto out;
				}
			}
			else
			{
				SET_UI64_RESULT(result, 2);
				ret = SYSINFO_RET_OK; //Not supported
				goto out;
			}
		}
		else if (0 == strcmp(request->key, "pgsql.replication.lag_byte"))
		{
			if (ZBX_DB_OK == zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_ROLE_DBS))
			{
				pg_replication_role = get_int_one_result(request, result, 0, 0, pgsql_result);
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Replication role: %u", __func__, request->key, pg_replication_role);
				zbx_db_clean_result(&pgsql_result);
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL replication recovery role.", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL replication recovery role."));
				ret = SYSINFO_RET_FAIL;
				goto out;
			}

			// Only standby
			if (1 == pg_replication_role)
			{
				version = zbx_db_version(pgsql_conn);

				if (0 != version)
				{
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): PgSQL version: %lu", __func__, request->key, version);

					if (version < 90600)
					{
						zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): This version of PostgreSQL does not support streaming replication.", __func__, request->key);
						SET_UI64_RESULT(result, 0);
						ret = SYSINFO_RET_OK;
						goto out;
					}
					else if (version >= 90600 && version < 100000)
					{
						db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_LAG_IN_BYTE_V9_DBS);
					}
					else
					{
						db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_LAG_IN_BYTE_V10_DBS);
					}
				}
				else
				{
					zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL version.", __func__, request->key);
					SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL version."));
					ret = SYSINFO_RET_FAIL;
					goto out;
				}
			}
			else
			{
				SET_UI64_RESULT(result, 0);
				ret = SYSINFO_RET_OK;
				goto out;
			}
		}
		else if (0 == strcmp(request->key, "pgsql.replication.lag_sec"))
		{
			if (ZBX_DB_OK == zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_ROLE_DBS))
			{
				pg_replication_role = get_int_one_result(request, result, 0, 0, pgsql_result);
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Replication role: %u", __func__, request->key, pg_replication_role);
				zbx_db_clean_result(&pgsql_result);
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL replication recovery role.", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL replication recovery role."));
				ret = SYSINFO_RET_FAIL;
				goto out;
			}

			// Only standby
			if (1 == pg_replication_role)
			{
				version = zbx_db_version(pgsql_conn);

				if (0 != version)
				{
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): PgSQL version: %lu", __func__, request->key, version);

					if (version < 90600)
					{
						zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): This version of PostgreSQL does not support streaming replication.", __func__, request->key);
						SET_UI64_RESULT(result, 0);
						ret = SYSINFO_RET_OK;
						goto out;
					}
					else if (version >= 90600 && version < 100000)
					{
						db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_LAG_IN_SEC_V9_DBS);
					}
					else
					{
						db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_LAG_IN_SEC_V10_DBS);
					}
				}
				else
				{
					zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL version.", __func__, request->key);
					SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL version."));
					ret = SYSINFO_RET_FAIL;
					goto out;
				}
			}
			else
			{
				SET_UI64_RESULT(result, 0);
				ret = SYSINFO_RET_OK;
				goto out;
			}
		}
		else if (0 == strcmp(request->key, "pgsql.replication.slots"))
		{
			if (ZBX_DB_OK == zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_ROLE_DBS))
			{
				pg_replication_role = get_int_one_result(request, result, 0, 0, pgsql_result);
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Replication role: %u", __func__, request->key, pg_replication_role);
				zbx_db_clean_result(&pgsql_result);
			}
			else
			{
				zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL replication recovery role.", __func__, request->key);
				SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL replication recovery role."));
				ret = SYSINFO_RET_FAIL;
				goto out;
			}

			// Only master
			if (0 == pg_replication_role)
			{
				version = zbx_db_version(pgsql_conn);

				if (0 != version)
				{
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): PgSQL version: %lu", __func__, request->key, version);

					if (version < 90500)
					{
						zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): This version of PostgreSQL does not support replication slots.", __func__, request->key);
						SET_TEXT_RESULT(result, zbx_strdup(NULL, "[]"));
						ret = SYSINFO_RET_OK;
						goto out;
					}
					else if (version >= 90500 && version < 90600)
					{
						db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_SLOTS_INFO_V95_DBS);
					}
					else if (version >= 90600 && version < 100000)
					{
						db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_SLOTS_INFO_V96_DBS);
					}
					else if (version >= 100000 && version < 130000)
					{
						db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_SLOTS_INFO_V10_DBS);
					}
					else
					{
						db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", PGSQL_REPLICATION_SLOTS_INFO_V13_DBS);
					}
				}
				else
				{
					zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Error get PgSQL version.", __func__, request->key);
					SET_MSG_RESULT(result, zbx_strdup(NULL, "Error get PgSQL version."));
					ret = SYSINFO_RET_FAIL;
					goto out;
				}
			}
			else
			{
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): This is a replica, getting information is available only on the master.", __func__, request->key);
				SET_TEXT_RESULT(result, zbx_strdup(NULL, "[]"));
				ret = SYSINFO_RET_OK;
				goto out;
			}
		}
		else
		{
			db_ret = zbx_db_query_select(pgsql_conn, &pgsql_result, "%s", query);
		}

		if (ZBX_DB_OK == db_ret)
		{
			ret = make_result(request, result, pgsql_result, result_type);
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
out:
	zbx_db_close_db(pgsql_conn);
	zbx_db_clean_connection(pgsql_conn);

	return ret;
}

#if !defined(_WINDOWS) && !defined(__MINGW32__)
static int	pgsql_get_result(AGENT_REQUEST *request, AGENT_RESULT *result)
#else
static int	pgsql_get_result(AGENT_REQUEST *request, AGENT_RESULT *result, HANDLE timeout_event)
#endif
{
	int ret = SYSINFO_RET_FAIL;

#if defined(_WINDOWS) && defined(__MINGW32__)
	/* 'timeout_event' argument is here to make the pgsql_get_result() prototype as required by */
	/* zbx_execute_dbmon_threaded_metric() on MS Windows */
	ZBX_UNUSED(timeout_event);
#endif

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);

	if (0 == strcmp(request->key, "pgsql.server.info"))
	{
		ret = pgsql_make_result(request, result, PGSQL_SERVER_INFO_DBS, ZBX_DB_RES_TYPE_ONEROW);
	}
	else if (0 == strcmp(request->key, "pgsql.db.info"))
	{
		ret = pgsql_make_result(request, result, PGSQL_DB_INFO_DBS, ZBX_DB_RES_TYPE_MULTIROW);
	}
	else if (0 == strcmp(request->key, "pgsql.db.locks"))
	{
		ret = pgsql_make_result(request, result, PGSQL_DB_LOCKS_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.db.stat.sum"))
	{
		ret = pgsql_make_result(request, result, PGSQL_DB_STAT_SUM_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.db.stat"))
	{
		ret = pgsql_make_result(request, result, PGSQL_DB_STAT_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.connections"))
	{
		ret = pgsql_make_result(request, result, PGSQL_CONNECTIONS_INFO_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.transactions"))
	{
		ret = pgsql_make_result(request, result, PGSQL_TRANSACTIONS_INFO_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.wal.stat"))
	{
		ret = pgsql_make_result(request, result, PGSQL_WAL_STAT_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.oldest.xid"))
	{
		ret = pgsql_make_result(request, result, PGSQL_OLDEST_XID_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.cache.hit"))
	{
		ret = pgsql_make_result(request, result, PGSQL_CACHE_HIT_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.bgwriter"))
	{
		ret = pgsql_make_result(request, result, PGSQL_BGWRITER_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.autovacuum.count"))
	{
		ret = pgsql_make_result(request, result, PGSQL_AUTOVACUUM_COUNT_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.archive.count"))
	{
		ret = pgsql_make_result(request, result, PGSQL_ARCHIVE_COUNT_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.archive.size"))
	{
		ret = pgsql_make_result(request, result, PGSQL_ARCHIVE_SIZE_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.prepared.transactions"))
	{
		ret = pgsql_make_result(request, result, PGSQL_PREPARED_COUNT_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.settings"))
	{
		ret = pgsql_make_result(request, result, PGSQL_NOT_DEFAULT_SETTINGS_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if ((0 == strcmp(request->key, "pgsql.replication.info")) || (0 == strcmp(request->key, "pgsql.replication.stat")))
	{
		ret = pgsql_make_result(request, result, PGSQL_REPLICATION_STAT_DBS, ZBX_DB_RES_TYPE_MULTIROW);
	}
	else if (0 == strcmp(request->key, "pgsql.replication.role"))
	{
		ret = pgsql_make_result(request, result, PGSQL_REPLICATION_ROLE_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.replication.count"))
	{
		ret = pgsql_make_result(request, result, PGSQL_REPLICATION_STANDBY_COUNT_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.replication.status"))
	{
		ret = pgsql_make_result(request, result, PGSQL_REPLICATION_STATUS_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.replication.replay"))
	{
		ret = pgsql_make_result(request, result, PGSQL_REPLICATION_REPLAY_PAUSED_STATUS_V10_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.replication.lag_byte"))
	{
		ret = pgsql_make_result(request, result, PGSQL_REPLICATION_LAG_IN_BYTE_V10_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.replication.lag_sec"))
	{
		ret = pgsql_make_result(request, result, PGSQL_REPLICATION_LAG_IN_SEC_V10_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else if (0 == strcmp(request->key, "pgsql.replication.slots"))
	{
		ret = pgsql_make_result(request, result, PGSQL_REPLICATION_SLOTS_INFO_V13_DBS, ZBX_DB_RES_TYPE_MULTIROW);
	}
	else if (0 == strcmp(request->key, "pgsql.backup.exclusive"))
	{
		ret = pgsql_make_result(request, result, PGSQL_IS_IN_BACKUP_DBS, ZBX_DB_RES_TYPE_NOJSON);
	}
	else
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Unknown request key"));
		ret = SYSINFO_RET_FAIL;
	}

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s): %s", __func__, request->key, zbx_sysinfo_ret_string(ret));

	return ret;
}

int	PGSQL_GET_RESULT(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	return zbx_execute_dbmon_threaded_metric(pgsql_get_result, request, result);
}
#endif
#endif