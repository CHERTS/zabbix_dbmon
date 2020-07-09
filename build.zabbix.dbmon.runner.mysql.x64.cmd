@echo off

title Build zabbix-agent-dbmon...

set ZBX_ARCH=AMD64
set VS_ARCH=x64
set BASE_DIR=%cd%
set SDK_DIR=D:\Neo\Project\zabbix\actions-runner\_dll
set PCRE_PATH=%SDK_DIR%\pcre
set OPENSSL_PATH=%SDK_DIR%\openssl
set PTHREADS4W_PATH=%SDK_DIR%\pthreads4w
set LIBCONIG_PATH=%SDK_DIR%\libconfig
set MYSQL_PATH=%SDK_DIR%\mariadb

title Build zabbix-agent-dbmon %ZBX_ARCH%...

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

  echo ------------ Build Zabbix Agent DBMON %ZBX_ARCH% ----------------------

  echo ------------ Build only DBMON code ------------
  del /s /q /f ..\..\..\*.o
  del /s /q /f ..\..\..\bin\*.pdb
  del /s /q /f ..\..\..\bin\*.exe.pdb
  del /s /q /f ..\..\..\build\*.pdb
  del /s /q /f ..\..\..\build\*.exe.pdb
  del /s /q /f ..\..\..\build\*.exe.idb
  del /s /q /f ..\..\..\build\win32\include\messages.h
  del /s /q /f ..\..\..\build\win32\include\messages.rc
  del /s /q /f ..\..\..\build\win32\include\MSG00001.bin
  del /s /q /f ..\..\..\build\win32\project\messages.h
  del /s /q /f ..\..\..\build\win32\project\messages.rc
  del /s /q /f ..\..\..\build\win32\project\MSG00001.bin
  del /s /q /f ..\..\..\build\win32\project\*.res
  cl.exe ..\..\..\src\libs\zbxdbmon\dbmon.c /Fo"..\..\..\src\libs\zbxdbmon\dbmon.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /I "%MYSQL_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON /D HAVE_MYSQL
  cl.exe ..\..\..\src\libs\zbxdbmon\dbmon_mysql.c /Fo"..\..\..\src\libs\zbxdbmon\dbmon_mysql.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /I "%MYSQL_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON /D HAVE_MYSQL
  cl.exe ..\..\..\src\libs\zbxdbmon\dbmon_pgsql.c /Fo"..\..\..\src\libs\zbxdbmon\dbmon_pgsql.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK
  cl.exe ..\..\..\src\libs\zbxdbmon\dbmon_oracle.c /Fo"..\..\..\src\libs\zbxdbmon\dbmon_oracle.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK
  cl.exe ..\..\..\src\libs\zbxdbmon\strptime.c /Fo"..\..\..\src\libs\zbxdbmon\strptime.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON
  cl.exe ..\..\..\src\libs\zbxsysinfo\dbmon\dbmon_common.c /Fo"..\..\..\src\libs\zbxsysinfo\dbmon\dbmon_common.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /I "%MYSQL_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON /D HAVE_MYSQL
  cl.exe ..\..\..\src\libs\zbxsysinfo\dbmon\dbmon_config.c /Fo"..\..\..\src\libs\zbxsysinfo\dbmon\dbmon_config.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON 
  cl.exe ..\..\..\src\libs\zbxsysinfo\dbmon\dbmon_params.c /Fo"..\..\..\src\libs\zbxsysinfo\dbmon\dbmon_params.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON
  cl.exe ..\..\..\src\libs\zbxsysinfo\dbmon\mysql_info.c /Fo"..\..\..\src\libs\zbxsysinfo\dbmon\mysql_info.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /I "%MYSQL_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK /D HAVE_DBMON /D HAVE_MYSQL
  cl.exe ..\..\..\src\libs\zbxsysinfo\dbmon\pgsql_info.c /Fo"..\..\..\src\libs\zbxsysinfo\dbmon\pgsql_info.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK
  cl.exe ..\..\..\src\libs\zbxsysinfo\dbmon\oracle_info.c /Fo"..\..\..\src\libs\zbxsysinfo\dbmon\oracle_info.o"  /I ..\..\..\src\zabbix_agent /I .\ /I ..\include /I ..\..\..\include /I "%PCRE_PATH%\include" /I "%OPENSSL_PATH%\include" /I "%PTHREADS4W_PATH%\include" /I "%LIBCONIG_PATH%\include" /D WITH_AGENT_METRICS /D WITH_COMMON_METRICS  /D WITH_SPECIFIC_METRICS /D WITH_HOSTNAME_METRIC /D WITH_SIMPLE_METRICS  /Zi /D DEFAULT_CONFIG_FILE="\"C:\\zabbix_agentd.conf\""  /Fdzabbix_agentd.exe.pdb /D _WIN32_WINNT=0x0502 /nologo /O2 /GF /FD /EHsc /MT /Gy /W3 /c /D _WINDOWS /D _CONSOLE /D UNICODE  /D _UNICODE /D HAVE_WINLDAP_H /D HAVE_ASSERT_H /D ZABBIX_SERVICE /D "_VC80_UPGRADE=0x0600" /D HAVE_IPV6 /TC /DPCRE_STATIC /DHAVE_OPENSSL /DHAVE_OPENSSL_WITH_PSK

  echo ------------ All database support ------------
  nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" PTHREADS4WINCDIR="%PTHREADS4W_PATH%\include" PTHREADS4WLIBDIR="%PTHREADS4W_PATH%\lib\%VS_ARCH%" LIBCONIGINCDIR="%LIBCONIG_PATH%\include" LIBCONIGLIBDIR="%LIBCONIG_PATH%\lib\%VS_ARCH%" DBMON=yes DBMON_MYSQL=yes MYSQLINCDIR="%MYSQL_PATH%\include" MYSQLLIBDIR="%MYSQL_PATH%\lib\%VS_ARCH%" /f Makefile_agent

  echo Build Zabbix Sender %ZBX_ARCH%...
  nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" /f Makefile_sender

  echo Build Zabbix Get %ZBX_ARCH%...
  nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" /f Makefile_get
)
