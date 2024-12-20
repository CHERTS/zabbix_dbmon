@echo off

title Build zabbix-agent...

set ZBX_ARCH=i386
set VS_ARCH=x86
set BASE_DIR=D:\Neo\Project\zabbix\zabbix_dbmon_github
set PCRE_PATH=%BASE_DIR%\vs2017\pcre
set OPENSSL_PATH=%BASE_DIR%\vs2017\openssl

set ZBX_AGENTD_BIN=%BASE_DIR%\bin\win32\zabbix_agentd.exe
set ZBX_SENDER_BIN=%BASE_DIR%\bin\win32\zabbix_sender.exe
set ZBX_GET_BIN=%BASE_DIR%\bin\win32\zabbix_get.exe

set cert_timestamp_server=http://time.certum.pl
set cert_thumbprint=c5f60c11bb7effdea6c983dced1f0e95a8d547bf

title Build zabbix-agent %ZBX_ARCH%...

rem https://support.zabbix.com/browse/ZBXNEXT-3047
rem -- Visual Studio 2015 Pro --
rem call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %VS_ARCH% >nul 2>&1
rem -- Visual Studio 2022 --
echo Set Visual Studio 2022 enviroment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" %VS_ARCH% >nul 2>&1

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

cd build\win32\project
copy ..\include\config.h ..\..\..\include\ >nul 2>&1

echo Build Zabbix Agent %ZBX_ARCH%...
rem nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIBDIR="%OPENSSL_PATH%\lib" PCREINCDIR="%PCRE_PATH%\pcre-%PCRE_VER%-%VS_ARCH%\include" PCRELIBDIR="%PCRE_PATH%\pcre-%PCRE_VER%-%VS_ARCH%\lib" /f Makefile_agent
nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" /f Makefile_agent

rem if exist "%ZBX_AGENTD_BIN%" (
rem   signtool_x86.exe sign /sha1 "%cert_thumbprint%" /tr "%cert_timestamp_server%" /fd sha256 /v "%ZBX_AGENTD_BIN%"
rem ) else (
rem   echo "ERROR! %ZBX_AGENTD_BIN% not found."
rem )

echo Build Zabbix Sender %ZBX_ARCH%...
nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" /f Makefile_sender

rem if exist "%ZBX_SENDER_BIN%" (
rem   signtool_x86.exe sign /sha1 "%cert_thumbprint%" /tr "%cert_timestamp_server%" /fd sha256 /v "%ZBX_SENDER_BIN%"
rem ) else (
rem   echo "ERROR! %ZBX_SENDER_BIN% not found."
rem )

echo Build Zabbix Get %ZBX_ARCH%...
nmake CPU=%ZBX_ARCH% TLS=openssl TLSINCDIR="%OPENSSL_PATH%\include" TLSLIB="%OPENSSL_PATH%\lib\%VS_ARCH%\ssleay32MT.lib" TLSLIB2="%OPENSSL_PATH%\lib\%VS_ARCH%\libeay32MT.lib" PCREINCDIR="%PCRE_PATH%\include" PCRELIBDIR="%PCRE_PATH%\lib\%VS_ARCH%" /f Makefile_get

rem if exist "%ZBX_GET_BIN%" (
rem   signtool_x86.exe sign /sha1 "%cert_thumbprint%" /tr "%cert_timestamp_server%" /fd sha256 /v "%ZBX_GET_BIN%"
rem ) else (
rem   echo "ERROR! %ZBX_GET_BIN% not found."
rem )

echo Done, press Enter to exit.
pause >nul
