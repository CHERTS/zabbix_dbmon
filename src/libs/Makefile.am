## Process this file with automake to produce Makefile.in

DIST_SUBDIRS = \
	zbxdbcache \
	zbxdbhigh \
	zbxdb \
	zbxdbupgrade \
	zbxmemory \
	zbxalgo \
	zbxmedia \
	zbxcrypto \
	zbxcommon \
	zbxcomms \
	zbxcommshigh \
	zbxlog \
	zbxnix \
	zbxconf \
	zbxsysinfo \
	zbxprof \
	zbxsys \
	zbxjson \
	zbxserver \
	zbxicmpping \
	zbxexec \
	zbxself \
	zbxmodules \
	zbxregexp \
	zbxtasks \
	zbxhttp \
	zbxipcservice \
	zbxhistory \
	zbxcompress \
	zbxembed \
	zbxprometheus \
	zbxvault \
	zbxdiag \
	zbxtrends \
	zbxavailability \
	zbxservice \
	zbxaudit \
	zbxeval \
	zbxxml \
	zbxha \
	zbxdbmon \
	zbxrtc

if SERVER
SERVER_SUBDIRS = \
	zbxdb \
	zbxdbupgrade \
	zbxdbcache \
	zbxdbhigh \
	zbxhttp \
	zbxmemory \
	zbxserver \
	zbxicmpping \
	zbxmedia \
	zbxself \
	zbxtasks \
	zbxhistory \
	zbxcompress \
	zbxxml \
	zbxembed \
	zbxprometheus \
	zbxvault \
	zbxdiag \
	zbxtrends \
	zbxavailability \
	zbxservice \
	zbxaudit \
	zbxeval \
	zbxha \
	zbxrtc
else
if PROXY
PROXY_SUBDIRS = \
	zbxdb \
	zbxdbupgrade \
	zbxdbcache \
	zbxdbhigh \
	zbxhttp \
	zbxmemory \
	zbxserver \
	zbxicmpping \
	zbxself \
	zbxtasks \
	zbxhistory \
	zbxcompress \
	zbxxml \
	zbxembed \
	zbxprometheus \
	zbxvault \
	zbxdiag \
	zbxtrends \
	zbxavailability \
	zbxaudit \
	zbxeval \
	zbxrtc
endif
endif

SUBDIRS = \
	zbxcrypto \
	zbxcommon \
	zbxlog \
	zbxalgo \
	zbxnix \
	zbxconf \
	zbxhttp \
	zbxsysinfo \
	zbxprof \
	zbxsys \
	zbxcommshigh \
	zbxcomms \
	zbxjson \
	zbxexec \
	zbxmodules \
	zbxregexp \
	zbxipcservice \
	zbxcompress \
	zbxdbmon \
	$(PROXY_SUBDIRS) \
	$(SERVER_SUBDIRS) \
	$(AGENT_SUBDIRS)

EXTRA_DIST = zbxwin32
