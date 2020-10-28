# Как собрать Zabbix-agent с поддержкой мониторинга MySQL (MariaDB) + PostgreSQL + Oracle 18c из данного репозитария

[Сборка на Oracle Linux 7 с поддержкой MySQL (MariaDB) + PostgreSQL + Oracle 18c](#oracle-linux-7)

[Сборка на Red Hat Enterprise Linux 8 с поддержкой MySQL (MariaDB) + PostgreSQL + Oracle 18c](#red-hat-enterprise-linux-8)

# Oracle Linux 7
## Сборка на Oracle Linux 7 с поддержкой MySQL (MariaDB) + PostgreSQL + Oracle 18c

### 1. Для подготовки к сборки на Oracle Linux 7 нужно установить дополнительные пакеты:

~~~~
yum group install "Development Tools"
yum-config-manager --enable ol7_optional_latest
yum-config-manager --enable ol7_developer
yum install -y wget unzip gettext libxml2-devel openssl-devel libcurl-devel pcre-devel libssh2-devel libconfig-devel
yum install -y MariaDB-client MariaDB-devel MariaDB-shared
yum install -y postgresql-devel postgresql-libs
~~~~

### 2. Скачать и распаковать свежую версию исходного кода:

~~~~
wget https://github.com/CHERTS/zabbix_dbmon/releases/download/v5.0.4/zabbix-5.0.4.tar.gz
tar -zxf zabbix-5.0.4.tar.gz
cd zabbix-5.0.4
~~~~

### 3. Сборка zabbix-agent с поддержкой (MariaDB) MySQL + PostgreSQL + Oracle 18c:

~~~~
./configure --with-openssl --with-libpthread --with-libpcre --with-libcurl --enable-dbmon --enable-dbmon-mysql --enable-dbmon-postgresql --enable-dbmon-oracle --with-oracle --with-oracle-lib=/u01/app/oracle/18c/dbhome_1/lib --with-oracle-include=/u01/app/oracle/18c/dbhome_1/rdbms/public --with-mysql --with-postgresql --enable-ipv6 --enable-agent --sysconfdir=/etc/zabbix
make
~~~~

### 4. После успешной сборки на шаге 3 можно использовать бинарные файлы zabbix, проверим факт наличия файла агента:

~~~~
ls -l src/zabbix_agent | grep -E 'zabbix_agentd$'
-rwxr-xr-x  1 root       root        2021176 Feb 19 21:17 zabbix_agentd
~~~~

### 5. Теперь Вы можете остановить zabbix-agent и заменить его данной сборкой, как правило это 2 команды:
~~~~
systemctl stop zabbix-agent
cp src/zabbix_agent/zabbix_agentd /sbin
~~~~

# Red Hat Enterprise Linux 8
## Сборка на Red Hat Enterprise Linux 8 с поддержкой MySQL (MariaDB) + PostgreSQL + Oracle 18c

### 1. Для подготовки к сборки на Red Hat Enterprise Linux 8 нужно установить дополнительные пакеты:

~~~~
dnf group install "Development Tools"
dnf install -y wget unzip gettext libxml2-devel openssl-devel libcurl-devel pcre-devel libconfig-devel
dnf install -y mariadb-devel postgresql-devel
~~~~

### 2. Скачать и распаковать свежую версию исходного кода:

~~~~
wget https://github.com/CHERTS/zabbix_dbmon/releases/download/v5.0.4/zabbix-5.0.4.tar.gz
tar -zxf zabbix-5.0.4.tar.gz
cd zabbix-5.0.4
~~~~

### 3. Сборка zabbix-agent с поддержкой (MariaDB) MySQL + PostgreSQL + Oracle 18c:

~~~~
./configure --with-openssl --with-libpthread --with-libpcre --with-libcurl --enable-dbmon --enable-dbmon-mysql --enable-dbmon-postgresql --enable-dbmon-oracle --with-oracle --with-oracle-lib=/u01/app/oracle/18c/dbhome_1/lib --with-oracle-include=/u01/app/oracle/18c/dbhome_1/rdbms/public --with-mysql --with-postgresql --enable-ipv6 --enable-agent --sysconfdir=/etc/zabbix
make
~~~~

### 4. После успешной сборки на шаге 3 можно использовать бинарные файлы zabbix, проверим факт наличия файла агента:

~~~~
ls -l src/zabbix_agent | grep -E 'zabbix_agentd$'
-rwxr-xr-x  1 root       root        2021176 Feb 19 21:17 zabbix_agentd
~~~~

### 5. Создание файла с переменными среды:

Создайте файл /etc/sysconfig/zabbix-agent-dbmon со следующим содержимым:

~~~~
ORACLE_HOME=/u01/app/oracle/18c/dbhome_1
ORACLE_SID=orcl
ORACLE_BASE=/u01/orabase
PATH=/u01/app/oracle/18c/dbhome_1/bin:/sbin:/bin:/usr/sbin:/usr/bin
LD_LIBRARY_PATH=/u01/app/oracle/18c/dbhome_1/lib
~~~~

### 6. Теперь Вы можете остановить zabbix-agent и заменить его данной сборкой, как правило это 2 команды:
~~~~
systemctl stop zabbix-agent
cp src/zabbix_agent/zabbix_agentd /sbin
~~~~
