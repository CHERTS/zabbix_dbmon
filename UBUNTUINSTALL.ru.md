# Новая устанока из пакетов для Ubuntu

[In English / По-английски](UBUNTUINSTALL.md)

## Новая установка из пакетов для Ubuntu 16.04/18.04/20.04

### 1. Установка репозитория DBService

Устанавливаем GPG ключ
~~~~
wget -q -O - https://repo.programs74.ru/dbservice-official-repo.key | apt-key add -
~~~~

Установка репозитария
~~~~
echo "deb [arch=$(dpkg --print-architecture)] https://repo.programs74.ru/zabbix/6.0/ubuntu $(lsb_release -c -s) main" > /etc/apt/sources.list.d/dbs.list
~~~~

### 2. Обновление пакетов zabbix

Обновите кэш пакетов
~~~~
apt-get update
~~~~


### 3. Установка Zabbix agent DBMON (с поддержкой MySQL и PostgreSQL)

~~~~
apt-get install zabbix-agent-dbmon
~~~~

Далее прочитайте [Инструкция по быстрому старту нативного мониторинга СУБД с помощью zabbix-agent](HOWTO_START_DBMON.ru.md)
