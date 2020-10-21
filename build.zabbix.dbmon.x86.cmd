@echo off

title Build zabbix-agent-dbmon...

set ZBX_ARCH=i386
set VS_ARCH=x86
set BASE_DIR=D:\Neo\Project\zabbix\zabbix_dbmon_github
set PCRE_PATH=%BASE_DIR%\vs2017\pcre
set OPENSSL_PATH=%BASE_DIR%\vs2017\openssl
set PTHREADS4W_PATH=%BASE_DIR%\vs2017\pthreads4w
set LIBCONIG_PATH=%BASE_DIR%\vs2017\libconfig
set MYSQL_PATH=%BASE_DIR%\vs2017\mariadb
set PGSQL_PATH=%BASE_DIR%\vs2017\postgresql_12
set ORACLE_PATH=%BASE_DIR%\vs2017\oracle_18

set ZBX_AGENTD_BIN=%BASE_DIR%\bin\win64\zabbix_agentd.exe
set ZBX_SENDER_BIN=%BASE_DIR%\bin\win64\zabbix_sender.exe
set ZBX_GET_BIN=%BASE_DIR%\bin\win64\zabbix_get.exe

set cert_timestamp_server=http://time.certum.pl
set cert_thumbprint=feb73ceedc91104ba207ebb4eb09a98413f4451f

title Build zabbix-agent-dbmon %ZBX_ARCH%...

rem https://support.zabbix.com/browse/ZBXNEXT-3047
rem -- Visual Studio 2015 Pro --
rem call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %VS_ARCH% >nul 2>&1
rem -- Visual Studio 2017 Community --
echo Set Visual Studio 2017 enviroment...
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" %VS_ARCH% >nul 2>&1

if not exist "%PCRE_PATH%\lib\%VS_ARCH%" (
  echo ERROR: PCRE %VS_ARCH% not found.
  echo Press Enter to exit.
  pause >nul
  exit 1
)

if not exist "%OPENSSL_PATH%\lib\%VS_ARCH%" (
  echo ERROR: OpenSSL %VS_ARCH% not found.
  echo Press Enter to exit.
  pause >nul
  exit 1
)

if not exist "%PTHREADS4W_PATH%\lib\%VS_ARCH%" (
  echo ERROR: PTHREADS4W %VS_ARCH% not found.
  echo Press Enter to exit.
  pause >nul
  exit 1
)

if not exist "%LIBCONIG_PATH%\lib\%VS_ARCH%" (
  echo ERROR: LIBCONIG %VS_ARCH% not found.
  echo Press Enter to exit.
  pause >nul
  exit 1
)

