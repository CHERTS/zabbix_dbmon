/*
** Zabbix
** Copyright (C) 2019 Mikhail Grigorev
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/

#ifndef ZABBIX_ORACLEINFO_H
#define ZABBIX_ORACLEINFO_H

#include "sysinfo.h"

extern ZBX_METRIC	parameters_dbmon_oracle[];

typedef enum
{
	ORA_ANY_ROLE = 0,
	ORA_STANDBY, // Include 'SNAPSHOT STANDBY','LOGICAL STANDBY','PHYSICAL STANDBY'
	ORA_PRIMARY
}
zbx_db_oracle_db_role;

static char *ORA_DB_ROLE[] = {
	"ANY",
	"STANDBY",
	"PRIMARY"
};

typedef enum
{
	ORA_TS_PERMANENT = 0,
	ORA_TS_TEMPORARY,
	ORA_TS_UNDO,
}
zbx_db_oracle_ts_type;

typedef enum
{
	ORA_ANY_STATUS = 0,
	ORA_ACTIVE,
	ORA_SUSPENDED,
	ORA_INSTANCE_RECOVERY
}
zbx_db_oracle_db_status;

static char *ORA_DB_STATUS[] = {
	"ANY",
	"ACTIVE",
	"SUSPENDED",
	"INSTANCE RECOVERY"
};

int	ORACLE_INSTANCE_PING(AGENT_REQUEST *request, AGENT_RESULT *result);
int	ORACLE_GET_INSTANCE_RESULT(AGENT_REQUEST *request, AGENT_RESULT *result);
int	ORACLE_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);
int	ORACLE_DB_INFO(AGENT_REQUEST *request, AGENT_RESULT *result);
int	ORACLE_PDB_INFO(AGENT_REQUEST *request, AGENT_RESULT *result);
int	ORACLE_TS_INFO(AGENT_REQUEST *request, AGENT_RESULT *result);

#endif /* ZABBIX_ORACLEINFO_H */
