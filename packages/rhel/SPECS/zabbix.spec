Name:		zabbix
Version:	4.4.6
Release:	%{?alphatag:0.}3%{?alphatag}%{?dist}
Summary:	The Enterprise-class open source monitoring solution
Group:		Applications/Internet
License:	GPLv2+
URL:		http://www.zabbix.com/
Source0:	%{name}-%{version}%{?alphatag}.tar.gz
Source1:	zabbix-web22.conf
Source2:	zabbix-web24.conf
Source3:	zabbix-logrotate.in
Source4:	zabbix-java-gateway.init
Source5:	zabbix-agent.init
Source6:	zabbix-server.init
Source7:	zabbix-proxy.init
Source10:	zabbix-agent.service
Source11:	zabbix-server.service
Source12:	zabbix-proxy.service
Source13:	zabbix-java-gateway.service
Source14:	zabbix_java_gateway-sysd
Source15:	zabbix-tmpfiles.conf
Source16:	zabbix-php-fpm.conf
Source17:	zabbix-web-fcgi.conf
Source18:	zabbix-nginx.conf
Source19:	zabbix-agent2.service
Source20:	zabbix-agent.sysconfig
Source21:	zabbix-agent-dbmon.service
Source22:	zabbix-agent-dbmon.init
Patch0:		config.patch
Patch1:		fping3-sourceip-option.patch
Patch2:		ZBXNEXT-435.patch
Patch3:		ZBXNEXT-5554.patch
Patch4:		DBS-001.patch
Patch5:		DBS-002.patch
Patch6:		DBS-003.patch

Buildroot:	%{_tmppath}/zabbix-%{version}-%{release}-root-%(%{__id_u} -n)

%ifarch x86_64
%if 0%{?rhel} >= 8
%define build_agent2 1
%endif
%endif

# FIXME: Building debuginfo is broken on RHEL-8. Disabled for now.
%if 0%{?rhel} == 8
%define debug_package %{nil}
%endif

%if 0%{?rhel} >= 6
%define build_server 1
%endif

BuildRequires:	mysql-devel
BuildRequires:	postgresql-devel
BuildRequires:	net-snmp-devel
BuildRequires:	openldap-devel
BuildRequires:	gnutls-devel
BuildRequires:	sqlite-devel
BuildRequires:	unixODBC-devel
BuildRequires:	curl-devel >= 7.13.1
BuildRequires:	OpenIPMI-devel >= 2
%if 0%{?rhel} >= 8
BuildRequires: libssh-devel >= 0.9.0
%else
BuildRequires: libssh2-devel >= 1.0.0
%endif
BuildRequires:	java-devel >= 1.6.0
BuildRequires:	libxml2-devel
BuildRequires:	pcre-devel
BuildRequires:	libevent-devel
%if 0%{?rhel} >= 6
BuildRequires:	openssl-devel >= 1.0.1
%endif
%if 0%{?rhel} >= 7
BuildRequires:	systemd
%endif
BuildRequires:	libconfig-devel

%description
Zabbix is the ultimate enterprise-level software designed for
real-time monitoring of millions of metrics collected from tens of
thousands of servers, virtual machines and network devices.

%package agent
Summary:		Old Zabbix Agent
Group:			Applications/Internet
Requires:		logrotate
Requires(pre):		/usr/sbin/useradd
%if 0%{?rhel} >= 7
Requires(post):		systemd
Requires(preun):	systemd
Requires(preun):	systemd
%else
Requires(post):		/sbin/chkconfig
Requires(preun):	/sbin/chkconfig
Requires(preun):	/sbin/service
Requires(postun):	/sbin/service
%endif
Obsoletes:		zabbix

%if 0%{?build_agent2} != 1
%description agent
Zabbix agent to be installed on monitored systems.

%else
%description agent
Old implementation of zabbix agent.
To be installed on monitored systems.

%package agent2
Summary:		New Zabbix Agent
Group:			Applications/Internet
Requires:		logrotate
Requires(post):		systemd
Requires(preun):	systemd
Requires(preun):	systemd
Obsoletes:		zabbix

%description agent2
New implementation of zabbix agent.
To be installed on monitored systems.
%endif

%package agent-dbmon
Summary:       Zabbix Agent for Database monitoring
Group:         Applications/Internet
Requires:      logrotate
Requires(pre):      /usr/sbin/useradd
%if 0%{?rhel} >= 7
Requires(post):     systemd
Requires(preun):    systemd
Requires(preun):    systemd
%else
Requires(post):     /sbin/chkconfig
Requires(preun):    /sbin/chkconfig
Requires(preun):    /sbin/service
Requires(postun):   /sbin/service
%endif
Obsoletes:          zabbix

%description agent-dbmon
Zabbix agent for Database monitoring.

%package get
Summary:		Zabbix Get
Group:			Applications/Internet

%description get
Zabbix get command line utility.

%package sender
Summary:		Zabbix Sender
Group:			Applications/Internet

%description sender
Zabbix sender command line utility.

%package js
Summary:		Zabbix JS
Group:			Applications/Internet

%description js
Zabbix js command line utility.

%package proxy-mysql
Summary:		Zabbix proxy for MySQL or MariaDB database
Group:			Applications/Internet
Requires:		fping
%if 0%{?rhel} >= 7
Requires(post):		systemd
Requires(preun):	systemd
Requires(postun):	systemd
%else
Requires(post):		/sbin/chkconfig
Requires(preun):	/sbin/chkconfig
Requires(preun):	/sbin/service
Requires(postun):	/sbin/service
%endif
Provides:		zabbix-proxy = %{version}-%{release}
Provides:		zabbix-proxy-implementation = %{version}-%{release}
Obsoletes:		zabbix
Obsoletes:		zabbix-proxy

%description proxy-mysql
Zabbix proxy with MySQL or MariaDB database support.

%package proxy-pgsql
Summary:		Zabbix proxy for PostgreSQL database
Group:			Applications/Internet
Requires:		fping
%if 0%{?rhel} >= 7
Requires(post):		systemd
Requires(preun):	systemd
Requires(postun):	systemd
%else
Requires(post):		/sbin/chkconfig
Requires(preun):	/sbin/chkconfig
Requires(preun):	/sbin/service
Requires(postun):	/sbin/service
%endif
Provides:		zabbix-proxy = %{version}-%{release}
Provides:		zabbix-proxy-implementation = %{version}-%{release}
Obsoletes:		zabbix
Obsoletes:		zabbix-proxy

%description proxy-pgsql
Zabbix proxy with PostgreSQL database support.

%package proxy-sqlite3
Summary:		Zabbix proxy for SQLite3 database
Group:			Applications/Internet
Requires:		fping
%if 0%{?rhel} >= 7
Requires(post):		systemd
Requires(preun):	systemd
Requires(postun):	systemd
%else
Requires(post):		/sbin/chkconfig
Requires(preun):	/sbin/chkconfig
Requires(preun):	/sbin/service
Requires(postun):	/sbin/service
%endif
Provides:		zabbix-proxy = %{version}-%{release}
Provides:		zabbix-proxy-implementation = %{version}-%{release}
Obsoletes:		zabbix
Obsoletes:		zabbix-proxy

%description proxy-sqlite3
Zabbix proxy with SQLite3 database support.

%package java-gateway
Summary:		Zabbix java gateway
Group:			Applications/Internet
%if 0%{?rhel} >= 7
Requires:		java-headless >= 1.6.0
%else
Requires:		java >= 1.6.0
%endif
%if 0%{?rhel} >= 7
Requires(post):		systemd
Requires(preun):	systemd
Requires(postun):	systemd
%else
Requires(post):		/sbin/chkconfig
Requires(preun):	/sbin/chkconfig
Requires(preun):	/sbin/service
Requires(postun):	/sbin/service
%endif
Obsoletes:			zabbix

%description java-gateway
Zabbix java gateway

