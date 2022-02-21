# Инструкция по быстрому старту нативного мониторинга СУБД с помощью zabbix-agent-dbmon

[In English / По-английски](HOWTO_START_DBMON.md)

После базовой установки агента Вам необходимо сделать:

### 1. Настроить подключение агента к MySQL/PostgreSQL или Oracle (создать пользователя и назначить ему права);

Пример создания пользователя и назначения прав для MySQL есть в файле templates\db\dbmon\mysql_grants.sql

Пример создания пользователя и назначения прав для PostgreSQL есть в файле templates\db\dbmon\pgsql_grants.sql

Пример создания пользователя и назначения прав для Oracle есть в файле templates\db\dbmon\oracle_grants.sql

### 2. Отредактировать файл конфигурации /etc/zabbix/zabbix_agentd_dbmon.conf

Если Вы собрали агента с поддержкой мониторинга СУБД MySQL прописать в файле zabbix_agentd_dbmon.conf новые настройки:
~~~~
MySQLUser=zabbixmon
MySQLPassword=zabbixmon
~~~~

Если Вы собрали агента с поддержкой мониторинга СУБД Oracle, то прописать в файле zabbix_agentd_dbmon.conf новые настройки:
~~~~
OracleUser=zabbixmon
OraclePassword=zabbixmon
~~~~

### 3. Если Вы собрали агента из исходников с поддержкой мониторинга СУБД Oracle, то необходимо сделать доп. настройки - создать файл /etc/sysconfig/zabbix-agent-dbmon следующего вида:

~~~~
ORACLE_HOME=/u01/app/oracle/18c/dbhome_1
ORACLE_SID=orcl
ORACLE_BASE=/u01/orabase
PATH=$ORACLE_HOME/bin:/sbin:/bin:/usr/sbin:/usr/bin
LD_LIBRARY_PATH=/u01/app/oracle/18c/dbhome_1/lib:${LD_LIBRARY_PATH}
~~~~

### 4. Если Вы собрали агента с поддержкой мониторинга СУБД Oracle, то необходимо добавить пользователя zabbix в группу oinstall, таким образом агент сможет читать некоторые каталоги и файлы из $ORACLE_HOME:

~~~~
usermod -a -G oinstall zabbix
~~~~

### 5. Запустить нового агента:

~~~~
systemctl start zabbix-agent-dbmon
systemctl enable zabbix-agent-dbmon
~~~~

### 6. Проверить лог агента:
~~~~
tail -n20 /var/log/zabbix/zabbix_agentd_dbmon.log
~~~~

Лог старта должен быть примерто таким:
~~~~
 22646:20200219:211042.119 Starting Zabbix Agent [XXXXXX]. Zabbix 5.0.19 (revision XXXXXXX).
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

В нем Вы увидите с поддержкой мониторинга каких СУБД собран агент.

### 7. Теперь Вы можете импортировать в Zabbix web-frontend новые шаблоны из папки templates\db\dbmon в следующей последовательности:

Последовательность импорта шаблонов:
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

Дополнительные действия по связыванию триггеров для MySQL for Windows:
~~~~
1. Зайдите в шаблон "MySQL for Windows (Active, DBMON)"
2. Перейдите в список триггеров
3. Откройте триггер "Instance access failed" (он наследуется из шаблона "DB MySQL (Active, DBMON, for all OS)")
4. Откройте зависимости в триггере из п.3
5. Добавьте зависимость от триггера "Service '{$DBS_MYSQL_SERVICE_NAME}' is not running" (Service 'MySQL' is not running)
~~~~

Дополнительные действия по связыванию триггеров для Oracle for Windows:
~~~~
1. Зайдите в шаблон "Oracle for Windows (Active, DBMON)"
2. Перейдите в список триггеров
3. Откройте триггер "Instance access failed" (он наследуется из шаблона "DB Oracle (Active, DBMON, for all OS)")
4. Откройте зависимости в триггере из п.3
5. Добавьте зависимость от триггера "Service '{$DBS_ORACLE_SERVICE_NAME}' is not running" (Service 'OracleServiceORCL' is not running)
~~~~

Дополнительные действия по связыванию триггеров для PostgreSQL for Windows:
~~~~
1. Зайдите в шаблон "PostgreSQL for Windows (Active, DBMON)"
2. Перейдите в список триггеров
3. Откройте триггер "Instance access failed" (он наследуется из шаблона "DB PostgreSQL (Active, DBMON, for all OS)")
4. Откройте зависимости в триггере из п.3
5. Добавьте зависимость от триггера "Service '{$DBS_PGSQL_SERVICE_NAME}' is not running" (Service 'postgresql-12' is not running)
~~~~

### 8. Теперь подключите шаблон мониторинга к Вашему хосту в Zabbix web-frontend:

Для мониторинга MySQL в Linux шаблон "MySQL for Linux (Active, DBMON)"

Для мониторинга MySQL в Windows шаблон "MySQL for Windows (Active, DBMON)"

Для мониторинга PostgreSQL в Linux шаблон "PostgreSQL for Linux (Active, DBMON)"

Для мониторинга PostgreSQL в Windows шаблон "PostgreSQL for Windows (Active, DBMON)"

Для мониторинга Oracle в Linux шаблон "Oracle for Linux (Active, DBMON)"

Для мониторинга Oracle в AIX шаблон "Oracle for AIX (Active, DBMON)"

Для мониторинга Oracle в Windows шаблон "Oracle for Windows (Active, DBMON)"

