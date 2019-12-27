@echo off

title tail zabbix-agent log...

powershell Get-Content C:\DBS_Zabbix\dbs_zabbix_agentd.log -Wait -Tail 10
