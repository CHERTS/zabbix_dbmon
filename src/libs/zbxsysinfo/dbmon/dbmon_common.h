#ifndef ZABBIX_DBMON_COMMON_H
#define ZABBIX_DBMON_COMMON_H

#include "sysinfo.h"

int make_discovery_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result);
int make_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result);
int make_onerow_json_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result);
int make_multirow_json_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result);
char *get_str_one_result(AGENT_REQUEST *request, AGENT_RESULT *result, const unsigned int row, const unsigned int col, struct zbx_db_result db_result);
int zbx_db_compare_version(char *version1, char *version2);

#endif /* ZABBIX_DBMON_COMMON_H */
