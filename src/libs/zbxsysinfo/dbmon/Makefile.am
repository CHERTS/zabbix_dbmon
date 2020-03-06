## Process this file with automake to produce Makefile.in

noinst_LIBRARIES = libzbxdbmoninfo.a

if DBMON_MYSQL
DBMON_MYSQL_INFO_FILE = \
	mysql_info.c mysql_info.h
endif

if DBMON_POSTGRESQL
DBMON_PGSQL_INFO_FILE = \
	pgsql_info.c pgsql_info.h
endif

if DBMON_ORACLE
DBMON_ORACLE_INFO_FILE = \
	oracle_info.c oracle_info.h
endif

libzbxdbmoninfo_a_SOURCES = \
	$(DBMON_MYSQL_INFO_FILE) \
	$(DBMON_PGSQL_INFO_FILE) \
	$(DBMON_ORACLE_INFO_FILE) \
	dbmon_common.c dbmon_common.h \
	dbmon_config.c dbmon_config.h \
	dbmon_params.c dbmon_params.h
	
libzbxdbmoninfo_a_CFLAGS = \
	-I@top_srcdir@/src/zabbix_agent/ \
	@DBMON_CFLAGS@
