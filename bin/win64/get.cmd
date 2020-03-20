@echo off

title Get zabbix-agent data...

set zbx_conn_string=-s 127.0.0.1 -p 11311 --tls-connect psk --tls-psk-identity default --tls-psk-file C:\DBS_Zabbix\Config\default.psk

set oracle_srv_name=127.0.0.1
set oracle_conn_string=127.0.0.1:1521/orcl
set oracle_instance=orcl
set oracle_dbname=ORCL

echo =============================================================================

echo Agent version:
zabbix_get.exe %zbx_conn_string% -k agent.version

echo === MySQL ===================================================================

echo MySQL version:
zabbix_get.exe %zbx_conn_string% -k mysql.version[127.0.0.1,3306]
echo -----------------------------------------------------------------------------

echo MySQL version full:
zabbix_get.exe %zbx_conn_string% -k mysql.version.full[127.0.0.1,3306]
echo -----------------------------------------------------------------------------

echo MySQL ping:
zabbix_get.exe %zbx_conn_string% -k mysql.ping[127.0.0.1,3306]
echo -----------------------------------------------------------------------------

echo MySQL server info:
zabbix_get.exe %zbx_conn_string% -k mysql.server.info[127.0.0.1,3306] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo MySQL global status:
zabbix_get.exe %zbx_conn_string% -k mysql.global.status[127.0.0.1,3306] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo MySQL global variables:
zabbix_get.exe %zbx_conn_string% -k mysql.global.variables[127.0.0.1,3306] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo MySQL database discovery:
zabbix_get.exe %zbx_conn_string% -k mysql.db.discovery[127.0.0.1,3306] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo MySQL database info:
zabbix_get.exe %zbx_conn_string% -k mysql.db.info[127.0.0.1,3306] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo MySQL error.log discovery:
zabbix_get.exe %zbx_conn_string% -k mysql.errorlog.discovery[127.0.0.1,3306] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo MySQL TOP10 by size:
zabbix_get.exe %zbx_conn_string% -k mysql.top10_table_by_size[127.0.0.1,3306] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo MySQL TOP10 by rows:
zabbix_get.exe %zbx_conn_string% -k mysql.top10_table_by_rows[127.0.0.1,3306] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo MySQL slave status:
zabbix_get.exe %zbx_conn_string% -k mysql.slave.status[127.0.0.1,3306] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo MySQL custom query (name: mysqlver):
zabbix_get.exe %zbx_conn_string% -k mysql.query.nojson[127.0.0.1,3306,mysqlver]
echo -----------------------------------------------------------------------------

echo MySQL custom query (name: mysqlinfo):
zabbix_get.exe %zbx_conn_string% -k mysql.query.onerow[127.0.0.1,3306,mysqlinfo] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo MySQL custom query (name: mysqldbdiscovery):
zabbix_get.exe %zbx_conn_string% -k mysql.query.discovery[127.0.0.1,3306,mysqldbdiscovery] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo MySQL custom query (name: mysqldbinfo):
zabbix_get.exe %zbx_conn_string% -k mysql.query.multirow[127.0.0.1,3306,mysqldbinfo] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo MySQL custom query (name: mysqlinnodbenginestatus):
zabbix_get.exe %zbx_conn_string% -k mysql.query.nojson[127.0.0.1,3306,mysqlinnodbenginestatus]

echo === PgSQL ===================================================================

echo PostgreSQL ping:
zabbix_get.exe %zbx_conn_string% -k pgsql.ping["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"]
echo -----------------------------------------------------------------------------

echo PostgreSQL version:
zabbix_get.exe %zbx_conn_string% -k pgsql.version["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"]
echo -----------------------------------------------------------------------------

echo PostgreSQL version full:
zabbix_get.exe %zbx_conn_string% -k pgsql.version.full["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"]
echo -----------------------------------------------------------------------------