### 9. Добавьте необходимые макросы для корректной работы мониторинга.

#### Для мониторинга PostgreSQL в Linux:

(Обязательно) Строка подключение в кластеру:
~~~~
Макрос: DBS_PGSQL_CONN_STRING
Значение: host=localhost port=5432 dbname=postgres user=zabbixmon password=XXXXXXX connect_timeout=10
~~~~

(Опционально) Имя master-процесса:
~~~~
Макрос: DBS_PGSQL_SERVICE_NAME
Значение: postgres
~~~~

(Опционально) Имя пользователя под которым работает master-процесс:
~~~~
Макрос: DBS_PGSQL_SERVICE_USER
Значение: postgres
~~~~

(Опционально) Параметры master-процесса если каталог данных находиться в нестандартном месте:
~~~~
Макрос: DBS_PGSQL_SERVICE_CMD_REGEXP
Значение: ^.*(config_file|--config-file)=.*\.conf.*$
~~~~

#### Для мониторинга PostgreSQL в Windows:

(Обязательно) Строка подключение в кластеру:
~~~~
Макрос: DBS_PGSQL_CONN_STRING
Значение: host=localhost port=5432 dbname=postgres user=zabbixmon password=XXXXXXX connect_timeout=10
~~~~

(Опционально) Имя Windows-службы:
~~~~
Макрос: DBS_PGSQL_SERVICE_NAME
Значение: postgres-12
~~~~

#### Для мониторинга MySQL в Linux:

ВНИМАНИЕ! Для мониторинга MySQL имя пользователя и пароль вводятся в файле конфигурации агента.

(Опционально) DNS-имя или IP-адрес для подключения к MySQL:
~~~~
Макрос: DBS_MYSQL_HOST
Значение: localhost
~~~~

(Опционально) Имя процесса:
~~~~
Макрос: DBS_MYSQL_SERVICE_NAME
Значение: mysqld
~~~~

(Опционально) Имя пользователя под которым работает процесс:
~~~~
Макрос: DBS_MYSQL_SERVICE_USER
Значение: mysql
~~~~

(Опционально) Параметры запуска процесса:
~~~~
Макрос: DBS_MYSQL_SERVICE_CMD
Значение: <пусто>
~~~~

#### Для мониторинга MySQL в Windows:

ВНИМАНИЕ! Для мониторинга MySQL имя пользователя и пароль вводятся в файле конфигурации агента.

(Опционально) DNS-имя или IP-адрес для подключения к MySQL:
~~~~
Макрос: DBS_MYSQL_HOST
Значение: localhost
~~~~

(Опционально) Имя исполняемого файлы без расширения:
~~~~
Макрос: DBS_MYSQL_SERVICE_EXE_NAME
Значение: mysqld
~~~~

(Опционально) Имя Windows-службы:
~~~~
Макрос: DBS_MYSQL_SERVICE_NAME
Значение: MySQL
~~~~

#### Для мониторинга Oracle в Linux или AIX:

ВНИМАНИЕ! Для мониторинга Oracle имя пользователя и пароль вводятся в файле конфигурации агента.

(Обязательно) Строка подключения к экземпляру в формате EasyConnect:
~~~~
Макрос: DBS_ORACLE_CONN_STRING
Значение: 127.0.0.1:1521/orcl
~~~~

(Обязательно) Имя экземпляра:
~~~~
Макрос: DBS_ORACLE_INSTANCE
Значение: orcl
~~~~

(Опционально) Права авторизации при подключении к экземпляру (возможные значения: 0 - OCI_DEFAULT (по-умолчанию), 1 - OCI_SYSDBA, 2 - OCI_SYSOPER, 3 - OCI_SYSASM, 4 - OCI_SYSDGD):
~~~~
Макрос: DBS_ORACLE_MODE
Значение: 0
~~~~
При подключении к Oracle Standby как правило нужно выбирать 1 - OCI_SYSDBA или 4 - OCI_SYSDGD

(Опционально) Имя Service Monitor процесса Oracle:
~~~~
Макрос: DBS_ORACLE_SERVICE_NAME
Значение: ora_smon
~~~~

#### Для мониторинга Oracle в Windows:

ВНИМАНИЕ! Для мониторинга Oracle имя пользователя и пароль вводятся в файле конфигурации агента.

(Обязательно) Строка подключения к экземпляру в формате EasyConnect:
~~~~
Макрос: DBS_ORACLE_CONN_STRING
Значение: 127.0.0.1:1521/orcl
~~~~

(Обязательно) Имя экземпляра:
~~~~
Макрос: DBS_ORACLE_INSTANCE
Значение: orcl
~~~~

(Опционально) Права авторизации при подключении к экземпляру (возможные значения: 0 - OCI_DEFAULT (по-умолчанию), 1 - OCI_SYSDBA, 2 - OCI_SYSOPER, 3 - OCI_SYSASM, 4 - OCI_SYSDGD):
~~~~
Макрос: DBS_ORACLE_MODE
Значение: 0
~~~~
При подключении к Oracle Standby как правило нужно выбирать 1 - OCI_SYSDBA или 4 - OCI_SYSDGD

(Опционально) Имя Windows-службы Oracle:
~~~~
Макрос: DBS_ORACLE_SERVICE_NAME
Значение: OracleServiceORCL
~~~~

(Опционально) Имя Windows-службы Listener:
~~~~
Макрос: DBS_ORACLE_LSNR_SERVICE_NAME
Значение: OracleOraDb11g_home1TNSListener
~~~~
