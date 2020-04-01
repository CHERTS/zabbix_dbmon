!IF "$(DBMON)" != ""

CFLAGS = $(CFLAGS) /DHAVE_DBMON

!	IF "$(PTHREADS4WINCDIR)" == ""
!		ERROR Please specify directory containing PTHREADS4W header files (e. g. PTHREADS4WINCDIR=\somepath\include).
!	ENDIF
!	IF ("$(PTHREADS4WLIBDIR)" == "")
!		ERROR Please specify PTHREADS4WLIBDIR (e. g. PTHREADS4WLIBDIR=\somepath\lib).
!	ENDIF
!	IF EXISTS("$(PTHREADS4WINCDIR)\pthread.h")
INCS =	$(INCS) /I "$(PTHREADS4WINCDIR)"
!	ELSE
!		ERROR PTHREADS4WINCDIR "$(PTHREADS4WINCDIR)" cannot find "$(PTHREADS4WINCDIR)\pthread.h"
!	ENDIF
PTHREADS4WLIB = $(PTHREADS4WLIBDIR)\libpthreadVC3.lib
!	IF EXISTS("$(PTHREADS4WLIB)")
LIBS = $(LIBS) "$(PTHREADS4WLIB)"
!	ELSE
!		ERROR PTHREADS4WLIBDIR "$(PTHREADS4WLIBDIR)" cannot find PTHREADS4WLIB "$(PTHREADS4WLIB)"
!	ENDIF

!	IF "$(LIBCONIGINCDIR)" == ""
!		ERROR Please specify directory containing LIBCONFIG header files (e. g. LIBCONIGINCDIR=\somepath\include).
!	ENDIF
!	IF ("$(LIBCONIGLIBDIR)" == "")
!		ERROR Please specify LIBCONIGLIBDIR (e. g. LIBCONIGLIBDIR=\somepath\lib).
!	ENDIF
!	IF EXISTS("$(LIBCONIGINCDIR)\libconfig.h")
INCS =	$(INCS) /I "$(LIBCONIGINCDIR)"
!	ELSE
!		ERROR LIBCONIGINCDIR "$(LIBCONIGINCDIR)" cannot find "$(LIBCONIGINCDIR)\libconfig.h"
!	ENDIF
LIBCONIGLIB = $(LIBCONIGLIBDIR)\libconfig.lib
!	IF EXISTS("$(LIBCONIGLIB)")
LIBS = $(LIBS) "$(LIBCONIGLIB)"
!	ELSE
!		ERROR LIBCONIGLIBDIR "$(LIBCONIGLIBDIR)" cannot find LIBCONIGLIB "$(LIBCONIGLIB)"
!	ENDIF

OBJS =	$(OBJS) ..\..\..\src\libs\zbxdbmon\dbmon.o ..\..\..\src\libs\zbxsysinfo\dbmon\dbmon_common.o ..\..\..\src\libs\zbxsysinfo\dbmon\dbmon_params.o ..\..\..\src\libs\zbxsysinfo\dbmon\dbmon_config.o

!	IF "$(DBMON_MYSQL)" == "yes"
!		IF "$(MYSQLINCDIR)" == ""
!			ERROR Please specify directory containing MYSQL header files (e. g. MYSQLINCDIR=\somepath\include).
!		ENDIF
!		IF ("$(MYSQLLIBDIR)" == "")
!			ERROR Please specify MYSQLLIBDIR (e. g. MYSQLLIBDIR=\somepath\lib).
!		ENDIF
!		IF EXISTS("$(MYSQLINCDIR)\mysql.h")
CFLAGS =	$(CFLAGS) /DHAVE_MYSQL
INCS =	$(INCS) /I "$(MYSQLINCDIR)"
!		ELSE
!			ERROR MYSQLINCDIR "$(MYSQLINCDIR)" cannot find "$(MYSQLINCDIR)\mysql.h"
!		ENDIF
MYSQLLIB = $(MYSQLLIBDIR)\libmariadb.lib
!		IF EXISTS("$(MYSQLLIB)")
LIBS = $(LIBS) "$(MYSQLLIB)"
OBJS =	$(OBJS) ..\..\..\src\libs\zbxdbmon\dbmon_mysql.o ..\..\..\src\libs\zbxdbmon\strptime.o ..\..\..\src\libs\zbxsysinfo\dbmon\mysql_info.o
!		ELSE
!			ERROR MYSQLLIBDIR "$(MYSQLLIBDIR)" cannot find MYSQLLIB "$(MYSQLLIB)"
!		ENDIF
!	ENDIF

