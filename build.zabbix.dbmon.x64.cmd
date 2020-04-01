@echo off

title Build zabbix-agent-dbmon...

set ZBX_ARCH=AMD64
set VS_ARCH=x64
set BASE_DIR=D:\Neo\Project\zabbix_dbmon_github
set PCRE_PATH=%BASE_DIR%\vs2017\pcre
set OPENSSL_PATH=%BASE_DIR%\vs2017\openssl
set PTHREADS4W_PATH=%BASE_DIR%\vs2017\pthreads4w
set LIBCONIG_PATH=%BASE_DIR%\vs2017\libconfig
set MYSQL_PATH=%BASE_DIR%\vs2017\mariadb
set PGSQL_PATH=%BASE_DIR%\vs2017\postgresql_12
set ORACLE_PATH=%BASE_DIR%\vs2017\oracle_18

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
  echo Build Zabbix Agent DBMON %ZBX_ARCH%...
  nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" PTHREADS4WINCDIR="%PTHREADS4W_PATH%\include" PTHREADS4WLIBDIR="%PTHREADS4W_PATH%\lib\%VS_ARCH%" LIBCONIGINCDIR="%LIBCONIG_PATH%\include" LIBCONIGLIBDIR="%LIBCONIG_PATH%\lib\%VS_ARCH%" DBMON=yes DBMON_ORACLE=yes ORACLEINCDIR="%ORACLE_PATH%\include" ORACLELIBDIR="%ORACLE_PATH%\lib" DBMON_PGSQL=yes PGSQLINCDIR="%PGSQL_PATH%\include" PGSQLLIBDIR="%PGSQL_PATH%\lib\%VS_ARCH%" DBMON_MYSQL=yes MYSQLINCDIR="%MYSQL_PATH%\include" MYSQLLIBDIR="%MYSQL_PATH%\lib\%VS_ARCH%" /f Makefile_agent
  rem nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" /f Makefile_agent
  rem echo Build Zabbix Sender %ZBX_ARCH%...
  rem nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" /f Makefile_sender
  rem echo Build Zabbix Get %ZBX_ARCH%...
  rem nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" /f Makefile_get
)

echo Done, press Enter to exit.
pause >nul
