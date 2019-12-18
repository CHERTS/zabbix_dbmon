#ifndef ZABBIX_MYSQLINFO_H
#define ZABBIX_MYSQLINFO_H

#include "sysinfo.h"

extern ZBX_METRIC	parameters_dbmon_mysql[];

static int	MYSQL_PING(AGENT_REQUEST *request, AGENT_RESULT *result);
static int	MYSQL_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result);
static int	MYSQL_DB_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);

#endif /* ZABBIX_MYSQLINFO_H */
