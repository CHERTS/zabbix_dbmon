# Новая устанока из пакетов для RedHat/CentOS/OracleLinux

[In English / По-английски](RHELINSTALL.md)

[Новая установка из пакетов для RedHat/CentOS/OracleLinux 7](#redhat-7)

[Новая установка из пакетов для RedHat/CentOS/OracleLinux 8](#redhat-8)

# RedHat 7
## Новая устанока из пакетов для RedHat / CentOS / Oracle Linux 7

### 1. Установка репозитория DBService

Установка репозитория DBService

~~~~
rpm -Uvh https://repo.programs74.ru/zabbix/6.0/rhel/7/x86_64/dbs-release-6.0-1.el7.noarch.rpm
yum clean all
yum makecache fast
~~~~

### 2. Установка Zabbix agent DBMON (с поддержкой MySQL и PostgreSQL)

~~~~
yum install zabbix-agent-dbmon
~~~~

# RedHat 8
## Новая устанока из пакетов для RedHat / CentOS / Oracle Linux 8

### 1. Установка репозитория DBService

Установка репозитория DBService

~~~~
rpm -Uvh https://repo.programs74.ru/zabbix/6.0/rhel/8/x86_64/dbs-release-6.0-1.el8.noarch.rpm
yum clean all
yum makecache fast
~~~~

### 2. Установка Zabbix agent DBMON (с поддержкой MySQL и PostgreSQL)

~~~~
yum install zabbix-agent-dbmon
~~~~

Далее прочитайте [Инструкция по быстрому старту нативного мониторинга СУБД с помощью zabbix-agent-dbmon](HOWTO_START_DBMON.ru.md)