%if 0%{?build_server}
%package server-mysql
Summary:		Zabbix server for MySQL or MariaDB database
Group:			Applications/Internet
Requires:		fping
%if 0%{?rhel} >= 7
Requires(post):		systemd
Requires(preun):	systemd
Requires(postun):	systemd
%else
Requires(post):		/sbin/chkconfig
Requires(preun):	/sbin/chkconfig
Requires(preun):	/sbin/service
Requires(postun):	/sbin/service
%endif
Provides:		zabbix-server = %{version}-%{release}
Provides:		zabbix-server-implementation = %{version}-%{release}
Obsoletes:		zabbix
Obsoletes:		zabbix-server

%description server-mysql
Zabbix server with MySQL or MariaDB database support.

%package server-pgsql
Summary:		Zabbix server for PostgresSQL database
Group:			Applications/Internet
Requires:		fping
%if 0%{?rhel} >= 7
Requires(post):		systemd
Requires(preun):	systemd
Requires(postun):	systemd
%else
Requires(post):		/sbin/chkconfig
Requires(preun):	/sbin/chkconfig
Requires(preun):	/sbin/service
Requires(postun):	/sbin/service
%endif
Provides:		zabbix-server = %{version}-%{release}
Provides:		zabbix-server-implementation = %{version}-%{release}
Obsoletes:		zabbix
Obsoletes:		zabbix-server
%description server-pgsql
Zabbix server with PostgresSQL database support.

%package web
Summary:		Zabbix web frontend common package
Group:			Application/Internet
BuildArch:		noarch
%if 0%{?rhel} == 7
Requires:		httpd
Requires:		php >= 5.4
%endif
%if 0%{?rhel} >= 7
Requires:		php-gd
Requires:		php-bcmath
Requires:		php-mbstring
Requires:		php-xml
Requires:		php-ldap
%endif
%if 0%{?rhel} >= 8
Requires:		php-fpm
Requires:		php-json
%endif
Requires:		dejavu-sans-fonts
Requires:		zabbix-web-database = %{version}-%{release}
Requires(post):		%{_sbindir}/update-alternatives
Requires(preun):	%{_sbindir}/update-alternatives

%description web
Zabbix web frontend common package

%package web-mysql
Summary:		Zabbix web frontend for MySQL
Group:			Applications/Internet
BuildArch:		noarch
%if 0%{?rhel} == 7
Requires:		php-mysql
%endif
%if 0%{?rhel} == 8
Requires:		php-mysqlnd
%endif
Requires:		zabbix-web = %{version}-%{release}
Provides:		zabbix-web-database = %{version}-%{release}
%if 0%{?rhel} >= 8
Suggests:		zabbix-apache-conf
Suggests:		zabbix-nginx-conf
%endif

%description web-mysql
Zabbix web frontend for MySQL

%package web-pgsql
Summary:		Zabbix web frontend for PostgreSQL
Group:			Applications/Internet
BuildArch:		noarch
%if 0%{?rhel} >= 7
Requires:		php-pgsql
%endif
Requires:		zabbix-web = %{version}-%{release}
Provides:		zabbix-web-database = %{version}-%{release}

%description web-pgsql
Zabbix web frontend for PostgreSQL

%package web-japanese
Summary:		Japanese font settings for Zabbix frontend
Group:			Applications/Internet
BuildArch:		noarch
%if 0%{?rhel} >= 8
Requires:		google-noto-sans-cjk-ttc-fonts
Requires:		glibc-langpack-ja
%else
Requires:		vlgothic-p-fonts
%endif
Requires:		zabbix-web = %{version}-%{release}
Requires(post):		%{_sbindir}/update-alternatives
Requires(preun):	%{_sbindir}/update-alternatives

%description web-japanese
Japanese font configuration for Zabbix web frontend

%if 0%{?rhel} >= 8
%package apache-conf
Summary:		Automatic zabbix frontend configuration with apache
Group:			Applications/Internet
BuildArch:		noarch
Requires:		zabbix-web
Requires:		httpd
Requires:		php >= 5.4

%description apache-conf
Automatic zabbix frontend configuration with apache
%endif

%if 0%{?rhel} >= 7
%package nginx-conf
Summary:		Automatic zabbix frontend configuration with nginx and php-fpm
Group:			Applications/Internet
BuildArch:		noarch
Requires:		zabbix-web
Requires:		nginx
Requires:		php-fpm

%description nginx-conf
Automatic zabbix frontend configuration with nginx and php-fpm
%endif

%endif

%prep
%setup0 -q -n %{name}-%{version}%{?alphatag}
%patch0 -p1
%if 0%{?rhel} >= 7
%patch1 -p1
%endif
%patch2 -p1
%patch3 -p1
%patch4 -p1
%patch5 -p1
%patch6 -p1

## remove font file
rm -f frontends/php/assets/fonts/DejaVuSans.ttf

# replace font in defines.inc.php
sed -i -r "s/(define\(.*_FONT_NAME.*)DejaVuSans/\1graphfont/" \
	frontends/php/include/defines.inc.php

# remove .htaccess files
rm -f frontends/php/app/.htaccess
rm -f frontends/php/conf/.htaccess
rm -f frontends/php/include/.htaccess
rm -f frontends/php/local/.htaccess

# remove translation source files and scripts
find frontends/php/locale -name '*.po' | xargs rm -f
find frontends/php/locale -name '*.sh' | xargs rm -f

# traceroute command path for global script
sed -i -e 's|/usr/bin/traceroute|/bin/traceroute|' database/mysql/data.sql
sed -i -e 's|/usr/bin/traceroute|/bin/traceroute|' database/postgresql/data.sql
sed -i -e 's|/usr/bin/traceroute|/bin/traceroute|' database/sqlite3/data.sql

# change log directory for Java Gateway
sed -i -e 's|/tmp/zabbix_java.log|/var/log/zabbix/zabbix_java_gateway.log|g' src/zabbix_java/lib/logback.xml

%if 0%{?build_server}
# copy sql files for servers
cat database/mysql/schema.sql > database/mysql/create.sql
cat database/mysql/images.sql >> database/mysql/create.sql
cat database/mysql/data.sql >> database/mysql/create.sql
gzip database/mysql/create.sql

cat database/postgresql/schema.sql > database/postgresql/create.sql
cat database/postgresql/images.sql >> database/postgresql/create.sql
cat database/postgresql/data.sql >> database/postgresql/create.sql
gzip database/postgresql/create.sql
gzip database/postgresql/timescaledb.sql
%endif

# sql files for proxyes
gzip database/mysql/schema.sql
gzip database/postgresql/schema.sql
gzip database/sqlite3/schema.sql

%build

build_flags="
	--enable-dependency-tracking
	--sysconfdir=/etc/zabbix
	--libdir=%{_libdir}/zabbix
	--enable-agent
%if 0%{?build_agent2}
	--enable-agent2
%endif
	--enable-proxy
	--enable-ipv6
	--enable-java
	--with-net-snmp
	--with-ldap
	--with-libcurl
	--with-openipmi
	--with-unixodbc
%if 0%{?rhel} >= 8
     --with-ssh
%else
     --with-ssh2
%endif
	--with-libxml2
	--with-libevent
	--with-libpcre
"

build_dbmon_flags="
	--enable-dependency-tracking
	--sysconfdir=/etc/zabbix
	--libdir=%{_libdir}/zabbix
	--enable-agent
	--enable-dbmon
	--enable-dbmon-mysql
	--enable-dbmon-postgresql
	--enable-ipv6
	--with-libcurl
	--with-libpcre
	--with-libpthread
	--with-mysql
	--with-postgresql
"

%if 0%{?rhel} >= 6
build_flags="$build_flags --with-openssl"
build_dbmon_flags="$build_dbmon_flags --with-openssl"
%endif

%configure $build_dbmon_flags
make %{?_smp_mflags}
mv src/zabbix_agent/zabbix_agentd src/zabbix_agent/zabbix_agentd_dbmon

%configure $build_flags --with-sqlite3
make %{?_smp_mflags}
mv src/zabbix_proxy/zabbix_proxy src/zabbix_proxy/zabbix_proxy_sqlite3

%if 0%{?build_server}
build_flags="$build_flags --enable-server"
%endif

%configure $build_flags --with-mysql
make %{?_smp_mflags}
%if 0%{?build_server}
mv src/zabbix_server/zabbix_server src/zabbix_server/zabbix_server_mysql
%endif
mv src/zabbix_proxy/zabbix_proxy src/zabbix_proxy/zabbix_proxy_mysql

