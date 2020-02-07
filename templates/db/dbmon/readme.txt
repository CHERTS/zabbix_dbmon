Последовательность импорта шаблонов:
1. templates_db_mysql_all_os.xml и templates_db_oracle_all_os.xml
2. templates_db_mysql_linux.xml, templates_db_mysql_windows.xml, templates_db_oracle_linux.xml и templates_db_oracle_windows.xml
3. templates_zabbix_agent_dbmon_linux.xml и templates_zabbix_agent_dbmon_windows.xml

Дополнительные действия по связыванию триггеров для MySQL for Windows:
1. Зайдите в шаблон "MySQL for Windows (Active, DBMON)"
2. Перейдите в список триггеров
3. Откройте триггер "Instance access failed" (он наследуется из шаблона "DB MySQL (Active, DBMON, for all OS)")
4. Откройте зависимости в триггере из п.3
5. Добавьте зависимость от триггера "Service '{$DBS_MYSQL_SERVICE_NAME}' is not running" (Service 'MySQL' is not running)

