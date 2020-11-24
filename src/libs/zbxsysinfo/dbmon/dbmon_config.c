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

#include "common.h"
#include "sysinfo.h"
#include "log.h"
#include "module.h"
#include "zbxdbmon.h"
#include "dbmon_params.h"
#include <libconfig.h>

// Default query config file location
#if !defined(_WINDOWS) && !defined(__MINGW32__)
#define DEFAULT_DBMON_CONFIG_FILE    "/etc/zabbix/zabbix_agentd_dbmon_sql.conf"
#else
#define DEFAULT_DBMON_CONFIG_FILE    "C:\\DBS_Zabbix_DBMON\\zabbix_agentd_dbmon_sql.conf"
#endif

// Default memory usage
#define MAX_GLOBBING_PATH_LENGTH         512

char **query_keys = NULL;
char **query_values = NULL;
int query_count = 0;

/*
 * Function get_dbmon_configfile
 *
 * Returns the config file path.
 * 
 * If the environment variable DBMONCONFIGFILE is set then that is
 * used, otherwise DEFAULT_DBMON_CONFIG_FILE is used.
 *
 * Returns: pointer to const char
 */
static inline const char *get_dbmon_configfile(const char *conf_path)
{
	char *path = NULL;

	if (NULL == conf_path || '\0' == *conf_path)
	{
		path = DEFAULT_DBMON_CONFIG_FILE;
		return path;
	}
	else if (strlen(conf_path) > MAX_GLOBBING_PATH_LENGTH)
	{
		zabbix_log(LOG_LEVEL_ERR, "In %s: The path specified in the DBSQLFileName parameter exceeds the maximum length %i", __func__, MAX_GLOBBING_PATH_LENGTH);
		return NULL;
	}
    
	return conf_path;
}

static inline int add_named_query(const char *name, const char *query)
{
	int i = query_count - 1;

	while(i >= 0 && (NULL == query_keys[i] || 0 > strcmp(name, query_keys[i])))
	{
		query_keys[i+1] = query_keys[i];
		query_values[i+1] = query_values[i];
		query_keys[i] = NULL;
		query_values[i] = NULL;
		i--;
	}

	query_keys[i+1] = strdup(name);
	query_values[i+1] = strdup(query);

	return EXIT_SUCCESS;
}

/*
 * Function get_query_by_name
 *
 * Searches the key array to find the
 * corresponding SQL stmt using binary
 * search.
 *
 * Returns: 
 *    If Key Found: pointer to query string
 *    If Not Found: NULL
 */
const char *get_query_by_name(const char *key)
{
	int top = query_count - 1;
	int mid = 0;
	int bottom = 0;
	int cmp = -1;

	while (bottom <= top)
	{
		mid = (bottom + top)/2;
		cmp = strcmp(query_keys[mid], key);
		if (cmp == 0)
		{
			return query_values[mid];
		}
		else if (cmp > 0)
		{
			top = mid - 1;
		}
		else if (cmp < 0)
		{
			bottom = mid + 1;
		}
	}

	return NULL;
}

static int read_config_queries(const config_setting_t *root)
{
	int                 i = 0;
	const char          *key = NULL, *value = NULL;
	config_setting_t    *node = NULL;

	if (CONFIG_TYPE_GROUP != config_setting_type(root))
	{
		zabbix_log(LOG_LEVEL_ERR, "In %s: queries is not a valid configuration group", __func__);
		return EXIT_FAILURE;
	}

	query_count = config_setting_length(root);
	query_keys = (char**)zbx_calloc(query_keys, query_count + 1, sizeof(char*));
	query_values = (char**)zbx_calloc(query_values, query_count + 1, sizeof(char*));

	for (i = 0; i < query_count; i++)
	{
		node = config_setting_get_elem(root, i);
		key = config_setting_name(node);

		if (CONFIG_TYPE_STRING != config_setting_type(node))
		{
			zabbix_log(LOG_LEVEL_ERR, "In %s: query '%s' is not a valid string", __func__, key);
			return EXIT_FAILURE;
		}

		value = config_setting_get_string_elem(root, i);

		if (EXIT_SUCCESS != (add_named_query(key, value)))
			return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

static int read_dbmon_config(const char *cfgfile)
{
	int                 i = 0;
	int                 res = EXIT_FAILURE;
	int                 cfglen = 0;
	const char          *key = NULL;
	config_t            cfg;
	config_setting_t    *root, *node;

	config_init(&cfg);

	zabbix_log(LOG_LEVEL_INFORMATION, "using sql configuration file: %s", cfgfile);

	if (CONFIG_TRUE != (config_read_file(&cfg, cfgfile)))
	{
		zabbix_log(LOG_LEVEL_ERR, "In %s: %s in %s:%i",	__func__, config_error_text(&cfg), cfgfile, config_error_line(&cfg));
		goto out;
	}

	root = config_root_setting(&cfg);
	cfglen = config_setting_length(root);

	for (i = 0; i < cfglen; i++)
	{
		node = config_setting_get_elem(root, i);
		key = config_setting_name(node);

		if (0 == strncmp(key, "queries", 8))
		{
			if (EXIT_SUCCESS != (read_config_queries(node)))
				goto out;
		}
		else
		{
			zabbix_log(LOG_LEVEL_ERR, "In %s: unrecognised configuration parameter: %s", __func__, key);
			goto out;
		}
	}

	res = EXIT_SUCCESS;
out:
	config_destroy(&cfg);
	return res;
}

int init_dbmon_config(const char *path)
{
	const char *cfgfile = get_dbmon_configfile(path);

	zabbix_log(LOG_LEVEL_DEBUG, "In %s: using dbmon configuration file: %s", __func__, cfgfile);

	if (EXIT_SUCCESS != (read_dbmon_config(cfgfile)))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

int uninit_dbmon_config()
{
	zbx_free(query_keys);
	zbx_free(query_values);
	query_count = 0;
	return EXIT_SUCCESS;
}