%configure $build_flags --with-postgresql
make %{?_smp_mflags}
%if 0%{?build_server}
mv src/zabbix_server/zabbix_server src/zabbix_server/zabbix_server_pgsql
%endif
mv src/zabbix_proxy/zabbix_proxy src/zabbix_proxy/zabbix_proxy_pgsql

%if 0%{?build_server}
touch src/zabbix_server/zabbix_server
%endif
touch src/zabbix_proxy/zabbix_proxy

%install

rm -rf $RPM_BUILD_ROOT

# install
%if 0%{?build_agent2}
make DESTDIR=$RPM_BUILD_ROOT GOBIN=$RPM_BUILD_ROOT%{_sbindir} install
%else
make DESTDIR=$RPM_BUILD_ROOT install
%endif

# install necessary directories
mkdir -p $RPM_BUILD_ROOT%{_localstatedir}/log/zabbix
mkdir -p $RPM_BUILD_ROOT%{_localstatedir}/run/zabbix

# install zabbix_agentd_dbmon
install -m 0755 -p src/zabbix_agent/zabbix_agentd_dbmon $RPM_BUILD_ROOT%{_sbindir}/

# install server and proxy binaries
%if 0%{?build_server}
install -m 0755 -p src/zabbix_server/zabbix_server_* $RPM_BUILD_ROOT%{_sbindir}/
rm $RPM_BUILD_ROOT%{_sbindir}/zabbix_server
%endif
install -m 0755 -p src/zabbix_proxy/zabbix_proxy_* $RPM_BUILD_ROOT%{_sbindir}/
rm $RPM_BUILD_ROOT%{_sbindir}/zabbix_proxy

# delete unnecessary files from java gateway
rm $RPM_BUILD_ROOT%{_sbindir}/zabbix_java/settings.sh
rm $RPM_BUILD_ROOT%{_sbindir}/zabbix_java/startup.sh
rm $RPM_BUILD_ROOT%{_sbindir}/zabbix_java/shutdown.sh

# install scripts and modules directories
mkdir -p $RPM_BUILD_ROOT/usr/lib/zabbix
%if 0%{?build_server}
mv $RPM_BUILD_ROOT%{_datadir}/zabbix/alertscripts $RPM_BUILD_ROOT/usr/lib/zabbix
%endif
mv $RPM_BUILD_ROOT%{_datadir}/zabbix/externalscripts $RPM_BUILD_ROOT/usr/lib/zabbix

%if 0%{?rhel} >= 7
mv $RPM_BUILD_ROOT%{_sbindir}/zabbix_java/lib/logback.xml $RPM_BUILD_ROOT/%{_sysconfdir}/zabbix/zabbix_java_gateway_logback.xml
rm $RPM_BUILD_ROOT%{_sbindir}/zabbix_java/lib/logback-console.xml
mv $RPM_BUILD_ROOT%{_sbindir}/zabbix_java $RPM_BUILD_ROOT/%{_datadir}/zabbix-java-gateway
install -m 0755 -p %{SOURCE14} $RPM_BUILD_ROOT%{_sbindir}/zabbix_java_gateway
%endif

%if 0%{?build_server}
# install frontend files
find frontends/php -name '*.orig' | xargs rm -f
cp -a frontends/php/* $RPM_BUILD_ROOT%{_datadir}/zabbix

# install frontend configuration files
mkdir -p $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/web
touch $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/web/zabbix.conf.php
mv $RPM_BUILD_ROOT%{_datadir}/zabbix/conf/maintenance.inc.php $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/web/

# drop config files in place
%if 0%{?rhel} >= 7
install -Dm 0644 -p %{SOURCE16} $RPM_BUILD_ROOT%{_sysconfdir}/php-fpm.d/zabbix.conf
install -Dm 0644 -p %{SOURCE18} $RPM_BUILD_ROOT%{_sysconfdir}/nginx/conf.d/zabbix.conf
%else
install -Dm 0644 -p %{SOURCE2} conf/httpd24-example.conf
install -Dm 0644 -p %{SOURCE1} conf/httpd22-example.conf
%endif

%if 0%{?rhel} == 7
install -Dm 0644 -p %{SOURCE2} $RPM_BUILD_ROOT%{_sysconfdir}/httpd/conf.d/zabbix.conf
%endif

%if 0%{?rhel} == 8
install -Dm 0644 -p %{SOURCE17} $RPM_BUILD_ROOT%{_sysconfdir}/httpd/conf.d/zabbix.conf
install -Dm 0644 -p %{SOURCE2} conf/httpd24-example.conf
%endif

%endif
#build_server

# install configuration files
mv $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_agentd.conf.d $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_agentd.d
mkdir $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_agentd_dbmon.d
%if 0%{?build_agent2}
mkdir $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_agent2.d
%endif
mv $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_proxy.conf.d $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_proxy.d
%if 0%{?build_server}
mv $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_server.conf.d $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_server.d
%endif

install -dm 755 $RPM_BUILD_ROOT%{_docdir}/zabbix-agent-%{version}
install -m 0644 conf/zabbix_agentd_dbmon/userparameter_dbmon.conf $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_agentd_dbmon.d
install -Dm 0755 conf/zabbix_agentd_dbmon/dbmon.sh $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_agentd_dbmon.d
install -m 0644 conf/zabbix_agentd_dbmon_sql.conf $RPM_BUILD_ROOT%{_sysconfdir}/zabbix

cat conf/zabbix_agentd.conf | sed \
	-e '/^# PidFile=/a \\nPidFile=%{_localstatedir}/run/zabbix/zabbix_agentd.pid' \
	-e 's|^LogFile=.*|LogFile=%{_localstatedir}/log/zabbix/zabbix_agentd.log|g' \
	-e '/^# LogFileSize=.*/a \\nLogFileSize=0' \
	-e '/^# Include=$/a \\nInclude=%{_sysconfdir}/zabbix/zabbix_agentd.d/*.conf' \
	> $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_agentd.conf

cat conf/zabbix_agentd_dbmon.conf | sed \
	-e '/^# PidFile=/a \\nPidFile=%{_localstatedir}/run/zabbix/zabbix_agentd_dbmon.pid' \
	-e 's|^LogFile=.*|LogFile=%{_localstatedir}/log/zabbix/zabbix_agentd_dbmon.log|g' \
	-e '/^# LogFileSize=.*/a \\nLogFileSize=5' \
	-e '/^# Include=$/a \\nInclude=%{_sysconfdir}/zabbix/zabbix_agentd_dbmon.d/*.conf' \
	> $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_agentd_dbmon.conf
cp man/zabbix_agentd_dbmon.man $RPM_BUILD_ROOT%{_mandir}/man8/zabbix_agentd_dbmon.8

%if 0%{?build_agent2}
cat src/go/conf/zabbix_agent2.conf | sed \
	-e '/^# PidFile=/a \\nPidFile=%{_localstatedir}/run/zabbix/zabbix_agent2.pid' \
	-e 's|^LogFile=.*|LogFile=%{_localstatedir}/log/zabbix/zabbix_agent2.log|g' \
	-e '/^# LogFileSize=.*/a \\nLogFileSize=0' \
	-e '/^# Include=$/a \\nInclude=%{_sysconfdir}/zabbix/zabbix_agent2.d/*.conf' \
	> $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_agent2.conf

cp man/zabbix_agent2.man $RPM_BUILD_ROOT%{_mandir}/man8/zabbix_agent2.8
%endif

%if 0%{?build_server}
cat conf/zabbix_server.conf | sed \
	-e '/^# PidFile=/a \\nPidFile=%{_localstatedir}/run/zabbix/zabbix_server.pid' \
	-e 's|^LogFile=.*|LogFile=%{_localstatedir}/log/zabbix/zabbix_server.log|g' \
	-e '/^# LogFileSize=/a \\nLogFileSize=0' \
	-e '/^# AlertScriptsPath=/a \\nAlertScriptsPath=/usr/lib/zabbix/alertscripts' \
	-e '/^# ExternalScripts=/a \\nExternalScripts=/usr/lib/zabbix/externalscripts' \
	-e 's|^DBUser=root|DBUser=zabbix|g' \
	-e '/^# SNMPTrapperFile=.*/a \\nSNMPTrapperFile=/var/log/snmptrap/snmptrap.log' \
	-e '/^# SocketDir=.*/a \\nSocketDir=/var/run/zabbix' \
	> $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_server.conf
