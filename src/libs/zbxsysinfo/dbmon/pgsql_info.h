#ifndef ZABBIX_PGINFO_H
#define ZABBIX_PGINFO_H

#include "sysinfo.h"

extern ZBX_METRIC	parameters_dbmon_pgsql[];

static int	PG_PING(AGENT_REQUEST *request, AGENT_RESULT *result);
static int	PG_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result);
static int	PG_DB_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);

#endif /* ZABBIX_PGINFO_H */
