# ��� ������� zabbix-agent � ���������� ����������� ���� �� ������� ����������� � ������ �������� ���������� ���� � ������� zabbix-agent

[������ zabbix-agent ��� ������ �� Linux ������� � ��������� ���������](BUILD_DBMON.ru.md)

����� ������ � ��������� ������ ��� ���������� �������:

1. ��������� ����������� ������ � MySQL ��� PostgreSQL (������� ������������ � ��������� ��� �����);

������ �������� ������������ � ���������� ���� ��� MySQL ���� � ����� templates\db\dbmon\mysql_grants.sql

������ �������� ������������ � ���������� ���� ��� PostgreSQL ���� � ����� templates\db\dbmon\pgsql_grants.sql

2. ��� MySQL ��������� � ����� zabbix_agentd.conf ����� ���������:
~~~~
MySQLUser=zabbixmon
MySQLPassword=zabbixmon
~~~~

3. ��������� ������ ������:
~~~~
systemctl start zabbix-agent
~~~~

4. ��������� ��� ������:
~~~~
tail -n20 /var/log/zabbix/zabbix_agentd.log
~~~~

��� ������ ������ ���� �������� �����:
~~~~
 22646:20200219:211042.119 Starting Zabbix Agent [XXXXXX]. Zabbix 4.4.5 (revision b93f5c4fc0).
 22646:20200219:211042.119 **** Enabled features ****
 22646:20200219:211042.119 IPv6 support:          YES
 22646:20200219:211042.119 TLS support:           YES
 22646:20200219:211042.119 MySQL support:         YES
 22646:20200219:211042.119 PostgreSQL support:    YES
 22646:20200219:211042.119 Oracle support:        NO
 22646:20200219:211042.119 MSSQL support:         NO
 22646:20200219:211042.119 **************************
 22646:20200219:211042.119 using configuration file: /etc/zabbix/zabbix_agentd.conf
 22646:20200219:211042.119 agent #0 started [main process]
 22647:20200219:211042.120 agent #1 started [collector]
 22648:20200219:211042.120 agent #2 started [listener #1]
 22649:20200219:211042.120 agent #3 started [active checks #1]
~~~~

� ��� �� ������� � ���������� ����������� ����� ���� ������ �����.

5. ������ �� ������ ������������� � Zabbix web-frontend ����� ������� �� ����� templates\db\dbmon � ��������� ������������������:

������������������ ������� �������� (�� ��������� ��!):
~~~~
1. templates_db_mysql_all_os.xml, templates_db_postgres_all_os.xml, templates_db_oracle_all_os.xml
2. templates_db_mysql_linux.xml, templates_db_mysql_windows.xml, templates_db_postgres_linux.xml, templates_db_postgres_windows.xml, templates_db_oracle_linux.xml � templates_db_oracle_windows.xml
3. templates_zabbix_agent_dbmon_linux.xml � templates_zabbix_agent_dbmon_windows.xml
~~~~

�������������� �������� �� ���������� ��������� ��� MySQL for Windows:
~~~~
1. ������� � ������ "MySQL for Windows (Active, DBMON)"
2. ��������� � ������ ���������
3. �������� ������� "Instance access failed" (�� ����������� �� ������� "DB MySQL (Active, DBMON, for all OS)")
4. �������� ����������� � �������� �� �.3
5. �������� ����������� �� �������� "Service '{$DBS_MYSQL_SERVICE_NAME}' is not running" (Service 'MySQL' is not running)
~~~~

�������������� �������� �� ���������� ��������� ��� Oracle for Windows:
~~~~
1. ������� � ������ "Oracle for Windows (Active, DBMON)"
2. ��������� � ������ ���������
3. �������� ������� "Instance access failed" (�� ����������� �� ������� "DB Oracle (Active, DBMON, for all OS)")
4. �������� ����������� � �������� �� �.3
5. �������� ����������� �� �������� "Service '{$DBS_ORACLE_SERVICE_NAME}' is not running" (Service 'OracleServiceORCL' is not running)
~~~~

�������������� �������� �� ���������� ��������� ��� PostgreSQL for Windows:
~~~~
1. ������� � ������ "PostgreSQL for Windows (Active, DBMON)"
2. ��������� � ������ ���������
3. �������� ������� "Instance access failed" (�� ����������� �� ������� "DB PostgreSQL (Active, DBMON, for all OS)")
4. �������� ����������� � �������� �� �.3
5. �������� ����������� �� �������� "Service '{$DBS_PGSQL_SERVICE_NAME}' is not running" (Service 'postgresql-12' is not running)
~~~~

6. ������ ���������� ������ ����������� � ������ ����� � Zabbix web-frontend:

��� ����������� MySQL � Linux ������ "MySQL for Linux (Active, DBMON)"

��� ����������� MySQL � Windows ������ "MySQL for Windows (Active, DBMON)"

��� ����������� PostgreSQL � Linux ������ "PostgreSQL for Linux (Active, DBMON)"

��� ����������� PostgreSQL � Windows ������ "PostgreSQL for Windows (Active, DBMON)"

��� ����������� Oracle � Linux ������ "Oracle for Linux (Active, DBMON)"

��� ����������� Oracle � Windows ������ "Oracle for Windows (Active, DBMON)"