!	IF "$(DBMON_PGSQL)" == "yes"
!		IF "$(PGSQLINCDIR)" == ""
!			ERROR Please specify directory containing PGSQL header files (e. g. PGSQLINCDIR=\somepath\include).
!		ENDIF
!		IF ("$(PGSQLLIBDIR)" == "")
!			ERROR Please specify PGSQLLIBDIR (e. g. PGSQLLIBDIR=\somepath\lib).
!		ENDIF
!		IF EXISTS("$(PGSQLINCDIR)\libpq-fe.h")
CFLAGS =	$(CFLAGS) /DHAVE_POSTGRESQL
INCS =	$(INCS) /I "$(PGSQLINCDIR)"
!		ELSE
!			ERROR PGSQLINCDIR "$(PGSQLINCDIR)" cannot find "$(PGSQLINCDIR)\libpq-fe.h"
!		ENDIF
PGSQLLIB = $(PGSQLLIBDIR)\libpq.lib
!		IF EXISTS("$(PGSQLLIB)")
LIBS = $(LIBS) "$(PGSQLLIB)"
OBJS =	$(OBJS) ..\..\..\src\libs\zbxdbmon\dbmon_pgsql.o ..\..\..\src\libs\zbxsysinfo\dbmon\pgsql_info.o
!		ELSE
!			ERROR PGSQLLIBDIR "$(PGSQLLIBDIR)" cannot find PGSQLLIB "$(PGSQLLIB)"
!		ENDIF
!	ENDIF

!	IF "$(DBMON_ORACLE)" == "yes"
!		IF "$(ORACLEINCDIR)" == ""
!			ERROR Please specify directory containing ORACLE header files (e. g. ORACLEINCDIR=\somepath\include).
!		ENDIF
!		IF ("$(ORACLELIBDIR)" == "")
!			ERROR Please specify ORACLELIBDIR (e. g. ORACLELIBDIR=\somepath\lib).
!		ENDIF
!		IF EXISTS("$(ORACLEINCDIR)\oci")
CFLAGS =	$(CFLAGS) /DHAVE_ORACLE
INCS =	$(INCS) /I "$(ORACLEINCDIR)"
!		ELSE
!			ERROR ORACLEINCDIR "$(ORACLEINCDIR)" cannot find "$(ORACLEINCDIR)\oci.h"
!		ENDIF
ORACLELIB = $(ORACLELIBDIR)\oci.lib
ORACLELIB2 = $(ORACLELIBDIR)\oraocci18.lib
!		IF EXISTS("$(ORACLELIB)")
LIBS = $(LIBS) "$(ORACLELIB)"
!		ELSE
!			ERROR ORACLELIBDIR "$(ORACLELIBDIR)" cannot find ORACLELIB "$(ORACLELIB)"
!		ENDIF
!		IF EXISTS("$(ORACLELIB2)")
LIBS = $(LIBS) "$(ORACLELIB2)"
!		ELSE
!			ERROR ORACLELIBDIR "$(ORACLELIBDIR)" cannot find ORACLELIB2 "$(ORACLELIB2)"
!		ENDIF
OBJS =	$(OBJS) ..\..\..\src\libs\zbxdbmon\dbmon_oracle.o ..\..\..\src\libs\zbxsysinfo\dbmon\oracle_info.o
!	ENDIF

!ENDIF
