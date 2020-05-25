@echo off

title Restarting all agents...

set service_name_list="Zabbix Agent [DB01]" "Zabbix Agent [DB02]"

net session >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
	echo =======================================================================
	echo.
	echo Error: To restart service need admin rights.
	echo.
	echo Run the script as an administrator.
	echo.
	echo To run as administrator, click on the file the right mouse
	echo button and select 'Run as administrator'
	echo.
	echo =======================================================================
	echo Press Enter to exit.
	pause >nul
	goto :eof
)

:stop
for %%a in (%service_name_list%) do (
	echo Stoping service '%%a'...
	sc stop %%a>nul
)

rem cause a ~10 second sleep before checking the service state
echo Wait 3 sec...
ping 127.0.0.1 -n 5 -w 1000 > nul

for %%a in (%service_name_list%) do (
	echo Checking service '%%a'...
	sc query %%a | find /I "STATE" | find "STOPPED">nul
	if errorlevel 1 goto :stop
)
goto :start

:start
for %%a in (%service_name_list%) do (
	echo Starting service '%%a'...
	sc start %%a>nul
)