%endif

cat conf/zabbix_proxy.conf | sed \
	-e '/^# PidFile=/a \\nPidFile=%{_localstatedir}/run/zabbix/zabbix_proxy.pid' \
	-e 's|^LogFile=.*|LogFile=%{_localstatedir}/log/zabbix/zabbix_proxy.log|g' \
	-e '/^# LogFileSize=/a \\nLogFileSize=0' \
	-e '/^# ExternalScripts=/a \\nExternalScripts=/usr/lib/zabbix/externalscripts' \
	-e 's|^DBUser=root|DBUser=zabbix|g' \
	-e '/^# SNMPTrapperFile=.*/a \\nSNMPTrapperFile=/var/log/snmptrap/snmptrap.log' \
	-e '/^# SocketDir=.*/a \\nSocketDir=/var/run/zabbix' \
	> $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_proxy.conf

cat src/zabbix_java/settings.sh | sed \
	-e 's|^PID_FILE=.*|PID_FILE="/var/run/zabbix/zabbix_java.pid"|g' \
	> $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_java_gateway.conf

# install logrotate configuration files
mkdir -p $RPM_BUILD_ROOT%{_sysconfdir}/logrotate.d
%if 0%{?build_server}
cat %{SOURCE3} | sed \
	-e 's|COMPONENT|server|g' \
	> $RPM_BUILD_ROOT%{_sysconfdir}/logrotate.d/zabbix-server
%endif
cat %{SOURCE3} | sed \
	-e 's|COMPONENT|agentd|g' \
	> $RPM_BUILD_ROOT%{_sysconfdir}/logrotate.d/zabbix-agent
cat %{SOURCE3} | sed \
	-e 's|COMPONENT|agentd-dbmon|g' \
	> $RPM_BUILD_ROOT%{_sysconfdir}/logrotate.d/zabbix-agent-dbmon
%if 0%{?build_agent2}
cat %{SOURCE3} | sed \
	-e 's|COMPONENT|agent2|g' \
	> $RPM_BUILD_ROOT%{_sysconfdir}/logrotate.d/zabbix-agent2
%endif
cat %{SOURCE3} | sed \
	-e 's|COMPONENT|proxy|g' \
	> $RPM_BUILD_ROOT%{_sysconfdir}/logrotate.d/zabbix-proxy

# install startup scripts
%if 0%{?rhel} >= 7
install -Dm 0644 -p %{SOURCE10} $RPM_BUILD_ROOT%{_unitdir}/zabbix-agent.service
install -Dm 0644 -p %{SOURCE21} $RPM_BUILD_ROOT%{_unitdir}/zabbix-agent-dbmon.service
%if 0%{?build_server}
install -Dm 0644 -p %{SOURCE11} $RPM_BUILD_ROOT%{_unitdir}/zabbix-server.service
%endif
install -Dm 0644 -p %{SOURCE12} $RPM_BUILD_ROOT%{_unitdir}/zabbix-proxy.service
install -Dm 0644 -p %{SOURCE13} $RPM_BUILD_ROOT%{_unitdir}/zabbix-java-gateway.service
%else
install -Dm 0755 -p %{SOURCE4} $RPM_BUILD_ROOT%{_sysconfdir}/init.d/zabbix-java-gateway
install -Dm 0755 -p %{SOURCE5} $RPM_BUILD_ROOT%{_sysconfdir}/init.d/zabbix-agent
install -Dm 0755 -p %{SOURCE22} $RPM_BUILD_ROOT%{_sysconfdir}/init.d/zabbix-agent-dbmon
%if 0%{?build_server}
install -Dm 0755 -p %{SOURCE6} $RPM_BUILD_ROOT%{_sysconfdir}/init.d/zabbix-server
%endif
install -Dm 0755 -p %{SOURCE7} $RPM_BUILD_ROOT%{_sysconfdir}/init.d/zabbix-proxy
%endif

%if 0%{?build_agent2}
install -Dm 0644 -p %{SOURCE19} $RPM_BUILD_ROOT%{_unitdir}/zabbix-agent2.service
%endif

%if 0%{?rhel} <= 6
install -Dm 0644 -p %{SOURCE20} $RPM_BUILD_ROOT%{_sysconfdir}/sysconfig/zabbix-agent
%endif

# install systemd-tmpfiles conf
%if 0%{?rhel} >= 7
install -Dm 0644 -p %{SOURCE15} $RPM_BUILD_ROOT%{_prefix}/lib/tmpfiles.d/zabbix-agent.conf
install -Dm 0644 -p %{SOURCE15} $RPM_BUILD_ROOT%{_prefix}/lib/tmpfiles.d/zabbix-agent-dbmon.conf
%if 0%{?build_server}
install -Dm 0644 -p %{SOURCE15} $RPM_BUILD_ROOT%{_prefix}/lib/tmpfiles.d/zabbix-server.conf
%endif
install -Dm 0644 -p %{SOURCE15} $RPM_BUILD_ROOT%{_prefix}/lib/tmpfiles.d/zabbix-proxy.conf
install -Dm 0644 -p %{SOURCE15} $RPM_BUILD_ROOT%{_prefix}/lib/tmpfiles.d/zabbix-java-gateway.conf
%endif

%if 0%{?build_agent2}
install -Dm 0644 -p %{SOURCE15} $RPM_BUILD_ROOT%{_prefix}/lib/tmpfiles.d/zabbix_agent2.conf
%endif

%clean
rm -rf $RPM_BUILD_ROOT

%pre agent
getent group zabbix > /dev/null || groupadd -r zabbix
getent passwd zabbix > /dev/null || \
	useradd -r -g zabbix -d %{_localstatedir}/lib/zabbix -s /sbin/nologin \
	-c "Zabbix Monitoring System" zabbix
:

%post agent
%if 0%{?rhel} >= 7
%systemd_post zabbix-agent.service
%else
/sbin/chkconfig --add zabbix-agent || :
%endif

%posttrans agent
# preserve old userparameter_mysql.conf file during upgrade
if [ -f %{_sysconfdir}/zabbix/zabbix_agentd.d/userparameter_mysql.conf.rpmsave ] && [ ! -f %{_sysconfdir}/zabbix/zabbix_agentd.d/userparameter_mysql.conf ]; then
	cp -vn %{_sysconfdir}/zabbix/zabbix_agentd.d/userparameter_mysql.conf.rpmsave %{_sysconfdir}/zabbix/zabbix_agentd.d/userparameter_mysql.conf
fi
:

%post agent-dbmon
%if 0%{?rhel} >= 7
%systemd_post zabbix-agent-dbmon.service
%else
/sbin/chkconfig --add zabbix-agent-dbmon || :
%endif
:

%posttrans agent-dbmon
# preserve old userparameter_dbmon.conf file during upgrade
if [ -f %{_sysconfdir}/zabbix/zabbix_agentd_dbmon.d/userparameter_dbmon.conf.rpmsave ] && [ ! -f %{_sysconfdir}/zabbix/zabbix_agentd_dbmon.d/userparameter_dbmon.conf ]; then
        cp -vn %{_sysconfdir}/zabbix/zabbix_agentd_dbmon.d/userparameter_dbmon.conf.rpmsave %{_sysconfdir}/zabbix/zabbix_agentd_dbmon.d/userparameter_dbmon.conf
fi
:

%if 0%{?build_agent2}
%pre agent2
getent group zabbix > /dev/null || groupadd -r zabbix
getent passwd zabbix > /dev/null || \
	useradd -r -g zabbix -d %{_localstatedir}/lib/zabbix -s /sbin/nologin \
	-c "Zabbix Monitoring System" zabbix
:

%post agent2
%systemd_post zabbix-agent2.service
:
%endif

