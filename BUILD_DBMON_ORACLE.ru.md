# Как собрать zabbix-agent-dbmon с поддержкой мониторинга Oracle 18c

[Сборка на Oracle Linux 7 с поддержкой Oracle 18c](#oracle-linux-7)

[Сборка на Red Hat Enterprise Linux 8 с поддержкой Oracle 18c](#red-hat-enterprise-linux-8)

# Oracle Linux 7
## Сборка на Oracle Linux 7 с поддержкой Oracle 18c

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

Далее прочитайте [Инструкция по быстрому старту нативного мониторинга СУБД с помощью zabbix-agent](HOWTO_START_DBMON.ru.md)

# Red Hat Enterprise Linux 8
## Сборка на Red Hat Enterprise Linux 8 с поддержкой MySQL (MariaDB) + PostgreSQL + Oracle 18c

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

Далее прочитайте [Инструкция по быстрому старту нативного мониторинга СУБД с помощью zabbix-agent](HOWTO_START_DBMON.ru.md)
