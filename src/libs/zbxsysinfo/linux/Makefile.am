## Process this file with automake to produce Makefile.in

noinst_LIBRARIES = libfunclistsysinfo.a libspecsysinfo.a libspechostnamesysinfo.a

libfunclistsysinfo_a_SOURCES = \
	linux.c

libspecsysinfo_a_CFLAGS = -I$(top_srcdir)/src/zabbix_agent

libspecsysinfo_a_SOURCES = \
	boottime.c \
	cpu.c \
	diskio.c \
	diskspace.c \
	hardware.c \
	hardware.h \
	inodes.c \
	inodes.h \
	kernel.c \
	memory.c \
	net.c \
	proc.c \
	proc.h \
	sensors.c \
	software.c \
	software.h \
	system.c \
	swap.c \
	uptime.c

libspechostnamesysinfo_a_SOURCES = \
	hostname.c