%pre proxy-mysql
getent group zabbix > /dev/null || groupadd -r zabbix
getent passwd zabbix > /dev/null || \
	useradd -r -g zabbix -d %{_localstatedir}/lib/zabbix -s /sbin/nologin \
	-c "Zabbix Monitoring System" zabbix
:

%pre proxy-pgsql
getent group zabbix > /dev/null || groupadd -r zabbix
getent passwd zabbix > /dev/null || \
	useradd -r -g zabbix -d %{_localstatedir}/lib/zabbix -s /sbin/nologin \
	-c "Zabbix Monitoring System" zabbix
:

%pre proxy-sqlite3
getent group zabbix > /dev/null || groupadd -r zabbix
getent passwd zabbix > /dev/null || \
	useradd -r -g zabbix -d %{_localstatedir}/lib/zabbix -s /sbin/nologin \
	-c "Zabbix Monitoring System" zabbix
:

%pre java-gateway
getent group zabbix > /dev/null || groupadd -r zabbix
getent passwd zabbix > /dev/null || \
	useradd -r -g zabbix -d %{_localstatedir}/lib/zabbix -s /sbin/nologin \
	-c "Zabbix Monitoring System" zabbix
:

%if 0%{?build_server}
%pre server-mysql
getent group zabbix > /dev/null || groupadd -r zabbix
getent passwd zabbix > /dev/null || \
	useradd -r -g zabbix -d %{_localstatedir}/lib/zabbix -s /sbin/nologin \
	-c "Zabbix Monitoring System" zabbix
:

%pre server-pgsql
getent group zabbix > /dev/null || groupadd -r zabbix
getent passwd zabbix > /dev/null || \
	useradd -r -g zabbix -d %{_localstatedir}/lib/zabbix -s /sbin/nologin \
	-c "Zabbix Monitoring System" zabbix
:
%endif

%post proxy-mysql
%if 0%{?rhel} >= 7
%systemd_post zabbix-proxy.service
%else
/sbin/chkconfig --add zabbix-proxy
%endif
/usr/sbin/update-alternatives --install %{_sbindir}/zabbix_proxy \
	zabbix-proxy %{_sbindir}/zabbix_proxy_mysql 10
:

%post proxy-pgsql
%if 0%{?rhel} >= 7
%systemd_post zabbix-proxy.service
%else
/sbin/chkconfig --add zabbix-proxy
%endif
/usr/sbin/update-alternatives --install %{_sbindir}/zabbix_proxy \
	zabbix-proxy %{_sbindir}/zabbix_proxy_pgsql 10
:

%post proxy-sqlite3
%if 0%{?rhel} >= 7
%systemd_post zabbix-proxy.service
%else
/sbin/chkconfig --add zabbix-proxy
%endif
/usr/sbin/update-alternatives --install %{_sbindir}/zabbix_proxy \
	zabbix-proxy %{_sbindir}/zabbix_proxy_sqlite3 10
:

%post java-gateway
%if 0%{?rhel} >= 7
%systemd_post zabbix-java-gateway.service
%else
/sbin/chkconfig --add zabbix-java-gateway
%endif
:

%if 0%{?build_server}
%post server-mysql
%if 0%{?rhel} >= 7
%systemd_post zabbix-server.service
%else
/sbin/chkconfig --add zabbix-server
%endif
/usr/sbin/update-alternatives --install %{_sbindir}/zabbix_server \
	zabbix-server %{_sbindir}/zabbix_server_mysql 10
:

%post server-pgsql
%if 0%{?rhel} >= 7
%systemd_post zabbix-server.service
%else
/sbin/chkconfig --add zabbix-server
%endif
/usr/sbin/update-alternatives --install %{_sbindir}/zabbix_server \
	zabbix-server %{_sbindir}/zabbix_server_pgsql 10
:

%post web
# The fonts directory was moved into assets subdirectory at one point.
#
# This broke invocation of update-alternatives command below, because the target link for zabbix-web-font changed
# from zabbix/fonts/graphfont.ttf to zabbix/assets/fonts/graphfont.ttf
#
# We handle this movement by deleting /var/lib/alternatives/zabbix-web-font file if it contains the old target link.
# We also remove symlink at zabbix/fonts/graphfont.ttf to have the old fonts directory be deleted during update.
if [ -f /var/lib/alternatives/zabbix-web-font ] && \
	[ -z "$(grep %{_datadir}/zabbix/assets/fonts/graphfont.ttf /var/lib/alternatives/zabbix-web-font)" ]
then
	rm /var/lib/alternatives/zabbix-web-font
	if [ -h %{_datadir}/zabbix/fonts/graphfont.ttf ]; then
		rm %{_datadir}/zabbix/fonts/graphfont.ttf
	fi
fi
/usr/sbin/update-alternatives --install %{_datadir}/zabbix/assets/fonts/graphfont.ttf \
	zabbix-web-font %{_datadir}/fonts/dejavu/DejaVuSans.ttf 10
:

%post web-japanese
/usr/sbin/update-alternatives --install %{_datadir}/zabbix/assets/fonts/graphfont.ttf zabbix-web-font \
%if 0%{?rhel} >= 8
	%{_datadir}/fonts/google-noto-cjk/NotoSansCJK-Regular.ttc 20
%else
	%{_datadir}/fonts/vlgothic/VL-PGothic-Regular.ttf 20
%endif
:

%if 0%{?rhel} >= 8
# The user apache must be available for these to work.
# It is provided by httpd or php-fpm packages.
%post apache-conf
if [ -d /etc/zabbix/web ]; then
	chown apache:apache /etc/zabbix/web/
fi
:
%endif

%if 0%{?rhel} >= 7
%post nginx-conf
if [ -d /etc/zabbix/web ]; then
	chown apache:apache /etc/zabbix/web/
fi
:
%endif

%endif

%preun agent
if [ "$1" = 0 ]; then
%if 0%{?rhel} >= 7
%systemd_preun zabbix-agent.service
%else
/sbin/service zabbix-agent stop >/dev/null 2>&1
/sbin/chkconfig --del zabbix-agent
%endif
fi
:

%preun agent-dbmon
if [ "$1" = 0 ]; then
%if 0%{?rhel} >= 7
%systemd_preun zabbix-agent-dbmon.service
%else
/sbin/service zabbix-agent-dbmon stop >/dev/null 2>&1
/sbin/chkconfig --del zabbix-agent-dbmon
%endif
fi
:

%if 0%{?build_agent2}
%preun agent2
%systemd_preun zabbix-agent2.service
:
%endif

%preun proxy-mysql
if [ "$1" = 0 ]; then
%if 0%{?rhel} >= 7
%systemd_preun zabbix-proxy.service
%else
/sbin/service zabbix-proxy stop >/dev/null 2>&1
/sbin/chkconfig --del zabbix-proxy
%endif
/usr/sbin/update-alternatives --remove zabbix-proxy \
%{_sbindir}/zabbix_proxy_mysql
fi
:

%preun proxy-pgsql
if [ "$1" = 0 ]; then
%if 0%{?rhel} >= 7
%systemd_preun zabbix-proxy.service
%else
/sbin/service zabbix-proxy stop >/dev/null 2>&1
/sbin/chkconfig --del zabbix-proxy
%endif
/usr/sbin/update-alternatives --remove zabbix-proxy \
	%{_sbindir}/zabbix_proxy_pgsql
fi
:

%preun proxy-sqlite3
if [ "$1" = 0 ]; then
%if 0%{?rhel} >= 7
%systemd_preun zabbix-proxy.service
%else
/sbin/service zabbix-proxy stop >/dev/null 2>&1
/sbin/chkconfig --del zabbix-proxy
%endif
/usr/sbin/update-alternatives --remove zabbix-proxy \
	%{_sbindir}/zabbix_proxy_sqlite3
fi
:

%preun java-gateway
if [ $1 -eq 0 ]; then
%if 0%{?rhel} >= 7
%systemd_preun zabbix-java-gateway.service
%else
/sbin/service zabbix-java-gateway stop >/dev/null 2>&1
/sbin/chkconfig --del zabbix-java-gateway
%endif
fi
:

