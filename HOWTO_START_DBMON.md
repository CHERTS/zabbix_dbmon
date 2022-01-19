# Instructions for a quick start of native DBMS monitoring using zabbix-agent-dbmon

[По-русски / In Russian](HOWTO_START_DBMON.ru.md)

After installing the agent from the repository, you need to do:

### 1. Set up an agent connection to MySQL/PostgreSQL or Oracle (create a user and add rights);

An example of creating a user and add rights for MySQL is in the file templates\db\dbmon\mysql_grants.sql

An example of creating a user and add rights for PostgreSQL is in the file templates\db\dbmon\pgsql_grants.sql

An example of creating a user and add rights for Oracle is in the file templates\db\dbmon\oracle_grants.sql

### 2. Edit configuration file /etc/zabbix/zabbix_agentd_dbmon.conf

If you have compiled an agent with MySQL DBMS monitoring support, add new settings in the zabbix_agentd_dbmon.conf file:
~~~~
MySQLUser=zabbixmon
MySQLPassword=zabbixmon
~~~~

If you have compiled an agent with Oracle monitoring support, then add new settings to the zabbix_agentd_dbmon.conf file:
~~~~
OracleUser=zabbixmon
OraclePassword=zabbixmon
~~~~

### 3. If you have compiled the agent from source with support for Oracle monitoring, then you need to make additional settings - create the /etc/sysconfig/zabbix-agent-dbmon file of the following form:

~~~~
ORACLE_HOME=/u01/app/oracle/18c/dbhome_1
ORACLE_SID=orcl
ORACLE_BASE=/u01/orabase
PATH=$ORACLE_HOME/bin:/sbin:/bin:/usr/sbin:/usr/bin
LD_LIBRARY_PATH=/u01/app/oracle/18c/dbhome_1/lib:${LD_LIBRARY_PATH}
~~~~

### 4. If you have compiled an agent with Oracle monitoring support, then you need to add the zabbix user to the oinstall group, so the agent will be able to read some directories and files from $ORACLE_HOME:

~~~~
usermod -a -G oinstall zabbix
~~~~

### 5. Start a new agent:

~~~~
systemctl start zabbix-agent-dbmon
systemctl enable zabbix-agent-dbmon
~~~~

### 6. Check agent log file:
~~~~
tail -n20 /var/log/zabbix/zabbix_agentd_dbmon.log
~~~~

The start log should look something like this:
~~~~
 22646:20200219:211042.119 Starting Zabbix Agent [XXXXXX]. Zabbix 5.0.16 (revision XXXXXXX).
 22646:20200219:211042.119 **** Enabled features ****
 22646:20200219:211042.119 IPv6 support:          YES
 22646:20200219:211042.119 TLS support:           YES
 22646:20200219:211042.119 MySQL support:         YES
 22646:20200219:211042.119 PostgreSQL support:    YES
 22646:20200219:211042.119 Oracle support:        YES
 22646:20200219:211042.119 MSSQL support:         NO
 22646:20200219:211042.119 **************************
 22646:20200219:211042.119 using configuration file: /etc/zabbix/zabbix_agentd.conf
 22646:20200219:211042.119 agent #0 started [main process]
 22647:20200219:211042.120 agent #1 started [collector]
 22648:20200219:211042.120 agent #2 started [listener #1]
 22649:20200219:211042.120 agent #3 started [active checks #1]
~~~~

In it you will see which DB the agent is built with monitoring support for.

### 7. Now you can import new templates from the templates\db\dbmon folder into Zabbix web-frontend in the following steps:

Template import steps:
~~~~
1. templates_zabbix_agent_dbmon_all_os.xml
2. templates_db_mysql_all_os.xml
3. templates_db_postgres_all_os.xml
4. templates_db_oracle_all_os.xml
5. templates_db_oracle_asm_all_os.xml
6. templates_db_mysql_windows_linux.xml
7. templates_db_postgres_windows_linux.xml
8. templates_db_oracle_asm_aix.xml
9. templates_db_oracle_windows_linux_aix.xml
~~~~

Additional trigger link steps for MySQL for Windows:
~~~~
1. Open template "MySQL for Windows (Active, DBMON)"
2. Open triggers lists
3. Open trigger "Instance access failed" (it is inherited from the template "DB MySQL (Active, DBMON, for all OS)")
4. Open the dependencies in the trigger from step 3
5. Add dependency on trigger "Service '{$DBS_MYSQL_SERVICE_NAME}' is not running" (Service 'MySQL' is not running)
~~~~

Additional trigger link steps for Oracle for Windows:
~~~~
1. Open template "Oracle for Windows (Active, DBMON)"
2. Open triggers lists
3. Open trigger "Instance access failed" (it is inherited from the template "DB Oracle (Active, DBMON, for all OS)")
4. Open the dependencies in the trigger from step 3
5. Add dependency on trigger "Service '{$DBS_ORACLE_SERVICE_NAME}' is not running" (Service 'OracleServiceORCL' is not running)
~~~~

Additional trigger link steps for PostgreSQL for Windows:
~~~~
1. Open template "PostgreSQL for Windows (Active, DBMON)"
2. Open triggers lists
3. Open trigger "Instance access failed" (it is inherited from the template "DB PostgreSQL (Active, DBMON, for all OS)")
4. Open the dependencies in the trigger from step 3
5. Add dependency on trigger "Service '{$DBS_PGSQL_SERVICE_NAME}' is not running" (Service 'postgresql-12' is not running)
~~~~

### 8. Now connect the monitoring template to your host in Zabbix web-frontend:

To monitor MySQL on Linux template "MySQL for Linux (Active, DBMON)"

To monitor MySQL on Windows template "MySQL for Windows (Active, DBMON)"

To monitor PostgreSQL on Linux template "PostgreSQL for Linux (Active, DBMON)"

To monitor PostgreSQL on Windows template "PostgreSQL for Windows (Active, DBMON)"

To monitor Oracle on Linux template "Oracle for Linux (Active, DBMON)"

To monitor Oracle on AIX template "Oracle for AIX (Active, DBMON)"

To monitor Oracle on Windows template "Oracle for Windows (Active, DBMON)"

### 9. Add the necessary zabbix macros for the monitoring to work correctly.
