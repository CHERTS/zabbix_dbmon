CREATE USER 'zabbixmon'@'localhost' IDENTIFIED BY 'zabbixmon';
CREATE USER 'zabbixmon'@'127.0.0.1' IDENTIFIED BY 'zabbixmon';
GRANT USAGE,PROCESS,SHOW DATABASES,REPLICATION CLIENT,SELECT ON *.* TO 'zabbixmon'@'localhost' WITH MAX_USER_CONNECTIONS 3;
GRANT USAGE,PROCESS,SHOW DATABASES,REPLICATION CLIENT,SELECT ON *.* TO 'zabbixmon'@'127.0.0.1' WITH MAX_USER_CONNECTIONS 3;
FLUSH PRIVILEGES;
