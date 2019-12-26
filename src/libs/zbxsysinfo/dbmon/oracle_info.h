#ifndef ZABBIX_ORACLEINFO_H
#define ZABBIX_ORACLEINFO_H

#include "sysinfo.h"

extern ZBX_METRIC	parameters_dbmon_oracle[];

static int	ORACLE_INSTANCE_PING(AGENT_REQUEST *request, AGENT_RESULT *result);
static int	ORACLE_GET_INSTANCE_RESULT(AGENT_REQUEST *request, AGENT_RESULT *result);
static int	ORACLE_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);
static int	ORACLE_DB_INFO(AGENT_REQUEST *request, AGENT_RESULT *result);
static int	ORACLE_DB_INCARNATION(AGENT_REQUEST *request, AGENT_RESULT *result);
static int	ORACLE_DB_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result);

#endif /* ZABBIX_ORACLEINFO_H */
