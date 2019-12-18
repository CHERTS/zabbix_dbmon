## Process this file with automake to produce Makefile.in

noinst_LIBRARIES = libzbxdbmoninfo.a

if DBMON_MYSQL
DBMON_MYSQL_INFO_FILE = \
	mysql_info.c
endif

if DBMON_POSTGRESQL
DBMON_PGSQL_INFO_FILE = \
	pgsql_info.c
endif

if DBMON_ORACLE
DBMON_ORACLE_INFO_FILE = \
	oracle_info.c
endif

libzbxdbmoninfo_a_SOURCES = \
	$(DBMON_MYSQL_INFO_FILE) \
	$(DBMON_PGSQL_INFO_FILE) \
	$(DBMON_ORACLE_INFO_FILE) \
	dbmon_common.c
	
libzbxdbmoninfo_a_CFLAGS = \
	-I@top_srcdir@/src/zabbix_agent/ \
	@DBMON_CFLAGS@
