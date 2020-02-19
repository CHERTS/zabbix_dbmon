# Как собрать Zabbix-agent с поддержкой мониторинга СУБД из данного репозитария

[In English / По-английски](BUILD.md)

[Сборка на Oracle Linux 7 с поддержкой MySQL (MariaDB) и PostgreSQL](#oracle-linux-7)

[Сборка на Red Hat Enterprise Linux 8 с поддержкой MySQL (MariaDB) и PostgreSQL](#red-hat-enterprise-linux-8)

[Сборка на Ubuntu 18.04 LTS (Bionic Beaver) с поддержкой MySQL (MariaDB) и PostgreSQL](#ubuntu)

# Oracle Linux 7
## Сборка на Oracle Linux 7 с поддержкой MySQL (MariaDB) и PostgreSQL

### 1. Для подготовки к сборки на Oracle Linux 7 нужно установить дополнительные пакеты:

~~~~
yum group install "Development Tools"
yum-config-manager --enable ol7_optional_latest
yum-config-manager --enable ol7_developer
yum install -y wget unzip gettext libxml2-devel openssl-devel libcurl-devel pcre-devel libssh2-devel
yum install -y MariaDB-client MariaDB-devel MariaDB-shared
yum install -y postgresql-devel postgresql-libs
~~~~

### 2. Скачать и распаковать свежую версию исходного кода:

~~~~
wget https://github.com/CHERTS/zabbix_dbmon/releases/download/v4.4.5/zabbix-4.4.5.tar.gz
tar -zxf zabbix-4.4.5.tar.gz
cd zabbix-4.4.5
~~~~

### 3. Сборка zabbix-agent с поддержкой (MariaDB) MySQL и PostgreSQL:

~~~~
./configure --with-openssl --with-libpthread --with-libpcre --with-libcurl --enable-dbmon --enable-dbmon-mysql --enable-dbmon-postgresql --with-mysql --with-postgresql --enable-ipv6 --enable-agent --sysconfdir=/etc/zabbix
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
## Сборка на Red Hat Enterprise Linux 8 с поддержкой MySQL (MariaDB) и PostgreSQL

### 1. Для подготовки к сборки на Red Hat Enterprise Linux 8 нужно установить дополнительные пакеты:

~~~~
dnf group install "Development Tools"
dnf install -y wget unzip gettext libxml2-devel openssl-devel libcurl-devel pcre-devel
dnf install -y mariadb-devel
~~~~

### 2. Скачать и распаковать свежую версию исходного кода:

~~~~
wget https://github.com/CHERTS/zabbix_dbmon/releases/download/v4.4.5/zabbix-4.4.5.tar.gz
tar -zxf zabbix-4.4.5.tar.gz
cd zabbix-4.4.5
~~~~

### 3. Сборка zabbix-agent с поддержкой (MariaDB) MySQL и PostgreSQL:

~~~~
./configure --with-openssl --with-libpthread --with-libpcre --with-libcurl --enable-dbmon --enable-dbmon-mysql --enable-dbmon-postgresql --with-mysql --with-postgresql --enable-ipv6 --enable-agent --sysconfdir=/etc/zabbix
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

# Ubuntu
## Сборка на Ubuntu 18.04 LTS (Bionic Beaver) с поддержкой MariaDB и PostgreSQL

### 1. Для подготовки к сборки на Ubuntu 18.04 нужно установить дополнительные пакеты:

~~~~
sudo apt-get update
sudo apt-get install -y autoconf automake gcc make wget unzip gettext libxml2-dev libssl-dev libcurl4-openssl-dev libpcre2-dev libmariadbclient-dev-compat
~~~~

### 2. Скачать и распаковать свежую версию исходного кода:

~~~~
wget https://github.com/CHERTS/zabbix_dbmon/releases/download/v4.4.5/zabbix-4.4.5.tar.gz
tar -zxf zabbix-4.4.5.tar.gz
cd zabbix-4.4.5
~~~~

### 3. Сборка zabbix-agent с поддержкой (MariaDB) MySQL и PostgreSQL:

~~~~
./configure --with-openssl --with-libpthread --with-libpcre --with-libcurl --enable-dbmon --enable-dbmon-mysql --enable-dbmon-postgresql --with-mysql --with-postgresql --enable-ipv6 --enable-agent --sysconfdir=/etc/zabbix
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
