# Как собрать zabbix-agent с поддержкой мониторинга СУБД из данного репозитария и начать нативный мониторинг СУБД с помощью zabbix-agent

[Сборка zabbix-agent под разные ОС Linux описана в отдельном документе](BUILD_DBMON.ru.md)

После сборки и установки агента Вам необходимо сделать:

1. Настроить подключение агента к MySQL или PostgreSQL (создать пользователя и назначить ему права);

Пример создания пользователя и назначения прав для MySQL есть в файле templates\db\dbmon\mysql_grants.sql

Пример создания пользователя и назначения прав для PostgreSQL есть в файле templates\db\dbmon\pgsql_grants.sql

2. Для MySQL прописать в файле zabbix_agentd.conf новые настройки:
~~~~
MySQLUser=zabbixmon
MySQLPassword=zabbixmon
~~~~

3. Запустить нового агента:
~~~~
systemctl start zabbix-agent
~~~~

4. Проверить лог агента:
~~~~
tail -n20 /var/log/zabbix/zabbix_agentd.log
~~~~

Лог старта должен быть примерто таким:
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

В нем Вы увидите с поддержкой мониторинга каких СУБД собран агент.

5. Теперь Вы можете импортировать в Zabbix web-frontend новые шаблоны из папки templates\db\dbmon в следующей последовательности:

Последовательность импорта шаблонов (НЕ НАРУШАЙТЕ ЕЕ!):
~~~~
1. templates_db_mysql_all_os.xml, templates_db_postgres_all_os.xml, templates_db_oracle_all_os.xml
2. templates_db_mysql_linux.xml, templates_db_mysql_windows.xml, templates_db_postgres_linux.xml, templates_db_postgres_windows.xml, templates_db_oracle_linux.xml и templates_db_oracle_windows.xml
3. templates_zabbix_agent_dbmon_linux.xml и templates_zabbix_agent_dbmon_windows.xml
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

6. Теперь подключите шаблон мониторинга к Вашему хосту в Zabbix web-frontend:

Для мониторинга MySQL в Linux шаблон "MySQL for Linux (Active, DBMON)"

Для мониторинга MySQL в Windows шаблон "MySQL for Windows (Active, DBMON)"

Для мониторинга PostgreSQL в Linux шаблон "PostgreSQL for Linux (Active, DBMON)"

Для мониторинга PostgreSQL в Windows шаблон "PostgreSQL for Windows (Active, DBMON)"

Для мониторинга Oracle в Linux шаблон "Oracle for Linux (Active, DBMON)"

Для мониторинга Oracle в Windows шаблон "Oracle for Windows (Active, DBMON)"