%if 0%{?build_server}
%preun server-mysql
if [ "$1" = 0 ]; then
%if 0%{?rhel} >= 7
%systemd_preun zabbix-server.service
%else
/sbin/service zabbix-server stop >/dev/null 2>&1
/sbin/chkconfig --del zabbix-server
%endif
/usr/sbin/update-alternatives --remove zabbix-server \
	%{_sbindir}/zabbix_server_mysql
fi
:

%preun server-pgsql
if [ "$1" = 0 ]; then
%if 0%{?rhel} >= 7
%systemd_preun zabbix-server.service
%else
/sbin/service zabbix-server stop >/dev/null 2>&1
/sbin/chkconfig --del zabbix-server
%endif
/usr/sbin/update-alternatives --remove zabbix-server \
	%{_sbindir}/zabbix_server_pgsql
fi
:

%preun web
if [ "$1" = 0 ]; then
/usr/sbin/update-alternatives --remove zabbix-web-font \
     %{_datadir}/fonts/dejavu/DejaVuSans.ttf
fi
:

%preun web-japanese
if [ "$1" = 0 ]; then
/usr/sbin/update-alternatives --remove zabbix-web-font \
%if 0%{?rhel} >= 8
     %{_datadir}/fonts/google-noto-cjk/NotoSansCJK-Regular.ttc
%else
     %{_datadir}/fonts/vlgothic/VL-PGothic-Regular.ttf
%endif
fi
:
%endif

%postun agent
%if 0%{?rhel} >= 7
%systemd_postun_with_restart zabbix-agent.service
%else
if [ $1 -ge 1 ]; then
/sbin/service zabbix-agent try-restart >/dev/null 2>&1 || :
fi
%endif

%postun agent-dbmon
%if 0%{?rhel} >= 7
%systemd_postun_with_restart zabbix-agent-dbmon.service
%else
if [ $1 -ge 1 ]; then
/sbin/service zabbix-agent-dbmon try-restart >/dev/null 2>&1 || :
fi
%endif

%if 0%{?build_agent2}
%postun agent2
%systemd_postun_with_restart zabbix-agent2.service
%endif

%postun proxy-mysql
%if 0%{?rhel} >= 7
%systemd_postun_with_restart zabbix-proxy.service
%else
if [ $1 -ge 1 ]; then
/sbin/service zabbix-proxy try-restart >/dev/null 2>&1 || :
fi
%endif

%postun proxy-pgsql
%if 0%{?rhel} >= 7
%systemd_postun_with_restart zabbix-proxy.service
%else
if [ $1 -ge 1 ]; then
/sbin/service zabbix-proxy try-restart >/dev/null 2>&1 || :
fi
%endif

%postun proxy-sqlite3
%if 0%{?rhel} >= 7
%systemd_postun_with_restart zabbix-proxy.service
%else
if [ $1 -ge 1 ]; then
/sbin/service zabbix-proxy try-restart >/dev/null 2>&1 || :
fi
%endif

%postun java-gateway
%if 0%{?rhel} >= 7
%systemd_postun_with_restart zabbix-java-gateway.service
%else
if [ $1 -gt 1 ]; then
/sbin/service zabbix-java-gateway condrestart >/dev/null 2>&1 || :
fi
%endif

%if 0%{?build_server}
%postun server-mysql
%if 0%{?rhel} >= 7
%systemd_postun_with_restart zabbix-server.service
%else
if [ $1 -ge 1 ]; then
/sbin/service zabbix-server try-restart >/dev/null 2>&1 || :
fi
%endif

%postun server-pgsql
%if 0%{?rhel} >= 7
%systemd_postun_with_restart zabbix-server.service
%else
if [ $1 -ge 1 ]; then
/sbin/service zabbix-server try-restart >/dev/null 2>&1 || :
fi
%endif
%endif

%files agent
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README conf/zabbix_agentd/userparameter_mysql.conf
%config(noreplace) %{_sysconfdir}/zabbix/zabbix_agentd.conf
%config(noreplace) %{_sysconfdir}/logrotate.d/zabbix-agent
%dir %{_sysconfdir}/zabbix/zabbix_agentd.d
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/log/zabbix
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/run/zabbix
%{_sbindir}/zabbix_agentd
%{_mandir}/man8/zabbix_agentd.8*
%if 0%{?rhel} >= 7
%{_unitdir}/zabbix-agent.service
%{_prefix}/lib/tmpfiles.d/zabbix-agent.conf
%else
%{_sysconfdir}/init.d/zabbix-agent
%config(noreplace) %{_sysconfdir}/sysconfig/zabbix-agent
%endif

%files agent-dbmon
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%config(noreplace) %{_sysconfdir}/zabbix/zabbix_agentd_dbmon.conf
%config(noreplace) %{_sysconfdir}/logrotate.d/zabbix-agent-dbmon
%dir %{_sysconfdir}/zabbix/zabbix_agentd_dbmon.d
%config(noreplace) %{_sysconfdir}/zabbix/zabbix_agentd_dbmon.d/userparameter_dbmon.conf
%config %{_sysconfdir}/zabbix/zabbix_agentd_dbmon.d/dbmon.sh
%config(noreplace) %{_sysconfdir}/zabbix/zabbix_agentd_dbmon_sql.conf
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/log/zabbix
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/run/zabbix
%{_sbindir}/zabbix_agentd_dbmon
%{_mandir}/man8/zabbix_agentd_dbmon.8*
%if 0%{?rhel} >= 7
%{_unitdir}/zabbix-agent-dbmon.service
%{_prefix}/lib/tmpfiles.d/zabbix-agent-dbmon.conf
%else
%{_sysconfdir}/init.d/zabbix-agent-dbmon
%endif

%if 0%{?build_agent2}
%files agent2
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%config(noreplace) %{_sysconfdir}/zabbix/zabbix_agent2.conf
%config(noreplace) %{_sysconfdir}/logrotate.d/zabbix-agent2
%dir %{_sysconfdir}/zabbix/zabbix_agent2.d
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/log/zabbix
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/run/zabbix
%{_sbindir}/zabbix_agent2
%{_mandir}/man8/zabbix_agent2.8*
%{_unitdir}/zabbix-agent2.service
%{_prefix}/lib/tmpfiles.d/zabbix_agent2.conf
%endif

%files get
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%{_bindir}/zabbix_get
%{_mandir}/man1/zabbix_get.1*

%files sender
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%{_bindir}/zabbix_sender
%{_mandir}/man1/zabbix_sender.1*

%files js
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%{_bindir}/zabbix_js

%files proxy-mysql
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%doc database/mysql/schema.sql.gz
%attr(0640,root,zabbix) %config(noreplace) %{_sysconfdir}/zabbix/zabbix_proxy.conf
%dir /usr/lib/zabbix/externalscripts
%config(noreplace) %{_sysconfdir}/logrotate.d/zabbix-proxy
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/log/zabbix
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/run/zabbix
%{_mandir}/man8/zabbix_proxy.8*
%if 0%{?rhel} >= 7
%{_unitdir}/zabbix-proxy.service
%{_prefix}/lib/tmpfiles.d/zabbix-proxy.conf
%else
%{_sysconfdir}/init.d/zabbix-proxy
%endif
%{_sbindir}/zabbix_proxy_mysql

%files proxy-pgsql
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%doc database/postgresql/schema.sql.gz
%attr(0640,root,zabbix) %config(noreplace) %{_sysconfdir}/zabbix/zabbix_proxy.conf
%dir /usr/lib/zabbix/externalscripts
%config(noreplace) %{_sysconfdir}/logrotate.d/zabbix-proxy
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/log/zabbix
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/run/zabbix
%{_mandir}/man8/zabbix_proxy.8*
%if 0%{?rhel} >= 7
%{_unitdir}/zabbix-proxy.service
%{_prefix}/lib/tmpfiles.d/zabbix-proxy.conf
%else
%{_sysconfdir}/init.d/zabbix-proxy
%endif
%{_sbindir}/zabbix_proxy_pgsql

