# Как собрать zabbix-agent с поддержкой мониторинга СУБД из данного репозитария и начать нативный мониторинг СУБД с помощью zabbix-agent

[Сборка zabbix-agent под разные ОС Linux с поддержкой MySQL и PostgreSQL](BUILD_DBMON.ru.md)

[Сборка zabbix-agent под Oracle Linux (RedHat) с поддержкой СУБД Oracle](BUILD_DBMON_ORACLE.ru.md)

После сборки и установки агента Вам необходимо сделать:

1. Переименовать файл /etc/zabbix/zabbix_agentd.d/userparameter_mysql.conf

Т.к. в новом агенте мониторинга СУБД имена некоторых элементов данных пересекаются с именами из стандартного файла userparameter_mysql.conf, то
чтобы новый агент запустился Вы должны переименовать файл userparameter_mysql.conf

~~~~
mv /etc/zabbix/zabbix_agentd.d/userparameter_mysql.{conf,bak}
~~~~

ВНИМАНИЕ! Если Вы осуществляете мониторинг MySQL с помощью userparameter_mysql.conf, то его переименование заставит работать мониторинг некорректно.
Если Вам хочется оставить userparameter_mysql.conf для мониторинга СУБД MySQL и использовать новый агент, то необходимо запускать нового агента с
отдельным файлом конфигурации, для этого есть отдельная Инструкция.

2. Настроить подключение агента к MySQL/PostgreSQL или Oracle (создать пользователя и назначить ему права);

Пример создания пользователя и назначения прав для MySQL есть в файле templates\db\dbmon\mysql_grants.sql

Пример создания пользователя и назначения прав для PostgreSQL есть в файле templates\db\dbmon\pgsql_grants.sql

Пример создания пользователя и назначения прав для Oracle есть в файле templates\db\dbmon\oracle_grants.sql

3. Прописать в файле zabbix_agentd.conf новые настройки:

~~~~
Alias=dbmon.vfs.fs.discovery[*]:vfs.fs.discovery
Alias=dbmon.vfs.fs.size[*]:vfs.fs.size
Alias=dbmon.service.discovery[*]:service.discovery
Alias=dbmon.service.info[*]:service.info
Alias=dbmon.agent.ping[*]:agent.ping
Alias=dbmon.agent.version[*]:agent.version
Alias=dbmon.agent.hostname[*]:agent.hostname
~~~~

Если Вы собрали агента с поддержкой мониторинга СУБД MySQL прописать в файле zabbix_agentd.conf новые настройки:
~~~~
MySQLUser=zabbixmon
MySQLPassword=zabbixmon
~~~~

Если Вы собрали агента с поддержкой мониторинга СУБД Oracle, то прописать в файле zabbix_agentd.conf новые настройки:
~~~~
OracleUser=zabbixmon
OraclePassword=zabbixmon
~~~~

4. Если Вы собрали агента с поддержкой мониторинга СУБД Oracle, то необходимо сделать доп. настройки - создать файл /etc/sysconfig/zabbix-agent следующего вида:

~~~~
ORACLE_HOME=/u01/app/oracle/18c/dbhome_1
ORACLE_SID=orcl
ORACLE_BASE=/u01/orabase
PATH=$ORACLE_HOME/bin:/sbin:/bin:/usr/sbin:/usr/bin
LD_LIBRARY_PATH=/u01/app/oracle/18c/dbhome_1/lib:${LD_LIBRARY_PATH}
~~~~

5. Запустить нового агента:
~~~~
systemctl start zabbix-agent
~~~~

6. Проверить лог агента:
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

7. Теперь Вы можете импортировать в Zabbix web-frontend новые шаблоны из папки templates\db\dbmon в следующей последовательности:

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

8. Теперь подключите шаблон мониторинга к Вашему хосту в Zabbix web-frontend:

Для мониторинга MySQL в Linux шаблон "MySQL for Linux (Active, DBMON)"

Для мониторинга MySQL в Windows шаблон "MySQL for Windows (Active, DBMON)"

Для мониторинга PostgreSQL в Linux шаблон "PostgreSQL for Linux (Active, DBMON)"

Для мониторинга PostgreSQL в Windows шаблон "PostgreSQL for Windows (Active, DBMON)"

Для мониторинга Oracle в Linux шаблон "Oracle for Linux (Active, DBMON)"

Для мониторинга Oracle в AIX шаблон "Oracle for AIX (Active, DBMON)"

Для мониторинга Oracle в Windows шаблон "Oracle for Windows (Active, DBMON)"

