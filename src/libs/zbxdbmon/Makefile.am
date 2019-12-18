## Process this file with automake to produce Makefile.in

noinst_LIBRARIES = libzbxdbmon.a

if DBMON_MYSQL
DBMON_MYSQL_FILE = \
	dbmon_mysql.c
endif

if DBMON_POSTGRESQL
DBMON_PGSQL_FILE = \
	dbmon_pgsql.c
endif

if DBMON_ORACLE
DBMON_ORACLE_FILE = \
	dbmon_oracle.c
endif

libzbxdbmon_a_SOURCES = \
	$(DBMON_MYSQL_FILE) \
	$(DBMON_PGSQL_FILE) \
	$(DBMON_ORACLE_FILE) \
	dbmon.c
	
libzbxdbmon_a_CFLAGS = @DBMON_CFLAGS@