%files proxy-sqlite3
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%doc database/sqlite3/schema.sql.gz
%attr(0640,root,zabbix) %config(noreplace) %{_sysconfdir}/zabbix/zabbix_proxy.conf
%dir /usr/lib/zabbix/externalscripts
%config(noreplace) %{_sysconfdir}/logrotate.d/zabbix-proxy
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/log/zabbix
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/run/zabbix
%{_mandir}/man8/zabbix_proxy.8*
%if 0%{?rhel} >= 7
%{_unitdir}/zabbix-proxy.service
%{_prefix}/lib/tmpfiles.d/zabbix-proxy.conf
%else
%{_sysconfdir}/init.d/zabbix-proxy
%endif
%{_sbindir}/zabbix_proxy_sqlite3

%files java-gateway
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%config(noreplace) %{_sysconfdir}/zabbix/zabbix_java_gateway.conf
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/log/zabbix
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/run/zabbix
%if 0%{?rhel} >= 7
%{_datadir}/zabbix-java-gateway
%{_sbindir}/zabbix_java_gateway
%{_unitdir}/zabbix-java-gateway.service
%{_prefix}/lib/tmpfiles.d/zabbix-java-gateway.conf
%config(noreplace) %{_sysconfdir}/zabbix/zabbix_java_gateway_logback.xml
%else
%{_sbindir}/zabbix_java
%{_sysconfdir}/init.d/zabbix-java-gateway
%endif

%if 0%{?build_server}
%files server-mysql
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%doc database/mysql/create.sql.gz
%attr(0640,root,zabbix) %config(noreplace) %{_sysconfdir}/zabbix/zabbix_server.conf
%dir /usr/lib/zabbix/alertscripts
%dir /usr/lib/zabbix/externalscripts
%config(noreplace) %{_sysconfdir}/logrotate.d/zabbix-server
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/log/zabbix
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/run/zabbix
%{_mandir}/man8/zabbix_server.8*
%if 0%{?rhel} >= 7
%{_unitdir}/zabbix-server.service
%{_prefix}/lib/tmpfiles.d/zabbix-server.conf
%else
%{_sysconfdir}/init.d/zabbix-server
%endif
%{_sbindir}/zabbix_server_mysql

%files server-pgsql
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%doc database/postgresql/create.sql.gz
%doc database/postgresql/timescaledb.sql.gz
%attr(0640,root,zabbix) %config(noreplace) %{_sysconfdir}/zabbix/zabbix_server.conf
%dir /usr/lib/zabbix/alertscripts
%dir /usr/lib/zabbix/externalscripts
%config(noreplace) %{_sysconfdir}/logrotate.d/zabbix-server
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/log/zabbix
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/run/zabbix
%{_mandir}/man8/zabbix_server.8*
%if 0%{?rhel} >= 7
%{_unitdir}/zabbix-server.service
%{_prefix}/lib/tmpfiles.d/zabbix-server.conf
%else
%{_sysconfdir}/init.d/zabbix-server
%endif
%{_sbindir}/zabbix_server_pgsql

%files web
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%dir %attr(0750,apache,apache) %{_sysconfdir}/zabbix/web
%if 0%{?rhel} >= 8
%config(noreplace) %{_sysconfdir}/php-fpm.d/zabbix.conf
%endif
%if 0%{?rhel} == 7
%config(noreplace) %{_sysconfdir}/httpd/conf.d/zabbix.conf
%endif
%if 0%{?rhel} <= 6
%doc conf/httpd22-example.conf conf/httpd24-example.conf
%endif
%ghost %attr(0644,apache,apache) %config(noreplace) %{_sysconfdir}/zabbix/web/zabbix.conf.php
%config(noreplace) %{_sysconfdir}/zabbix/web/maintenance.inc.php
%{_datadir}/zabbix

%files web-mysql
%defattr(-,root,root,-)

%files web-pgsql
%defattr(-,root,root,-)

%files web-japanese
%defattr(-,root,root,-)

%if 0%{?rhel} >= 8
%files apache-conf
%doc AUTHORS ChangeLog COPYING NEWS README
%config(noreplace) %{_sysconfdir}/httpd/conf.d/zabbix.conf
%endif

%if 0%{?rhel} >= 7
%files nginx-conf
%doc AUTHORS ChangeLog COPYING NEWS README
%config(noreplace) %{_sysconfdir}/nginx/conf.d/zabbix.conf
%config(noreplace) %{_sysconfdir}/php-fpm.d/zabbix.conf
%endif

%endif

%changelog
* Fri Feb 21 2020 Zabbix Packager <info@zabbix.com> - 4.4.6-1
- update to 4.4.6
- using libssh instead of libssh2 on rhel/centos 8
- fixed font configuration in pre/post scriptlets on rhel-8

* Wed Jan 29 2020 Zabbix Packager <info@zabbix.com> - 4.4.5-2
- added posttrans script that preserves /etc/zabbix/zabbix_agentd.d/userparameter_mysql.conf file
- added config(noreplace) to /etc/sysconfig/zabbix-agent

* Fri Jan 17 2020  Zabbix Packager <info@zabbix.com> - 4.4.5-1
- update to 4.4.5
- added zabbix-js package
- moved /etc/zabbix/zabbix_agentd.d/userparameter_mysql.conf file to /usr/share/doc/zabbix-agent-<vers>/ (ZBX-14399)
- using zabbix-agent2.service instead of zabbix-agent.service in agent2 preun
- setting user for zabbix agent in /etc/sysconfig/zabbix-agent file on rhel <= 6

* Tue Jan 07 2020  Zabbix Packager <info@zabbix.com> - 4.4.4-2
- build of rhel-5 packages to be resigned with gpg version 3

* Thu Dec 19 2019  Zabbix Packager <info@zabbix.com> - 4.4.4-1
- update to 4.4.4
  added After=<database>.service directives to server and proxy service files

* Wed Nov 27 2019 Zabbix Packager <info@zabbix.com> - 4.4.3-1
- update to 4.4.3
  added User=zabbix and Group=zabbix directives to agent service file

* Mon Nov 25 2019 Zabbix Packager <info@zabbix.com> - 4.4.2-1
- update to 4.4.2

* Mon Oct 28 2019 Zabbix Packager <info@zabbix.com> - 4.4.1-1
- update to 4.4.1

* Mon Oct 07 2019 Zabbix Packager <info@zabbix.com> 4.4.0-1
- update to 4.4.0

* Thu Oct 03 2019 Zabbix Packager <info@zabbix.com> - 4.4.0-0.5rc1
- update to 4.4.0rc1

* Tue Sep 24 2019 Zabbix Packager <info@zabbix.com> - 4.4.0-0.4beta1
- update to 4.4.0beta1
- added zabbix-agent2 package

* Wed Sep 18 2019 Zabbix Packager <info@zabbix.com> - 4.4.0-0.3alpha3
- update to 4.4.0alpha3

* Thu Aug 15 2019 Zabbix Packager <info@zabbix.com> - 4.4.0-0.2alpha2
- update to 4.4.0alpha2
- using google-noto-sans-cjk-ttc-fonts for graphfont in web-japanese package on rhel-8
- added php-fpm as dependency of zabbix-web packages on rhel-8

* Wed Jul 17 2019 Zabbix Packager <info@zabbix.com> - 4.4.0-0.1alpha1
- update to 4.4.0alpha1
- removed apache config from zabbix-web package
- added dedicated zabbix-apache-conf and zabbix-nginx-conf packages

* Fri Mar 29 2019 Zabbix Packager <info@zabbix.com> - 4.2.0-1
- update to 4.2.0
- removed jabber notifications support and dependency on iksemel library

* Tue Mar 26 2019 Zabbix Packager <info@zabbix.com> - 4.2.0-0.6rc2
- update to 4.2.0rc2

* Mon Mar 18 2019 Zabbix Packager <info@zabbix.com> - 4.2.0-0.5rc1
- update to 4.2.0rc1

* Mon Mar 04 2019 Zabbix Packager <info@zabbix.com> - 4.2.0-0.4beta2
- update to 4.2.0beta2

* Mon Feb 18 2019 Zabbix Packager <info@zabbix.com> - 4.2.0-0.1beta1
- update to 4.2.0beta1

* Tue Feb 05 2019 Zabbix Packager <info@zabbix.com> - 4.2.0-0.3alpha3
- build of 4.2.0alpha3 with *.mo files

