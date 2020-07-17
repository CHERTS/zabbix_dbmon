Name:		zabbix
Version:	5.0.2
Release:	%{?alphatag:0.}1%{?alphatag}%{?dist}
Summary:	The Enterprise-class open source monitoring solution
Group:		Applications/Internet
License:	GPLv2+
URL:		http://www.zabbix.com/
Source0:	%{name}-%{version}%{?alphatag}.tar.gz
Source1:        zabbix-logrotate.in
Source2:	zabbix-tmpfiles.conf
Source3:        zabbix-agent-dbmon.service
Source4:        zabbix-agent-dbmon.init
Source5:        zabbix-agent-dbmon.sysconfig

Buildroot:	%{_tmppath}/zabbix-%{version}-%{release}-root-%(%{__id_u} -n)

# FIXME: Building debuginfo is broken on RHEL-8. Disabled for now.
%if 0%{?rhel} == 8
%define debug_package %{nil}
%endif

%if 0%{?rhel} >= 7
BuildRequires:	mysql-devel >= 5.5
BuildRequires:	postgresql-devel >= 9.2
%endif
BuildRequires:	curl-devel >= 7.13.1
BuildRequires:	libxml2-devel
BuildRequires:	pcre-devel
BuildRequires:	libevent-devel
%if 0%{?rhel} >= 6
BuildRequires:	openssl-devel >= 1.0.1
%endif
%if 0%{?rhel} >= 7
BuildRequires:	systemd
%endif
BuildRequires:  libconfig-devel

%description
Zabbix is the ultimate enterprise-level software designed for
real-time monitoring of millions of metrics collected from tens of
thousands of servers, virtual machines and network devices.

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
Summary:                Zabbix Get
Group:                  Applications/Internet

%description get
Zabbix get command line utility.

%package sender
Summary:                Zabbix Sender
Group:                  Applications/Internet

%description sender
Zabbix sender command line utility.

%prep
%setup0 -q -n %{name}-%{version}%{?alphatag}

%build

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
build_dbmon_flags="$build_dbmon_flags --with-openssl"
%endif

%configure $build_dbmon_flags
make %{?_smp_mflags}
cp src/zabbix_agent/zabbix_agentd src/zabbix_agent/zabbix_agentd_dbmon

%install

rm -rf $RPM_BUILD_ROOT

# install
make DESTDIR=$RPM_BUILD_ROOT install

# install necessary directories
mkdir -p $RPM_BUILD_ROOT%{_localstatedir}/log/zabbix
mkdir -p $RPM_BUILD_ROOT%{_localstatedir}/run/zabbix
mkdir -p $RPM_BUILD_ROOT%{_localstatedir}/lib/zabbix

# install zabbix_agentd_dbmon
install -m 0755 -p src/zabbix_agent/zabbix_agentd_dbmon $RPM_BUILD_ROOT%{_sbindir}/

# install configuration files
mkdir $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_agentd_dbmon.d
install -m 0644 conf/zabbix_agentd_dbmon/userparameter_dbmon.conf $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_agentd_dbmon.d
install -Dm 0755 conf/zabbix_agentd_dbmon/dbmon.sh $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_agentd_dbmon.d
install -m 0644 conf/zabbix_agentd_dbmon_sql.conf $RPM_BUILD_ROOT%{_sysconfdir}/zabbix

cat conf/zabbix_agentd_dbmon.conf | sed \
        -e '/^# PidFile=/a \\nPidFile=%{_localstatedir}/run/zabbix/zabbix_agentd_dbmon.pid' \
        -e 's|^LogFile=.*|LogFile=%{_localstatedir}/log/zabbix/zabbix_agentd_dbmon.log|g' \
        -e '/^# LogFileSize=.*/a \\nLogFileSize=5' \
        -e '/^# Include=$/a \\nInclude=%{_sysconfdir}/zabbix/zabbix_agentd_dbmon.d/*.conf' \
        > $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_agentd_dbmon.conf
cp man/zabbix_agentd_dbmon.man $RPM_BUILD_ROOT%{_mandir}/man8/zabbix_agentd_dbmon.8

# install logrotate configuration files
mkdir -p $RPM_BUILD_ROOT%{_sysconfdir}/logrotate.d
cat %{SOURCE1} | sed \
        -e 's|COMPONENT|agentd-dbmon|g' \
        > $RPM_BUILD_ROOT%{_sysconfdir}/logrotate.d/zabbix-agent-dbmon

# install startup scripts
%if 0%{?rhel} >= 7
install -Dm 0644 -p %{SOURCE3} $RPM_BUILD_ROOT%{_unitdir}/zabbix-agent-dbmon.service
%else
install -Dm 0755 -p %{SOURCE4} $RPM_BUILD_ROOT%{_sysconfdir}/init.d/zabbix-agent-dbmon
%endif

%if 0%{?rhel} <= 6
install -Dm 0644 -p %{SOURCE5} $RPM_BUILD_ROOT%{_sysconfdir}/sysconfig/zabbix-agent
%endif

# install systemd-tmpfiles conf
%if 0%{?rhel} >= 7
install -Dm 0644 -p %{SOURCE2} $RPM_BUILD_ROOT%{_prefix}/lib/tmpfiles.d/zabbix-agent-dbmon.conf
%endif

%clean
rm -rf $RPM_BUILD_ROOT

%pre agent-dbmon
getent group zabbix > /dev/null || groupadd -r zabbix
getent passwd zabbix > /dev/null || \
	useradd -r -g zabbix -d %{_localstatedir}/lib/zabbix -s /sbin/nologin \
	-c "Zabbix Monitoring System" zabbix
mkdir -p %{_localstatedir}/lib/zabbix > /dev/null
chown -R zabbix:zabbix %{_localstatedir}/lib/zabbix > /dev/null
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

%postun agent-dbmon
%if 0%{?rhel} >= 7
%systemd_postun_with_restart zabbix-agent-dbmon.service
%else
if [ $1 -ge 1 ]; then
/sbin/service zabbix-agent-dbmon try-restart >/dev/null 2>&1 || :
fi
%endif

%files agent-dbmon
%exclude %{_sysconfdir}/zabbix/zabbix_agentd.conf
%exclude %{_sbindir}/zabbix_agentd
%exclude %{_mandir}/man8/zabbix_agentd.8.gz
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
%attr(0755,zabbix,zabbix) %dir %{_localstatedir}/lib/zabbix
%{_sbindir}/zabbix_agentd_dbmon
%{_mandir}/man8/zabbix_agentd_dbmon.8*
%if 0%{?rhel} >= 7
%{_unitdir}/zabbix-agent-dbmon.service
%{_prefix}/lib/tmpfiles.d/zabbix-agent-dbmon.conf
%else
%{_sysconfdir}/init.d/zabbix-agent-dbmon
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

%changelog
* Fri Jul 17 2020 Mikhail Grigorev <support@db-service.ru> - 5.0.0-1
- update to 5.0.0
