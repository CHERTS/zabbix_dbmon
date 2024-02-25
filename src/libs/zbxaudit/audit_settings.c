/*
** Zabbix
** Copyright (C) 2001-2024 Zabbix SIA
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

#include "audit_settings.h"

#include "zbxalgo.h"
#include "audit.h"

void	zbx_audit_settings_create_entry(int audit_action, zbx_uint64_t configid)
{
	zbx_audit_entry_t	local_audit_entry, *plocal_audit_entry = &local_audit_entry;

	RETURN_IF_AUDIT_OFF();

	local_audit_entry.id = configid;
	local_audit_entry.cuid = NULL;
	local_audit_entry.id_table = AUDIT_CONFIG_ID;

	if (NULL == zbx_hashset_search(zbx_get_audit_hashset(), &plocal_audit_entry))
	{
		zbx_audit_entry_t	*new_entry;

		new_entry = zbx_audit_entry_init(configid, AUDIT_CONFIG_ID, "", audit_action, AUDIT_RESOURCE_SETTINGS);
		zbx_hashset_insert(zbx_get_audit_hashset(), &new_entry, sizeof(new_entry));
	}
}

void	zbx_audit_settings_update_field_int(zbx_uint64_t configid, const char *key, int old_value, int new_value)
{
	zbx_audit_entry_t	*entry;

	RETURN_IF_AUDIT_OFF();

	entry = zbx_audit_get_entry(configid, NULL, AUDIT_CONFIG_ID);
	zbx_audit_entry_append_int(entry, AUDIT_ACTION_UPDATE, key, old_value, new_value);
}
