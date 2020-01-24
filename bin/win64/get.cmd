@echo off

title Get zabbix-agent data...

set zbx_conn_string=-s 127.0.0.1 -p 11311 --tls-connect psk --tls-psk-identity default --tls-psk-file C:\DBS_Zabbix\Config\default.psk

set oracle_srv_name=dbs-db-dcb.vpn
set oracle_srv_ip=dbs-db-dcb.vpn
set oracle_instance=logsb
set oracle_dbname=LOGSB

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

echo === PgSQL ===================================================================

echo PostgreSQL version:
zabbix_get.exe %zbx_conn_string% -k pg.version["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"]
echo -----------------------------------------------------------------------------

echo PostgreSQL version full:
zabbix_get.exe %zbx_conn_string% -k pg.version.full["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"]
echo -----------------------------------------------------------------------------

echo PostgreSQL ping:
zabbix_get.exe %zbx_conn_string% -k pg.ping["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"]
echo -----------------------------------------------------------------------------

echo PostgreSQL database discovery:
zabbix_get.exe %zbx_conn_string% -k pg.db.discovery["host = localhost port = 5432 dbname = template1 user=postgres password=xxxxxx connect_timeout = 10"] | jq-win64.exe .

echo === Oracle (%oracle_srv_name%) =================================================

echo Orcale ping (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.ping[%oracle_srv_ip%,1521,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Orcale version (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.version[%oracle_srv_ip%,1521,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle instance info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.info[%oracle_srv_ip%,1521,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle instance parameter (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.parameter[%oracle_srv_ip%,1521,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle instance patch info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.patch_info[%oracle_srv_ip%,1521,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle instance resource (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.resource[%oracle_srv_ip%,1521,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle instance number of dbfiles (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.dbfiles[%oracle_srv_ip%,1521,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle instance resumable sessions info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.resumable[%oracle_srv_ip%,1521,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle instance bad processes info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.bad_processes[%oracle_srv_ip%,1521,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle instance FRA info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.fra[%oracle_srv_ip%,1521,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle instance redo log switch rate (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.redolog_switch_rate[%oracle_srv_ip%,1521,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle instance redo log size per 1 hour (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.instance.redolog_size_per_hour[%oracle_srv_ip%,1521,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle last archive log backup (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.backup.archivelog[%oracle_srv_ip%,1521,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle last full backup (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.backup.full[%oracle_srv_ip%,1521,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle last incremental backup (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.backup.incr[%oracle_srv_ip%,1521,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle total incremental file number (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.backup.incr_file_num[%oracle_srv_ip%,1521,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle last control file backup (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.backup.cf[%oracle_srv_ip%,1521,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle database discovery (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.db.discovery[%oracle_srv_ip%,1521,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle database info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.db.info[%oracle_srv_ip%,1521,%oracle_instance%,1,%oracle_dbname%] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle database incarnation (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.db.incarnation[%oracle_srv_ip%,1521,%oracle_instance%,1,%oracle_dbname%] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle database size (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.db.size[%oracle_srv_ip%,1521,%oracle_instance%,1,%oracle_dbname%] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle standby discovery (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.standby.discovery[%oracle_srv_ip%,1521,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle get standby lag (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.standby.lag[%oracle_srv_ip%,1521,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle get standby MRP status (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.standby.mrp_status[%oracle_srv_ip%,1521,%oracle_instance%,1]
echo -----------------------------------------------------------------------------

echo Oracle archlogdest discovery (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.archlogdest.discovery[%oracle_srv_ip%,1521,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle archlogdest info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.archlogdest.info[%oracle_srv_ip%,1521,%oracle_instance%,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle permanent tablespace info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.tablespace.info[%oracle_srv_ip%,1521,%oracle_instance%,1,0] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle temporary tablespace info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.tablespace.info[%oracle_srv_ip%,1521,%oracle_instance%,1,1] | jq-win64.exe .
echo -----------------------------------------------------------------------------

echo Oracle undo tablespace info (%oracle_srv_name%):
zabbix_get.exe %zbx_conn_string% -k oracle.tablespace.info[%oracle_srv_ip%,1521,%oracle_instance%,1,2] | jq-win64.exe .

echo =============================================================================
