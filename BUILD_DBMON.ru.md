# ��� ������� Zabbix-agent � ���������� ����������� ���� �� ������� �����������

[In English / ��-���������](BUILD.md)

[������ �� Oracle Linux 7 � ���������� MySQL (MariaDB) � PostgreSQL](#oracle-linux-7)

[������ �� Red Hat Enterprise Linux 8 � ���������� MySQL (MariaDB) � PostgreSQL](#red-hat-enterprise-linux-8)

[������ �� Ubuntu 18.04 LTS (Bionic Beaver) � ���������� MySQL (MariaDB) � PostgreSQL](#ubuntu)

# Oracle Linux 7
## ������ �� Oracle Linux 7 � ���������� MySQL (MariaDB) � PostgreSQL

### 1. ��� ���������� � ������ �� Oracle Linux 7 ����� ���������� �������������� ������:

~~~~
yum group install "Development Tools"
yum-config-manager --enable ol7_optional_latest
yum-config-manager --enable ol7_developer
yum install -y wget unzip gettext libxml2-devel openssl-devel libcurl-devel pcre-devel libssh2-devel
yum install -y MariaDB-client MariaDB-devel MariaDB-shared
yum install -y postgresql-devel postgresql-libs
~~~~

### 2. ������� � ����������� ������ ������ ��������� ����:

~~~~
wget https://github.com/CHERTS/zabbix_dbmon/releases/download/v4.4.5/zabbix-4.4.5.tar.gz
tar -zxf zabbix-4.4.5.tar.gz
cd zabbix-4.4.5
~~~~

### 3. ������ zabbix-agent � ���������� (MariaDB) MySQL � PostgreSQL:

~~~~
./configure --with-openssl --with-libpthread --with-libpcre --with-libcurl --enable-dbmon --enable-dbmon-mysql --enable-dbmon-postgresql --with-mysql --with-postgresql --enable-ipv6 --enable-agent --sysconfdir=/etc/zabbix
make
~~~~

### 4. ����� �������� ������ �� ���� 3 ����� ������������ �������� ����� zabbix, �������� ���� ������� ����� ������:

~~~~
ls -l src/zabbix_agent | grep -E 'zabbix_agentd$'
-rwxr-xr-x  1 root       root        2021176 Feb 19 21:17 zabbix_agentd_v4.4.5
~~~~

### 5. ������ �� ������ ���������� zabbix-agent � �������� ��� ������ �������, ��� ������� ��� 2 �������:
~~~~
systemctl stop zabbix-agent
cp src/zabbix_agent/zabbix_agentd /sbin
~~~~

# Red Hat Enterprise Linux 8
## ������ �� Red Hat Enterprise Linux 8 � ���������� MySQL (MariaDB) � PostgreSQL

### 1. ��� ���������� � ������ �� Red Hat Enterprise Linux 8 ����� ���������� �������������� ������:

~~~~
dnf group install "Development Tools"
dnf install -y wget unzip gettext libxml2-devel openssl-devel libcurl-devel pcre-devel
dnf install -y mariadb-devel
~~~~

### 2. ������� � ����������� ������ ������ ��������� ����:

~~~~
wget https://github.com/CHERTS/zabbix_dbmon/releases/download/v4.4.5/zabbix-4.4.5.tar.gz
tar -zxf zabbix-4.4.5.tar.gz
cd zabbix-4.4.5
~~~~

### 3. ������ zabbix-agent � ���������� (MariaDB) MySQL � PostgreSQL:

~~~~
./configure --with-openssl --with-libpthread --with-libpcre --with-libcurl --enable-dbmon --enable-dbmon-mysql --enable-dbmon-postgresql --with-mysql --with-postgresql --enable-ipv6 --enable-agent --sysconfdir=/etc/zabbix
make
~~~~

### 4. ����� �������� ������ �� ���� 3 ����� ������������ �������� ����� zabbix, �������� ���� ������� ����� ������:

~~~~
ls -l src/zabbix_agent | grep -E 'zabbix_agentd$'
-rwxr-xr-x  1 root       root        2021176 Feb 19 21:17 zabbix_agentd_v4.4.5
~~~~

### 5. ������ �� ������ ���������� zabbix-agent � �������� ��� ������ �������, ��� ������� ��� 2 �������:
~~~~
systemctl stop zabbix-agent
cp src/zabbix_agent/zabbix_agentd /sbin
~~~~

# Ubuntu
## ������ �� Ubuntu 18.04 LTS (Bionic Beaver) � ���������� MariaDB � PostgreSQL

### 1. ��� ���������� � ������ �� Ubuntu 18.04 ����� ���������� �������������� ������:

~~~~
sudo apt-get update
sudo apt-get install -y autoconf automake gcc make wget unzip gettext libxml2-dev libssl-dev libcurl4-openssl-dev libpcre2-dev libmariadbclient-dev-compat
~~~~

### 2. ������� � ����������� ������ ������ ��������� ����:

~~~~
wget https://github.com/CHERTS/zabbix_dbmon/releases/download/v4.4.5/zabbix-4.4.5.tar.gz
tar -zxf zabbix-4.4.5.tar.gz
cd zabbix-4.4.5
~~~~

### 3. ������ zabbix-agent � ���������� (MariaDB) MySQL � PostgreSQL:

~~~~
./configure --with-openssl --with-libpthread --with-libpcre --with-libcurl --enable-dbmon --enable-dbmon-mysql --enable-dbmon-postgresql --with-mysql --with-postgresql --enable-ipv6 --enable-agent --sysconfdir=/etc/zabbix
make
~~~~

### 4. ����� �������� ������ �� ���� 3 ����� ������������ �������� ����� zabbix, �������� ���� ������� ����� ������:

~~~~
ls -l src/zabbix_agent | grep -E 'zabbix_agentd$'
-rwxr-xr-x  1 root       root        2021176 Feb 19 21:17 zabbix_agentd_v4.4.5
~~~~

### 5. ������ �� ������ ���������� zabbix-agent � �������� ��� ������ �������, ��� ������� ��� 2 �������:
~~~~
systemctl stop zabbix-agent
cp src/zabbix_agent/zabbix_agentd /sbin
~~~~