echo PostgreSQL database discovery:
zabbix_get.exe %zbx_conn_string% -k pgsql.db.discovery["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo PostgreSQL database info:
zabbix_get.exe %zbx_conn_string% -k pgsql.db.info["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo PostgreSQL database locks:
zabbix_get.exe %zbx_conn_string% -k pgsql.db.locks["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo PostgreSQL database stat sum:
zabbix_get.exe %zbx_conn_string% -k pgsql.db.stat.sum["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo PostgreSQL database stat:
zabbix_get.exe %zbx_conn_string% -k pgsql.db.stat["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo PostgreSQL connections:
zabbix_get.exe %zbx_conn_string% -k pgsql.connections["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo PostgreSQL transactions:
zabbix_get.exe %zbx_conn_string% -k pgsql.transactions["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo PostgreSQL WAL stat:
zabbix_get.exe %zbx_conn_string% -k pgsql.wal.stat["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo PostgreSQL age of oldest xid:
zabbix_get.exe %zbx_conn_string% -k pgsql.oldest.xid["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"]
echo -----------------------------------------------------------------------------

echo PostgreSQL cache hit:
zabbix_get.exe %zbx_conn_string% -k pgsql.cache.hit["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"]
echo -----------------------------------------------------------------------------

echo PostgreSQL bgwriter:
zabbix_get.exe %zbx_conn_string% -k pgsql.bgwriter["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo PostgreSQL autovacuum count:
zabbix_get.exe %zbx_conn_string% -k pgsql.autovacuum.count["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"]
echo -----------------------------------------------------------------------------

echo PostgreSQL archive count:
zabbix_get.exe %zbx_conn_string% -k pgsql.archive.count["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo PostgreSQL archive size:
zabbix_get.exe %zbx_conn_string% -k pgsql.archive.size["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"] | jq-win64.exe .

echo === Oracle (%oracle_srv_name%) =================================================

echo Orcale ping (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.ping[%oracle_conn_string%,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Orcale version (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.version[%oracle_conn_string%,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle instance info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.info[%oracle_conn_string%,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle instance parameter (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.parameter[%oracle_conn_string%,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle instance patch info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.patch_info[%oracle_conn_string%,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle instance resource (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.resource[%oracle_conn_string%,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle instance number of dbfiles (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.dbfiles[%oracle_conn_string%,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle instance resumable sessions info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.resumable[%oracle_conn_string%,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle instance bad processes info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.bad_processes[%oracle_conn_string%,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle instance FRA info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.fra[%oracle_conn_string%,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle instance redo log switch rate (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.redolog_switch_rate[%oracle_conn_string%,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle instance redo log size per 1 hour (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.redolog_size_per_hour[%oracle_conn_string%,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle last archive log backup (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.backup.archivelog[%oracle_conn_string%,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle last full backup (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.backup.full[%oracle_conn_string%,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle last incremental backup (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.backup.incr[%oracle_conn_string%,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle total incremental file number (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.backup.incr_file_num[%oracle_conn_string%,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle last control file backup (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.backup.cf[%oracle_conn_string%,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle database discovery (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.db.discovery[%oracle_conn_string%,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle database info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.db.info[%oracle_conn_string%,%oracle_instance%,1,%oracle_dbname%] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle database incarnation (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.db.incarnation[%oracle_conn_string%,%oracle_instance%,1,%oracle_dbname%] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle database size (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.db.size[%oracle_conn_string%,%oracle_instance%,1,%oracle_dbname%] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle standby discovery (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.standby.discovery[%oracle_conn_string%,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle get standby lag (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.standby.lag[%oracle_conn_string%,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle get standby MRP status (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.standby.mrp_status[%oracle_conn_string%,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle archlogdest discovery (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.archlogdest.discovery[%oracle_conn_string%,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle archlogdest info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.archlogdest.info[%oracle_conn_string%,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle permanent tablespace info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.tablespace.info[%oracle_conn_string%,%oracle_instance%,1,0] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle temporary tablespace info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.tablespace.info[%oracle_conn_string%,%oracle_instance%,1,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle undo tablespace info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.tablespace.info[%oracle_conn_string%,%oracle_instance%,1,2] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle alert log discovery (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.alertlog.discovery[%oracle_conn_string%,%oracle_instance%,1] | jq-win64.exe .

echo =============================================================================
