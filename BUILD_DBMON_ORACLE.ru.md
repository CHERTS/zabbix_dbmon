# Как собрать zabbix-agent-dbmon с поддержкой мониторинга Oracle

[Установка готового агента на Oracle Linux 7 с поддержкой Oracle 11g/12c/18c/19c](#oracle-linux-7-binary)

[Сборка из исходников на Oracle Linux 7 с поддержкой Oracle 18c](#oracle-linux-7)

[Сборка из исходников на Red Hat Enterprise Linux 8 с поддержкой Oracle 18c](#red-hat-enterprise-linux-8)

# Oracle Linux 7 Binary
## Установка готового агента на Oracle Linux 7 с поддержкой Oracle 11g/12c/18c/19c

### 1. Установка дополнительных пакетов на Oracle Linux 7:

~~~~
yum-config-manager --enable ol7_optional_latest
yum install -y wget unzip openssl libcurl libconfig
~~~~

### 2. Скачать и распаковать готовый архив для установки:

~~~~
wget https://github.com/CHERTS/zabbix_dbmon/raw/master/bin/rhel/7/zabbix_agent_dbmon-5.0.19-linux-oracle.tar.gz
tar -zxf zabbix_agent_dbmon-5.0.19-linux-oracle.tar.gz
cd zabbix_dbmon_oracle_ol7
~~~~

### 3. Отредактировать файл настроек под ваши задачи:

Откройте файл settings.conf и отредактируйте переменные ZBX_SERVERS и ZBX_ACTIVE_SERVERS, а так же ORA_VER

Переменные ZBX_SERVERS и ZBX_ACTIVE_SERVERS задают адрес ващего zabbix-server.

Переменная ORA_VER задает версию Oracle, поддерживаются версии 11.2, 12.2, 18.5, 19.3 и 19.8

### 4. Базовая установка агента:

Запустите скрипт install_zabbix_agent_dbmon.sh, он произведет базовую установку агента на вашу ОС:
~~~~
./install_zabbix_agent_dbmon.sh
~~~~

### 5. Настройка мониторинга Oracle:

ВАЖНО! Для правильной настройки мониторинга все работающие экземпляры Oracle должны быть описаны в /etc/oratab

Если на сервере запущен один экземпляр Oracle, то запустите скрипт create_single_instance_config.sh и следуйте его инструкциям. После настройки агента будет создан один systemd-сервис с именем zabbix-agent-dbmon.

Если на сервере запущено несколько экземпляров Oracle, то запустите скрипт create_multi_instance_config.sh и следуйте его инструкциям. После настройки агента будет создано нсколько systemd-сервисов для каждого экземпляра Oracle с именами zabbix-agent-dbmon@имя-экземпляра-oracle.

### 6. Настройка агента для мониторинга Oracle, импорт шаблонов и т.п.:

Далее прочитайте [Инструкция по быстрому старту нативного мониторинга СУБД с помощью zabbix-agent-dbmon](HOWTO_START_DBMON.ru.md)

# Oracle Linux 7
## Сборка из исходников на Oracle Linux 7 с поддержкой Oracle 18c

### 1. Для подготовки к сборки на Oracle Linux 7 нужно установить дополнительные пакеты:

~~~~
yum group install "Development Tools"
yum-config-manager --enable ol7_optional_latest
yum-config-manager --enable ol7_developer
yum install -y wget unzip gettext libxml2-devel openssl-devel libcurl-devel pcre-devel libssh2-devel libconfig-devel
~~~~

### 2. Скачать и распаковать свежую версию исходного кода:

~~~~
wget https://github.com/CHERTS/zabbix_dbmon/releases/download/v5.0.19/zabbix-5.0.19.tar.gz
tar -zxf zabbix-5.0.19.tar.gz
cd zabbix-5.0.19
~~~~

### 3. Сборка zabbix-agent с поддержкой Oracle 18c:

~~~~
./configure --with-openssl --with-libpthread --with-libpcre --with-libcurl --enable-dbmon --enable-dbmon-oracle --with-oracle --with-oracle-lib=/u01/app/oracle/18c/dbhome_1/lib --with-oracle-include=/u01/app/oracle/18c/dbhome_1/rdbms/public --enable-ipv6 --enable-agent --sysconfdir=/etc/zabbix
make
~~~~

### 4. После успешной сборки на шаге 3 можно использовать бинарные файлы zabbix, проверим факт наличия файла агента:

~~~~
ls -l src/zabbix_agent | grep -E 'zabbix_agentd$'
-rwxr-xr-x  1 root       root        2021176 Feb 19 21:17 zabbix_agentd
~~~~

### 5. Скопировать бинарники агента в /sbin

~~~~
cp src/zabbix_agent/zabbix_agentd /sbin/zabbix_agentd_dbmon
~~~~

### 6. Скопировать файл конфигурации и создать вспомогательные каталоги

~~~~
mkdir -p /etc/zabbix/zabbix_agentd_dbmon.d
cp conf/zabbix_agentd_dbmon.conf /etc/zabbix
cp conf/zabbix_agentd_dbmon/userparameter_dbmon.conf /etc/zabbix/zabbix_agentd_dbmon.d
cp conf/zabbix_agentd_dbmon/dbmon.sh /etc/zabbix/zabbix_agentd_dbmon.d
~~~~

Далее прочитайте [Инструкция по быстрому старту нативного мониторинга СУБД с помощью zabbix-agent-dbmon](HOWTO_START_DBMON.ru.md)

# Red Hat Enterprise Linux 8
## Сборка из исходников на Red Hat Enterprise Linux 8 с поддержкой Oracle 18c

### 1. Для подготовки к сборки на Red Hat Enterprise Linux 8 нужно установить дополнительные пакеты:

~~~~
dnf group install "Development Tools"
dnf install -y wget unzip gettext libxml2-devel openssl-devel libcurl-devel pcre-devel libconfig-devel
~~~~

### 2. Скачать и распаковать свежую версию исходного кода:

~~~~
wget https://github.com/CHERTS/zabbix_dbmon/releases/download/v5.0.19/zabbix-5.0.19.tar.gz
tar -zxf zabbix-5.0.19.tar.gz
cd zabbix-5.0.19
~~~~

### 3. Сборка zabbix-agent с поддержкой Oracle 18c:

~~~~
./configure --with-openssl --with-libpthread --with-libpcre --with-libcurl --enable-dbmon --enable-dbmon-oracle --with-oracle --with-oracle-lib=/u01/app/oracle/18c/dbhome_1/lib --with-oracle-include=/u01/app/oracle/18c/dbhome_1/rdbms/public --enable-ipv6 --enable-agent --sysconfdir=/etc/zabbix
make
~~~~

### 4. После успешной сборки на шаге 3 можно использовать бинарные файлы zabbix, проверим факт наличия файла агента:

~~~~
ls -l src/zabbix_agent | grep -E 'zabbix_agentd$'
-rwxr-xr-x  1 root       root        2021176 Feb 19 21:17 zabbix_agentd
~~~~

### 5. Скопировать бинарники агента в /sbin

~~~~
cp src/zabbix_agent/zabbix_agentd /sbin/zabbix_agentd_dbmon
~~~~

### 6. Скопировать файл конфигурации и создать вспомогательные каталоги

~~~~
mkdir -p /etc/zabbix/zabbix_agentd_dbmon.d
cp conf/zabbix_agentd_dbmon.conf /etc/zabbix
cp conf/zabbix_agentd_dbmon/userparameter_dbmon.conf /etc/zabbix/zabbix_agentd_dbmon.d
cp conf/zabbix_agentd_dbmon/dbmon.sh /etc/zabbix/zabbix_agentd_dbmon.d
~~~~

Далее прочитайте [Инструкция по быстрому старту нативного мониторинга СУБД с помощью zabbix-agent-dbmon](HOWTO_START_DBMON.ru.md)