* Wed Jan 30 2019 Zabbix Packager <info@zabbix.com> - 4.2.0-0.2alpha3
- added timescaledb.sql.gz to zabbix-server-pgsql package

* Mon Jan 28 2019 Zabbix Packager <info@zabbix.com> - 4.2.0-0.1alpha3
- update to 4.2.0alpha3

* Fri Dec 21 2018 Zabbix Packager <info@zabbix.com> - 4.2.0-0.2alpha2
- update to 4.2.0alpha2

* Tue Nov 27 2018 Zabbix Packager <info@zabbix.com> - 4.2.0-0.1alpha1
- update to 4.2.0alpha1

* Mon Oct 29 2018 Zabbix Packager <info@zabbix.com> - 4.0.1-1
- update to 4.0.1

* Mon Oct 01 2018 Zabbix Packager <info@zabbix.com> - 4.0.0-2
- update to 4.0.0

* Fri Sep 28 2018 Zabbix Packager <info@zabbix.com> - 4.0.0-1.1rc3
- update to 4.0.0rc3

* Tue Sep 25 2018 Zabbix Packager <info@zabbix.com> - 4.0.0-1.1rc2
- update to 4.0.0rc2

* Wed Sep 19 2018 Zabbix Packager <info@zabbix.com> - 4.0.0-1.1rc1
- update to 4.0.0rc1

* Mon Sep 10 2018 Zabbix Packager <info@zabbix.com> - 4.0.0-1.1beta2
- update to 4.0.0beta2

* Tue Aug 28 2018 Zabbix Packager <info@zabbix.com> - 4.0.0-1.1beta1
- update to 4.0.0beta1

* Mon Jul 23 2018 Zabbix Packager <info@zabbix.com> - 4.0.0-1.1alpha9
- update to 4.0.0alpha9
- add PHP variable max_input_vars = 10000, overriding default 1000

* Mon Jun 18 2018 Zabbix Packager <info@zabbix.com> - 4.0.0-1.1alpha8
- update to 4.0.0alpha8

* Wed May 30 2018 Zabbix Packager <info@zabbix.com> - 4.0.0-1.1alpha7
- update to 4.0.0alpha7

* Fri Apr 27 2018 Zabbix Packager <info@zabbix.com> - 4.0.0-1.1alpha6
- update to 4.0.0alpha6
- add support for Ubuntu 18.04 (Bionic)
- move enabling JMX interface on Zabbix java gateway to zabbix_java_gateway.conf

* Mon Mar 26 2018 Vladimir Levijev <vladimir.levijev@zabbix.com> - 4.0.0-1.1alpha5
- update to 4.0.0alpha5

* Tue Feb 27 2018 Vladimir Levijev <vladimir.levijev@zabbix.com> - 4.0.0-1.1alpha4
- update to 4.0.0alpha4

* Mon Feb 05 2018 Vladimir Levijev <vladimir.levijev@zabbix.com> - 4.0.0-1.1alpha3
- update to 4.0.0alpha3

* Tue Jan 09 2018 Vladimir Levijev <vladimir.levijev@zabbix.com> - 4.0.0-1.1alpha2
- update to 4.0.0alpha2

* Tue Dec 19 2017 Vladimir Levijev <vladimir.levijev@zabbix.com> - 4.0.0-1alpha1
- update to 4.0.0alpha1

* Thu Nov 09 2017 Vladimir Levijev <vladimir.levijev@zabbix.com> - 3.4.4-2
- add missing translation (.mo) files

* Tue Nov 07 2017 Vladimir Levijev <vladimir.levijev@zabbix.com> - 3.4.4-1
- update to 3.4.4
- fix issue with new line character in pid file that resulted in failure when shutting down daemons on RHEL 5

* Tue Oct 17 2017 Vladimir Levijev <vladimir.levijev@zabbix.com> - 3.4.3-1
- update to 3.4.3

* Mon Sep 25 2017 Vladimir Levijev <vladimir.levijev@zabbix.com> - 3.4.2-1
- update to 3.4.2

* Mon Aug 28 2017 Vladimir Levijev <vladimir.levijev@zabbix.com> - 3.4.1-1
- update to 3.4.1
- change SocketDir to /var/run/zabbix

* Mon Aug 21 2017 Vladimir Levijev <vladimir.levijev@zabbix.com> - 3.4.0-1
- update to 3.4.0

* Wed Apr 26 2017 Kodai Terashima <kodai.terashima@zabbix.com> - 3.4.0-1alpha1
- update to 3.4.0alpla1 r68116
- add libpcre and libevent for compile option

* Sun Apr 23 2017 Kodai Terashima <kodai.terashima@zabbix.com> - 3.2.5-1
- update to 3.2.5
- add TimeoutSec=0 to systemd service file

* Thu Mar 02 2017 Kodai Terashima <kodai.terashima@zabbix.com> - 3.2.4-2
- remove TimeoutSec for systemd

* Mon Feb 27 2017 Kodai Terashima <kodai.terashima@zabbix.com> - 3.2.4-1
- update to 3.2.4
- add TimeoutSec for systemd service file

* Wed Dec 21 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.2.3-1
- update to 3.2.3

* Thu Dec 08 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.2.2-1
- update to 3.2.2

* Sun Oct 02 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.2.1-1
- update to 3.2.1
- use zabbix user and group for Java Gateway
- add SuccessExitStatus=143 for Java Gateway servie file

* Tue Sep 13 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.2.0-1
- update to 3.2.0
- add *.conf for Include parameter in agent configuration file

* Mon Sep 12 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.2.0rc2-1
- update to 3.2.0rc2

* Fri Sep 09 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.2.0rc1-1
- update to 3.2.0rc1

* Thu Sep 01 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.2.0beta2-1
- update to 3.2.0beta2

* Fri Aug 26 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.2.0beta1-1
- update to 3.2.0beta1

* Fri Aug 12 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.2.0alpha1-1
- update to 3.2.0alpha1

* Sun Jul 24 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.4-1
- update to 3.0.4

* Sun May 22 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.3-1
- update to 3.0.3
- fix java gateway systemd script to use java options

* Wed Apr 20 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.2-1
- update to 3.0.2
- remove ZBX-10459.patch

* Sat Apr 02 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.1-2
- fix proxy packges doesn't have schema.sql.gz
- add server and web packages for RHEL6
- add ZBX-10459.patch

* Sun Feb 28 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.1-1
- update to 3.0.1
- remove DBSocker parameter

* Sat Feb 20 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.0-2
- agent, proxy and java-gateway for RHEL 5 and 6

* Mon Feb 15 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.0-1
- update to 3.0.0

* Thu Feb 11 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.0rc2
- update to 3.0.0rc2
- add TIMEOUT parameter for java gateway conf

* Thu Feb 04 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.0rc1
- update to 3.0.0rc1

* Sat Jan 30 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.0beta2
- update to 3.0.0beta2

* Thu Jan 21 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.0beta1
- update to 3.0.0beta1

* Thu Jan 14 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.0alpha6
- update to 3.0.0alpla6
- remove zabbix_agent conf and binary

* Wed Jan 13 2016 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.0alpha5
- update to 3.0.0alpha5

* Fri Nov 13 2015 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.0alpha4-1
- update to 3.0.0alpha4

* Thu Oct 29 2015 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.0alpha3-2
- fix web-pgsql package dependency
- add --with-openssl option

* Mon Oct 19 2015 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.0alpha3-1
- update to 3.0.0alpha3

* Tue Sep 29 2015 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.0alpha2-3
- add IfModule for mod_php5 in apache configuration file
- fix missing proxy_mysql alternatives symlink
- chagne snmptrap log filename
- remove include dir from server and proxy conf

* Fri Sep 18 2015 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.0alpha2-2
- fix create.sql doesn't contain schema.sql & images.sql

* Tue Sep 15 2015 Kodai Terashima <kodai.terashima@zabbix.com> - 3.0.0alpha2-1
- update to 3.0.0alpha2

* Sat Aug 22 2015 Kodai Terashima <kodai.terashima@zabbix.com> - 2.5.0-1
- create spec file from scratch
- update to 2.5.0
