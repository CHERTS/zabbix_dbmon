@echo off

title Restart zabbix-agent..

set service_name="Zabbix Agent [MYHOMEPC]"

:stop
echo Stoping service...
sc stop %service_name%>nul

rem cause a ~3 second sleep before checking the service state
echo Wait 3 sec...
ping 127.0.0.1 -n 3 -w 1000 > nul

sc query %service_name% | find /I "STATE" | find "STOPPED">nul
if errorlevel 1 goto :stop
goto :start

:start
echo Starting service...
net start | find /i %service_name%>nul && goto :start
sc start %service_name%>nul
