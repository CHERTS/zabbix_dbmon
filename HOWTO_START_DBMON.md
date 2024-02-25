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
 22646:20200219:211042.119 Starting Zabbix Agent [XXXXXX]. Zabbix 6.0.27 (revision XXXXXXX).
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

### 7. Now you can import new templates from the [templates\db\dbmon](https://github.com/CHERTS/zabbix_dbmon/archive/refs/heads/master.zip) folder into Zabbix web-frontend in the following steps:

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

#### To monitor PostgreSQL on Linux:

(Required) Connection string:
~~~~
Macros: DBS_PGSQL_CONN_STRING
Value: host=localhost port=5432 dbname=postgres user=zabbixmon password=XXXXXXX connect_timeout=10
~~~~

(Optional) Name of the master process:
~~~~
Macros: DBS_PGSQL_SERVICE_NAME
Value: postgres
~~~~

(Optional) The username under which the master process is running:
~~~~
Macros: DBS_PGSQL_SERVICE_USER
Value: postgres
~~~~

(Optional) Parameters of the master process if the data directory is in a non-standard location:
~~~~
Macros: DBS_PGSQL_SERVICE_CMD_REGEXP
Value: ^.*(config_file|--config-file)=.*\.conf.*$
~~~~

#### To monitor PostgreSQL on Windows:

(Required) Connection string:
~~~~
Macros: DBS_PGSQL_CONN_STRING
Value: host=localhost port=5432 dbname=postgres user=zabbixmon password=XXXXXXX connect_timeout=10
~~~~

(Optional) Windows service name:
~~~~
Macros: DBS_PGSQL_SERVICE_NAME
Value: postgres-12
~~~~

#### To monitor MySQL on Linux:

ATTENTION! For MySQL monitoring, the username and password are entered in the agent configuration file.

(Optional) DNS name or IP address to connect to MySQL:
~~~~
Macros: DBS_MYSQL_HOST
Value: localhost
~~~~

(Optional) Process name:
~~~~
Macros: DBS_MYSQL_SERVICE_NAME
Value: mysqld
~~~~

(Optional) The username under which the process is running:
~~~~
Macros: DBS_MYSQL_SERVICE_USER
Value: mysql
~~~~

(Optional) Process launch options:
~~~~
Macros: DBS_MYSQL_SERVICE_CMD
Value: <empty>
~~~~

#### To monitor MySQL on Windows:

ATTENTION! For MySQL monitoring, the username and password are entered in the agent configuration file.

(Optional) DNS name or IP address to connect to MySQL:
~~~~
Macros: DBS_MYSQL_HOST
Value: localhost
~~~~

(Optional) Executable file name without extension:
~~~~
Macros: DBS_MYSQL_SERVICE_EXE_NAME
Value: mysqld
~~~~

(Optional) Windows service name:
~~~~
Macros: DBS_MYSQL_SERVICE_NAME
Value: MySQL
~~~~

#### To monitor Oracle on Linux or AIX:

ATTENTION! For Oracle monitoring, the username and password are entered in the agent configuration file.

(Required) Instance connection string in EasyConnect format:
~~~~
Macros: DBS_ORACLE_CONN_STRING
Value: 127.0.0.1:1521/orcl
~~~~

(Required) Instance name:
~~~~
Macros: DBS_ORACLE_INSTANCE
Value: orcl
~~~~

(Optional) Authorization rights when connecting to an instance (possible values: 0 - OCI_DEFAULT (default), 1 - OCI_SYSDBA, 2 - OCI_SYSOPER, 3 - OCI_SYSASM, 4 - OCI_SYSDGD):
~~~~
Macros: DBS_ORACLE_MODE
Value: 0
~~~~
When connecting to Oracle Standby, as a macro value, you need to choose 1 - OCI_SYSDBA or 4 - OCI_SYSDGD

(Optional) Имя Service Monitor процесса Oracle:
~~~~
Macros: DBS_ORACLE_SERVICE_NAME
Value: ora_smon
~~~~

#### To monitor Oracle on Windows:

ATTENTION! For Oracle monitoring, the username and password are entered in the agent configuration file.

(Required) Instance connection string in EasyConnect format:
~~~~
Macros: DBS_ORACLE_CONN_STRING
Value: 127.0.0.1:1521/orcl
~~~~

(Required) Instance name:
~~~~
Macros: DBS_ORACLE_INSTANCE
Value: orcl
~~~~

(Optional) Authorization rights when connecting to an instance (possible values: 0 - OCI_DEFAULT (default), 1 - OCI_SYSDBA, 2 - OCI_SYSOPER, 3 - OCI_SYSASM, 4 - OCI_SYSDGD):
~~~~
Macros: DBS_ORACLE_MODE
Value: 0
~~~~
When connecting to Oracle Standby, as a macro value, you need to choose 1 - OCI_SYSDBA or 4 - OCI_SYSDGD

(Optional) Oracle Windows service name:
~~~~
Macros: DBS_ORACLE_SERVICE_NAME
Value: OracleServiceORCL
~~~~

(Optional) Listener Windows service name:
~~~~
Macros: DBS_ORACLE_LSNR_SERVICE_NAME
Value: OracleOraDb11g_home1TNSListener
~~~~