if exist "build" (
  cd build\win32\project
  copy ..\include\config.h ..\..\..\include\ >nul 2>&1
  copy ..\..\..\include\version.h ..\..\..\include\version.h.orig >nul 2>&1
  del /s /q /f ..\..\..\include\version.h >nul 2>&1
  copy ..\..\..\include\version.h.win ..\..\..\include\version.h >nul 2>&1

  echo ------------ Build Zabbix Agent DBMON %ZBX_ARCH% ----------------------

  echo ------------ Build only DBMON code ------------
  rem set DBMON_OPTS="/D HAVE_MYSQL /D HAVE_POSTGRESQL /D HAVE_ORACLE"
  del /s /q /f ..\..\..\*.o >nul 2>&1
  del /s /q /f ..\..\..\bin\*.pdb >nul 2>&1
  del /s /q /f ..\..\..\bin\*.exe.pdb >nul 2>&1
  del /s /q /f ..\..\..\build\*.pdb >nul 2>&1
  del /s /q /f ..\..\..\build\*.exe.pdb >nul 2>&1
  del /s /q /f ..\..\..\build\*.exe.idb >nul 2>&1
  del /s /q /f ..\..\..\build\win32\include\messages.h >nul 2>&1
  del /s /q /f ..\..\..\build\win32\include\messages.rc >nul 2>&1
  del /s /q /f ..\..\..\build\win32\include\MSG00001.bin >nul 2>&1
  del /s /q /f ..\..\..\build\win32\project\messages.h >nul 2>&1
  del /s /q /f ..\..\..\build\win32\project\messages.rc >nul 2>&1
  del /s /q /f ..\..\..\build\win32\project\MSG00001.bin >nul 2>&1
  del /s /q /f ..\..\..\build\win32\project\*.res >nul 2>&1
  cl.exe ..\..\..\src\libs\zbxdbmon\dbmon.c /Fo"..\..\..\src\libs\zbxdbmon\dbmon.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /I "%MYSQL_PATH%\include" /I "%ORACLE_PATH%\include" /I "%PGSQL_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON /D HAVE_MYSQL /D HAVE_ORACLE /D HAVE_POSTGRESQL
  cl.exe ..\..\..\src\libs\zbxdbmon\dbmon_mysql.c /Fo"..\..\..\src\libs\zbxdbmon\dbmon_mysql.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /I "%MYSQL_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON /D HAVE_MYSQL
  cl.exe ..\..\..\src\libs\zbxdbmon\dbmon_pgsql.c /Fo"..\..\..\src\libs\zbxdbmon\dbmon_pgsql.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /I "%PGSQL_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON /D HAVE_POSTGRESQL
  cl.exe ..\..\..\src\libs\zbxdbmon\dbmon_oracle.c /Fo"..\..\..\src\libs\zbxdbmon\dbmon_oracle.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /I "%ORACLE_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON /D HAVE_ORACLE
  cl.exe ..\..\..\src\libs\zbxdbmon\strptime.c /Fo"..\..\..\src\libs\zbxdbmon\strptime.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON
  cl.exe ..\..\..\src\libs\zbxsysinfo\dbmon\dbmon_common.c /Fo"..\..\..\src\libs\zbxsysinfo\dbmon\dbmon_common.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /I "%MYSQL_PATH%\include" /I "%ORACLE_PATH%\include" /I "%PGSQL_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON /D HAVE_MYSQL /D HAVE_ORACLE /D HAVE_POSTGRESQL
  cl.exe ..\..\..\src\libs\zbxsysinfo\dbmon\dbmon_config.c /Fo"..\..\..\src\libs\zbxsysinfo\dbmon\dbmon_config.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON 
  cl.exe ..\..\..\src\libs\zbxsysinfo\dbmon\dbmon_params.c /Fo"..\..\..\src\libs\zbxsysinfo\dbmon\dbmon_params.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON
  cl.exe ..\..\..\src\libs\zbxsysinfo\dbmon\mysql_info.c /Fo"..\..\..\src\libs\zbxsysinfo\dbmon\mysql_info.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /I "%MYSQL_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON /D HAVE_MYSQL
  cl.exe ..\..\..\src\libs\zbxsysinfo\dbmon\pgsql_info.c /Fo"..\..\..\src\libs\zbxsysinfo\dbmon\pgsql_info.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /I "%PGSQL_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON /D HAVE_POSTGRESQL
  cl.exe ..\..\..\src\libs\zbxsysinfo\dbmon\oracle_info.c /Fo"..\..\..\src\libs\zbxsysinfo\dbmon\oracle_info.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /I "%ORACLE_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON /D HAVE_ORACLE

  echo ------------ All database support ------------
  nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" PTHREADS4WINCDIR="%PTHREADS4W_PATH%\include" PTHREADS4WLIBDIR="%PTHREADS4W_PATH%\lib\%VS_ARCH%" LIBCONIGINCDIR="%LIBCONIG_PATH%\include" LIBCONIGLIBDIR="%LIBCONIG_PATH%\lib\%VS_ARCH%" DBMON=yes DBMON_ORACLE=yes ORACLEINCDIR="%ORACLE_PATH%\include" ORACLELIBDIR="%ORACLE_PATH%\lib\%VS_ARCH%" DBMON_PGSQL=yes PGSQLINCDIR="%PGSQL_PATH%\include" PGSQLLIBDIR="%PGSQL_PATH%\lib\%VS_ARCH%" DBMON_MYSQL=yes MYSQLINCDIR="%MYSQL_PATH%\include" MYSQLLIBDIR="%MYSQL_PATH%\lib\%VS_ARCH%" /f Makefile_agent
  rem echo ------------ MySQL + Oracle database support ------------
  rem nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" PTHREADS4WINCDIR="%PTHREADS4W_PATH%\include" PTHREADS4WLIBDIR="%PTHREADS4W_PATH%\lib\%VS_ARCH%" LIBCONIGINCDIR="%LIBCONIG_PATH%\include" LIBCONIGLIBDIR="%LIBCONIG_PATH%\lib\%VS_ARCH%" DBMON=yes DBMON_ORACLE=yes ORACLEINCDIR="%ORACLE_PATH%\include" ORACLELIBDIR="%ORACLE_PATH%\lib\%VS_ARCH%" DBMON_MYSQL=yes MYSQLINCDIR="%MYSQL_PATH%\include" MYSQLLIBDIR="%MYSQL_PATH%\lib\%VS_ARCH%" /f Makefile_agent
  rem echo ------------ Only MySQL support ------------
  rem nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" PTHREADS4WINCDIR="%PTHREADS4W_PATH%\include" PTHREADS4WLIBDIR="%PTHREADS4W_PATH%\lib\%VS_ARCH%" LIBCONIGINCDIR="%LIBCONIG_PATH%\include" LIBCONIGLIBDIR="%LIBCONIG_PATH%\lib\%VS_ARCH%" DBMON=yes DBMON_MYSQL=yes MYSQLINCDIR="%MYSQL_PATH%\include" MYSQLLIBDIR="%MYSQL_PATH%\lib\%VS_ARCH%" /f Makefile_agent
  rem echo ------------ Only Oracle support ------------
  rem nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" PTHREADS4WINCDIR="%PTHREADS4W_PATH%\include" PTHREADS4WLIBDIR="%PTHREADS4W_PATH%\lib\%VS_ARCH%" LIBCONIGINCDIR="%LIBCONIG_PATH%\include" LIBCONIGLIBDIR="%LIBCONIG_PATH%\lib\%VS_ARCH%" DBMON=yes DBMON_ORACLE=yes ORACLEINCDIR="%ORACLE_PATH%\include" ORACLELIBDIR="%ORACLE_PATH%\lib\%VS_ARCH%" /f Makefile_agent
  rem echo ------------ No database support ------------
  rem nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" /f Makefile_agent

  if exist "%ZBX_AGENTD_BIN%" (
    signtool_x86.exe sign /sha1 "%cert_thumbprint%" /tr "%cert_timestamp_server%" /fd sha256 /v "%ZBX_AGENTD_BIN%"
  ) else (
    echo "ERROR! %ZBX_AGENTD_BIN% not found."
  )

  rem echo Build Zabbix Sender %ZBX_ARCH%...
  nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" /f Makefile_sender

  if exist "%ZBX_SENDER_BIN%" (
    signtool_x86.exe sign /sha1 "%cert_thumbprint%" /tr "%cert_timestamp_server%" /fd sha256 /v "%ZBX_SENDER_BIN%"
  ) else (
    echo "ERROR! %ZBX_SENDER_BIN% not found."
  )

  echo Build Zabbix Get %ZBX_ARCH%...
  nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" /f Makefile_get

  if exist "%ZBX_GET_BIN%" (
    signtool_x86.exe sign /sha1 "%cert_thumbprint%" /tr "%cert_timestamp_server%" /fd sha256 /v "%ZBX_GET_BIN%"
  ) else (
    echo "ERROR! %ZBX_GET_BIN% not found."
  )

  del /s /q /f ..\..\..\include\version.h >nul 2>&1
  copy ..\..\..\include\version.h.orig ..\..\..\include\version.h >nul 2>&1
  del /s /q /f ..\..\..\include\version.h.orig >nul 2>&1
)

echo Done, press Enter to exit.
pause >nul
