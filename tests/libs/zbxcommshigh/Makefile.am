if SERVER
ZLIB_tests = zbx_tcp_recv_ext_zlib
endif

noinst_PROGRAMS = zbx_tcp_recv_ext zbx_tcp_recv_raw_ext $(ZLIB_tests)

COMMON_SRC_FILES = \
	../../zbxmocktest.h

COMMON_LIB_FILES = \
	$(top_srcdir)/tests/libzbxmockdata.a \
	$(top_srcdir)/src/libs/zbxsysinfo/libzbxagentsysinfo.a \
	$(top_srcdir)/src/libs/zbxsysinfo/$(ARCH)/libfunclistsysinfo.a \
	$(top_srcdir)/src/libs/zbxsysinfo/$(ARCH)/libspechostnamesysinfo.a \
	$(top_srcdir)/src/libs/zbxsysinfo/agent/libagentsysinfo.a \
	$(top_srcdir)/src/libs/zbxsysinfo/simple/libsimplesysinfo.a \
	$(top_srcdir)/src/libs/zbxsysinfo/common/libcommonsysinfo.a \
	$(top_srcdir)/src/libs/zbxlog/libzbxlog.a \
	$(top_srcdir)/src/libs/zbxprof/libzbxprof.a \
	$(top_srcdir)/src/libs/zbxregexp/libzbxregexp.a \
	$(top_srcdir)/src/libs/zbxalgo/libzbxalgo.a \
	$(top_srcdir)/src/libs/zbxsys/libzbxsys.a \
	$(top_srcdir)/src/libs/zbxnix/libzbxnix.a \
	$(top_srcdir)/src/libs/zbxsysinfo/$(ARCH)/libspecsysinfo.a \
	$(top_srcdir)/src/libs/zbxcomms/libzbxcomms.a \
	$(top_srcdir)/src/libs/zbxcompress/libzbxcompress.a \
	$(top_srcdir)/src/libs/zbxconf/libzbxconf.a \
	$(top_srcdir)/src/libs/zbxlog/libzbxlog.a \
	$(top_srcdir)/src/libs/zbxprof/libzbxprof.a \
	$(top_srcdir)/src/libs/zbxsys/libzbxsys.a \
	$(top_srcdir)/src/libs/zbxcommon/libzbxcommon.a \
	$(top_srcdir)/src/libs/zbxcrypto/libzbxcrypto.a \
	$(top_srcdir)/src/libs/zbxjson/libzbxjson.a \
	$(top_srcdir)/src/libs/zbxexec/libzbxexec.a \
	$(top_srcdir)/src/libs/zbxmodules/libzbxmodules.a \
	$(top_srcdir)/src/libs/zbxnix/libzbxnix.a \
	$(top_srcdir)/src/libs/zbxregexp/libzbxregexp.a \
	$(top_srcdir)/tests/libzbxmocktest.a \
	$(top_srcdir)/tests/libzbxmockdata.a

COMMON_COMPILER_FLAGS = -DZABBIX_DAEMON -I@top_srcdir@/tests

zbx_tcp_recv_ext_SOURCES = \
	zbx_tcp_recv_ext.c \
	$(COMMON_SRC_FILES)

zbx_tcp_recv_ext_LDADD = \
	$(COMMON_LIB_FILES)

zbx_tcp_recv_ext_LDADD += @AGENT_LIBS@

zbx_tcp_recv_ext_LDFLAGS = @AGENT_LDFLAGS@

zbx_tcp_recv_ext_CFLAGS = $(COMMON_COMPILER_FLAGS)

if SERVER
zbx_tcp_recv_ext_zlib_SOURCES = \
	zbx_tcp_recv_ext.c \
	$(COMMON_SRC_FILES)

zbx_tcp_recv_ext_zlib_LDADD = \
	$(COMMON_LIB_FILES)

zbx_tcp_recv_ext_zlib_LDADD += @AGENT_LIBS@

zbx_tcp_recv_ext_zlib_LDFLAGS = @AGENT_LDFLAGS@

zbx_tcp_recv_ext_zlib_CFLAGS = $(COMMON_COMPILER_FLAGS)
endif

zbx_tcp_recv_raw_ext_SOURCES = \
	zbx_tcp_recv_raw_ext.c \
	$(COMMON_SRC_FILES)

zbx_tcp_recv_raw_ext_LDADD = \
	$(COMMON_LIB_FILES)

zbx_tcp_recv_raw_ext_LDADD += @AGENT_LIBS@

zbx_tcp_recv_raw_ext_LDFLAGS = @AGENT_LDFLAGS@

zbx_tcp_recv_raw_ext_CFLAGS = $(COMMON_COMPILER_FLAGS)
