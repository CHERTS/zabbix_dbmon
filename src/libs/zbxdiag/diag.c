/*
** Zabbix
** Copyright (C) 2001-2021 Zabbix SIA
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
#include "zbxalgo.h"
#include "dbcache.h"
#include "preproc.h"
#include "zbxdiag.h"
#include "diag.h"
#include "log.h"

void	diag_map_free(zbx_diag_map_t *map)
{
	zbx_free(map->name);
	zbx_free(map);
}

/******************************************************************************
 *                                                                            *
 * Function: diag_parse_request                                               *
 *                                                                            *
 * Purpose: parse diagnostic section request having json format               *
 *          {"stats":[<field1>,<field2>,...], "top":{<field1>:<limit1>,...}}  *
 *                                                                            *
 * Parameters: jp         - [IN] the request                                  *
 *             field_map  - [IN] a map of supported statistic field names to  *
 *                               bitmasks                                     *
 *             field_mask - [OUT] the bitmask of the requested fields         *
 *             top_views  - [OUT] the requested top views                     *
 *             error      - [OUT] error message                               *
 *                                                                            *
 * Return value: SUCCEED - the request was parsed successfully                *
 *               FAIL    - otherwise                                          *
 *                                                                            *
 ******************************************************************************/
int	diag_parse_request(const struct zbx_json_parse *jp, const zbx_diag_map_t *field_map,
		zbx_uint64_t *field_mask, zbx_vector_ptr_t *top_views, char **error)
{
	struct zbx_json_parse	jp_stats;
	int			ret = FAIL;
	const char		*pnext = NULL;
	char			name[ZBX_DIAG_FIELD_MAX + 1], value[MAX_ID_LEN + 1];
	zbx_uint64_t		value_ui64;

	*field_mask = 0;

	/* parse requested statistics fields */
	if (SUCCEED == zbx_json_brackets_by_name(jp, "stats", &jp_stats))
	{
		while (NULL != (pnext = zbx_json_next(&jp_stats, pnext)))
		{
			if (NULL != zbx_json_decodevalue(pnext, name, sizeof(name), NULL))
			{
				const zbx_diag_map_t	*stat;

				for (stat = field_map;; stat++)
				{
					if (NULL == stat->name)
					{
						*error = zbx_dsprintf(*error, "Unsupported statistics field: %s", name);
						goto out;
					}

					if (0 == strcmp(name, stat->name))
						break;
				}

				*field_mask |= stat->value;
			}
		}
	}
	else
	{
		if (SUCCEED == zbx_json_value_by_name(jp, "stats", value, sizeof(value), NULL) &&
				0 == strcmp(value, "extend"))
		{
			*field_mask |= field_map->value;
		}
		else
		{
			*error = zbx_dsprintf(*error, "Unknown statistic field value: %s", value);
			goto out;
		}
	}

	/* parse requested top views */
	if (SUCCEED == zbx_json_brackets_by_name(jp, "top", &jp_stats))
	{
		while (NULL != (pnext = zbx_json_pair_next(&jp_stats, pnext, name, sizeof(name))))
		{
			zbx_diag_map_t	*top;

			if (NULL == zbx_json_decodevalue(pnext, value, sizeof(value), NULL))
			{
				*error = zbx_strdup(*error, zbx_json_strerror());
				goto out;
			}
			if (FAIL == is_uint64(value, &value_ui64))
			{
				*error = zbx_dsprintf(*error, "Invalid top limit value: %s", value);
				goto out;
			}

			top = (zbx_diag_map_t *)zbx_malloc(NULL, sizeof(zbx_diag_map_t));
			top->name = zbx_strdup(NULL, name);
			top->value = value_ui64;
			zbx_vector_ptr_append(top_views, top);
		}
	}
	ret = SUCCEED;
out:
	if (FAIL == ret)
		zbx_vector_ptr_clear_ext(top_views, (zbx_ptr_free_func_t)diag_map_free);

	return ret;
}

/******************************************************************************
 *                                                                            *
 * Function: diag_add_mem_stats                                               *
 *                                                                            *
 * Purpose: add memory statistics to the json data                            *
 *                                                                            *
 * Parameters: json  - [IN/OUT] the json to update                            *
 *             name  - [IN] the memory object name                            *
 *             stats - [IN] the memory statistics                             *
 *                                                                            *
 ******************************************************************************/
void	diag_add_mem_stats(struct zbx_json *json, const char *name, const zbx_mem_stats_t *stats)
{
	int	i;

	if (NULL == stats)
		return;

	zbx_json_addobject(json, name);

	zbx_json_addobject(json, "size");
	zbx_json_adduint64(json, "free", stats->free_size);
	zbx_json_adduint64(json, "used", stats->used_size);
	zbx_json_close(json);

	zbx_json_addobject(json, "chunks");
	zbx_json_adduint64(json, "free", stats->free_chunks);
	zbx_json_adduint64(json, "used", stats->used_chunks);
	zbx_json_adduint64(json, "min", stats->min_chunk_size);
	zbx_json_adduint64(json, "max", stats->max_chunk_size);

	zbx_json_addarray(json, "buckets");

	for (i = 0; i < MEM_BUCKET_COUNT; i++)
	{
		if (0 != stats->chunks_num[i])
		{
			char	buf[MAX_ID_LEN + 2];

			zbx_snprintf(buf, sizeof(buf), "%d%s", MEM_MIN_BUCKET_SIZE + 8 * i,
					(MEM_BUCKET_COUNT - 1 == i ? "+" : ""));
			zbx_json_addobject(json, NULL);
			zbx_json_adduint64(json, buf, stats->chunks_num[i]);
			zbx_json_close(json);
		}
	}

	zbx_json_close(json);
	zbx_json_close(json);
	zbx_json_close(json);
}

/******************************************************************************
 *                                                                            *
 * Function: diag_compare_pair_second_desc                                    *
 *                                                                            *
 * Purpose: compare uint64 pairs by second value for descending sorting       *
 *                                                                            *
 ******************************************************************************/
static int	diag_compare_pair_second_desc(const void *d1, const void *d2)
{
	zbx_uint64_pair_t	*p1 = (zbx_uint64_pair_t *)d1;
	zbx_uint64_pair_t	*p2 = (zbx_uint64_pair_t *)d2;

	if (p1->second < p2->second)
		return 1;
	if (p1->second > p2->second)
		return -1;
	return 0;
}

/******************************************************************************
 *                                                                            *
 * Function: diag_historycahe_add_items                                       *
 *                                                                            *
 * Purpose: add history cache items diagnostic statistics to json             *
 *                                                                            *
 ******************************************************************************/
static void	diag_historycache_add_items(struct zbx_json *json, const char *field, const zbx_uint64_pair_t *pairs,
		int pairs_num)
{
	int	i;

	zbx_json_addarray(json, field);

	for (i = 0; i < pairs_num; i++)
	{
		zbx_json_addobject(json, NULL);
		zbx_json_addint64(json, "itemid", pairs[i].first);
		zbx_json_addint64(json, "values", pairs[i].second);
		zbx_json_close(json);
	}

	zbx_json_close(json);
}

/******************************************************************************
 *                                                                            *
 * Function: diag_add_historycache_info                                       *
 *                                                                            *
 * Purpose: add requested history cache diagnostic information to json data   *
 *                                                                            *
 * Parameters: jp    - [IN] the request                                       *
 *             json  - [IN/OUT] the json to update                            *
 *             error - [OUT] error message                                    *
 *                                                                            *
 * Return value: SUCCEED - the information was added successfully             *
 *               FAIL    - otherwise                                          *
 *                                                                            *
 ******************************************************************************/
int	diag_add_historycache_info(const struct zbx_json_parse *jp, struct zbx_json *json, char **error)
{
	zbx_vector_ptr_t	tops;
	int			ret;
	double			time1, time2, time_total = 0;
	zbx_uint64_t		fields;
	zbx_diag_map_t		field_map[] = {
					{"", ZBX_DIAG_HISTORYCACHE_SIMPLE | ZBX_DIAG_HISTORYCACHE_MEMORY},
					{"items", ZBX_DIAG_HISTORYCACHE_ITEMS},
					{"values", ZBX_DIAG_HISTORYCACHE_VALUES},
					{"memory", ZBX_DIAG_HISTORYCACHE_MEMORY},
					{"memory.data", ZBX_DIAG_HISTORYCACHE_MEMORY_DATA},
					{"memory.index", ZBX_DIAG_HISTORYCACHE_MEMORY_INDEX},
					{NULL, 0}
					};

	zbx_vector_ptr_create(&tops);

	if (SUCCEED == (ret = diag_parse_request(jp, field_map, &fields, &tops, error)))
	{
		int	i;

		zbx_json_addobject(json, ZBX_DIAG_HISTORYCACHE);

		if (0 != (fields & ZBX_DIAG_HISTORYCACHE_SIMPLE))
		{
			zbx_uint64_t	values_num, items_num;

			time1 = zbx_time();
			zbx_hc_get_diag_stats(&items_num, &values_num);
			time2 = zbx_time();
			time_total += time2 - time1;

			if (0 != (fields & ZBX_DIAG_HISTORYCACHE_ITEMS))
				zbx_json_addint64(json, "items", items_num);
			if (0 != (fields & ZBX_DIAG_HISTORYCACHE_VALUES))
				zbx_json_addint64(json, "values", values_num);
		}

		if (0 != (fields & ZBX_DIAG_HISTORYCACHE_MEMORY))
		{
			zbx_mem_stats_t	data_mem, index_mem, *pdata_mem, *pindex_mem;

			pdata_mem = (0 != (fields & ZBX_DIAG_HISTORYCACHE_MEMORY_DATA) ? &data_mem : NULL);
			pindex_mem = (0 != (fields & ZBX_DIAG_HISTORYCACHE_MEMORY_INDEX) ? &index_mem : NULL);

			time1 = zbx_time();
			zbx_hc_get_mem_stats(pdata_mem, pindex_mem);
			time2 = zbx_time();
			time_total += time2 - time1;

			zbx_json_addobject(json, "memory");
			diag_add_mem_stats(json, "data", pdata_mem);
			diag_add_mem_stats(json, "index", pindex_mem);
			zbx_json_close(json);
		}

		if (0 != tops.values_num)
		{
			zbx_json_addobject(json, "top");

			for (i = 0; i < tops.values_num; i++)
			{
				zbx_diag_map_t	*map = (zbx_diag_map_t *)tops.values[i];

				if (0 == strcmp(map->name, "values"))
				{
					zbx_vector_uint64_pair_t	items;
					int				limit;

					zbx_vector_uint64_pair_create(&items);

					time1 = zbx_time();
					zbx_hc_get_items(&items);
					time2 = zbx_time();
					time_total += time2 - time1;

					zbx_vector_uint64_pair_sort(&items, diag_compare_pair_second_desc);
					limit = MIN((int)map->value, items.values_num);

					diag_historycache_add_items(json, map->name, (zbx_uint64_pair_t *)items.values,
							limit);
					zbx_vector_uint64_pair_destroy(&items);
				}
				else
				{
					*error = zbx_dsprintf(*error, "Unsupported top field: %s", map->name);
					ret = FAIL;
					break;
				}
			}

			zbx_json_close(json);
		}

		zbx_json_addfloat(json, "time", time_total);

		zbx_json_close(json);
	}

	zbx_vector_ptr_clear_ext(&tops, (zbx_ptr_free_func_t)diag_map_free);
	zbx_vector_ptr_destroy(&tops);

	return ret;
}

/******************************************************************************
 *                                                                            *
 * Function: diag_add_preproc_items                                           *
 *                                                                            *
 * Purpose: add item top list to output json                                  *
 *                                                                            *
 * Parameters: json  - [OUT] the output json                                  *
 *             field - [IN] the field name                                    *
 *             items - [IN] a top item list                                   *
 *                                                                            *
 ******************************************************************************/
static void	diag_add_preproc_items(struct zbx_json *json, const char *field, const zbx_vector_ptr_t *items)
{
	int	i;

	zbx_json_addarray(json, field);

	for (i = 0; i < items->values_num; i++)
	{
		const zbx_preproc_item_stats_t	*item = (const zbx_preproc_item_stats_t *)items->values[i];

		zbx_json_addobject(json, NULL);
		zbx_json_adduint64(json, "itemid", item->itemid);
		zbx_json_adduint64(json, "values", item->values_num);
		zbx_json_adduint64(json, "steps", item->steps_num);
		zbx_json_close(json);
	}

	zbx_json_close(json);
}

/******************************************************************************
 *                                                                            *
 * Function: diag_add_preproc_info                                            *
 *                                                                            *
 * Purpose: add requested preprocessing diagnostic information to json data   *
 *                                                                            *
 * Parameters: jp    - [IN] the request                                       *
 *             json  - [IN/OUT] the json to update                            *
 *             error - [OUT] error message                                    *
 *                                                                            *
 * Return value: SUCCEED - the information was added successfully             *
 *               FAIL    - otherwise                                          *
 *                                                                            *
 ******************************************************************************/
int	diag_add_preproc_info(const struct zbx_json_parse *jp, struct zbx_json *json, char **error)
{
	zbx_vector_ptr_t	tops;
	int			ret = FAIL;
	double			time1, time2, time_total = 0;
	zbx_uint64_t		fields;
	zbx_diag_map_t		field_map[] = {
					{"", ZBX_DIAG_PREPROC_VALUES | ZBX_DIAG_PREPROC_VALUES_PREPROC},
					{"values", ZBX_DIAG_PREPROC_VALUES},
					{"preproc.values", ZBX_DIAG_PREPROC_VALUES_PREPROC},
					{NULL, 0}
					};

	zbx_vector_ptr_create(&tops);

	if (SUCCEED == (ret = diag_parse_request(jp, field_map, &fields, &tops, error)))
	{
		zbx_json_addobject(json, ZBX_DIAG_PREPROCESSING);

		if (0 != (fields & ZBX_DIAG_PREPROC_SIMPLE))
		{
			int	values_num, values_preproc_num;

			time1 = zbx_time();
			if (FAIL == (ret = zbx_preprocessor_get_diag_stats(&values_num, &values_preproc_num, error)))
				goto out;

			time2 = zbx_time();
			time_total += time2 - time1;

			if (0 != (fields & ZBX_DIAG_PREPROC_VALUES))
				zbx_json_addint64(json, "values", values_num);
			if (0 != (fields & ZBX_DIAG_PREPROC_VALUES_PREPROC))
				zbx_json_addint64(json, "preproc.values", values_preproc_num);
		}

		if (0 != tops.values_num)
		{
			int	i;

			zbx_json_addobject(json, "top");

			for (i = 0; i < tops.values_num; i++)
			{
				zbx_diag_map_t	*map = (zbx_diag_map_t *)tops.values[i];

				if (0 == strcmp(map->name, "values"))
				{
					zbx_vector_ptr_t	items;

					zbx_vector_ptr_create(&items);
					time1 = zbx_time();
					if (FAIL == (ret = zbx_preprocessor_get_top_items(map->value, &items, error)))
					{
						zbx_vector_ptr_destroy(&items);
						goto out;
					}
					time2 = zbx_time();
					time_total += time2 - time1;

					diag_add_preproc_items(json, map->name, &items);
					zbx_vector_ptr_clear_ext(&items, zbx_ptr_free);
					zbx_vector_ptr_destroy(&items);
				}
				else
				{
					*error = zbx_dsprintf(*error, "Unsupported top field: %s", map->name);
					ret = FAIL;
					goto out;
				}
			}

			zbx_json_close(json);
		}

		zbx_json_addfloat(json, "time", time_total);
		zbx_json_close(json);
	}
out:
	zbx_vector_ptr_clear_ext(&tops, (zbx_ptr_free_func_t)diag_map_free);
	zbx_vector_ptr_destroy(&tops);

	return ret;
}

static void	zbx_json_addhex(struct zbx_json *j, const char *name, zbx_uint64_t value)
{
	char	buffer[MAX_ID_LEN];

	zbx_snprintf(buffer, sizeof(buffer), "0x" ZBX_FS_UX64, value);
	zbx_json_addstring(j, name, buffer, ZBX_JSON_TYPE_STRING);
}

/******************************************************************************
 *                                                                            *
 * Function: diag_add_locks_info                                              *
 *                                                                            *
 * Purpose: add requested locks diagnostic information to json data           *
 *                                                                            *
 * Parameters: json  - [IN/OUT] the json to update                            *
 *                                                                            *
 ******************************************************************************/
void	diag_add_locks_info(struct zbx_json *json)
{
	int		i;
#ifdef HAVE_VMINFO_T_UPDATES
	const char	*names[ZBX_MUTEX_COUNT] = {"ZBX_MUTEX_LOG", "ZBX_MUTEX_CACHE", "ZBX_MUTEX_TRENDS",
				"ZBX_MUTEX_CACHE_IDS", "ZBX_MUTEX_SELFMON", "ZBX_MUTEX_CPUSTATS", "ZBX_MUTEX_DISKSTATS",
				"ZBX_MUTEX_ITSERVICES", "ZBX_MUTEX_VALUECACHE", "ZBX_MUTEX_VMWARE", "ZBX_MUTEX_SQLITE3",
				"ZBX_MUTEX_PROCSTAT", "ZBX_MUTEX_PROXY_HISTORY", "ZBX_MUTEX_KSTAT"};
#else
	const char	*names[ZBX_MUTEX_COUNT] = {"ZBX_MUTEX_LOG", "ZBX_MUTEX_CACHE", "ZBX_MUTEX_TRENDS",
				"ZBX_MUTEX_CACHE_IDS", "ZBX_MUTEX_SELFMON", "ZBX_MUTEX_CPUSTATS", "ZBX_MUTEX_DISKSTATS",
				"ZBX_MUTEX_ITSERVICES", "ZBX_MUTEX_VALUECACHE", "ZBX_MUTEX_VMWARE", "ZBX_MUTEX_SQLITE3",
				"ZBX_MUTEX_PROCSTAT", "ZBX_MUTEX_PROXY_HISTORY"};
#endif

	zbx_json_addarray(json, ZBX_DIAG_LOCKS);

	for (i = 0; i < ZBX_MUTEX_COUNT; i++)
	{
		zbx_json_addobject(json, NULL);
		zbx_json_addhex(json, names[i], (zbx_uint64_t)zbx_mutex_addr_get(i));
		zbx_json_close(json);
	}

	zbx_json_addobject(json, NULL);
	zbx_json_addhex(json, "ZBX_RWLOCK_CONFIG", (zbx_uint64_t)zbx_rwlock_addr_get(ZBX_RWLOCK_CONFIG));
	zbx_json_close(json);
	zbx_json_addobject(json, NULL);
	zbx_json_addhex(json, "ZBX_RWLOCK_VALUECACHE", (zbx_uint64_t)zbx_rwlock_addr_get(ZBX_RWLOCK_VALUECACHE));
	zbx_json_close(json);

	zbx_json_close(json);
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_diag_get_info                                                *
 *                                                                            *
 * Purpose: get diagnostic information                                        *
 *                                                                            *
 * Parameters: jp   - [IN] the request                                        *
 *             info - [OUT] the requested information or error message        *
 *                                                                            *
 * Return value: SUCCEED - the information was retrieved successfully         *
 *               FAIL    - otherwise                                          *
 *                                                                            *
 ******************************************************************************/
int	zbx_diag_get_info(const struct zbx_json_parse *jp, char **info)
{
	struct zbx_json_parse	jp_section;
	char			section[ZBX_DIAG_SECTION_MAX + 1];
	const char		*pnext = NULL;
	struct zbx_json		json;
	int			ret = SUCCEED;

	zbx_json_init(&json, 1024);

	while (NULL != (pnext = zbx_json_pair_next(jp, pnext, section, sizeof(section))))
	{
		if (FAIL == (ret = zbx_json_brackets_open(pnext, &jp_section)))
		{
			*info = zbx_strdup(*info, zbx_json_strerror());
			goto out;
		}

		if (FAIL == (ret = diag_add_section_info(section, &jp_section, &json, info)))
			goto out;
	}
out:
	if (SUCCEED == ret)
		*info = zbx_strdup(*info, json.buffer);

	zbx_json_free(&json);

	return ret;
}

#define ZBX_DIAG_DEFAULT_TOP_LIMIT	25

/******************************************************************************
 *                                                                            *
 * Function: diag_add_section_request                                         *
 *                                                                            *
 * Purpose: add default diagnostic section request                            *
 *                                                                            *
 * Parameters: j       - [OUT] the request json                               *
 *             section - [IN] the section name                                *
 *             ...     - [IN] null terminated list of top field names         *
 *                                                                            *
 ******************************************************************************/
static void	diag_add_section_request(struct zbx_json *j, const char *section, ...)
{
	va_list		args;
	const char	*field;

	zbx_json_addobject(j, section);
	zbx_json_addstring(j, "stats", "extend", ZBX_JSON_TYPE_STRING);

	zbx_json_addobject(j, "top");

	va_start(args, section);

	while (NULL != (field = va_arg(args, const char *)))
		zbx_json_adduint64(j, field, ZBX_DIAG_DEFAULT_TOP_LIMIT);

	va_end(args);

	zbx_json_close(j);
	zbx_json_close(j);
}

/******************************************************************************
 *                                                                            *
 * Function: diag_prepare_default_request                                     *
 *                                                                            *
 * Purpose: prepare default diagnostic request for all sections               *
 *                                                                            *
 ******************************************************************************/
static void	diag_prepare_default_request(struct zbx_json *j, unsigned int flags)
{
	if (0 != (flags & (1 << ZBX_DIAGINFO_HISTORYCACHE)))
		diag_add_section_request(j, ZBX_DIAG_HISTORYCACHE, "values", NULL);

	if (0 != (flags & (1 << ZBX_DIAGINFO_VALUECACHE)))
		diag_add_section_request(j, ZBX_DIAG_VALUECACHE, "values", "request.values", NULL);

	if (0 != (flags & (1 << ZBX_DIAGINFO_PREPROCESSING)))
		diag_add_section_request(j, ZBX_DIAG_PREPROCESSING, "values", NULL);

	if (0 != (flags & (1 << ZBX_DIAGINFO_LLD)))
		diag_add_section_request(j, ZBX_DIAG_LLD, "values", NULL);

	if (0 != (flags & (1 << ZBX_DIAGINFO_ALERTING)))
		diag_add_section_request(j, ZBX_DIAG_ALERTING, "media.alerts", "source.alerts", NULL);

	if (0 != (flags & (1 << ZBX_DIAGINFO_LOCKS)))
		diag_add_section_request(j, ZBX_DIAG_LOCKS, NULL);
}

/******************************************************************************
 *                                                                            *
 * Function: diag_get_simple_values                                           *
 *                                                                            *
 * Purpose: extract simple values in format <key1>:<value1> <key2>:<value2>...*
 *          from the specified json location                                  *
 *                                                                            *
 * Parameters: jp  - [IN] the json location                                   *
 *             msg - [OUT] the extracted values                               *
 *                                                                            *
 ******************************************************************************/
static void	diag_get_simple_values(const struct zbx_json_parse *jp, char **msg)
{
	const char		*pnext = NULL;
	char			key[MAX_STRING_LEN], *value = NULL;
	struct zbx_json_parse	jp_value;
	size_t			value_alloc = 0, msg_alloc = 0, msg_offset = 0;
	zbx_json_type_t		type;

	while (NULL != (pnext = zbx_json_pair_next(jp, pnext, key, sizeof(key))))
	{
		if (FAIL == zbx_json_brackets_open(pnext, &jp_value))
		{
			zbx_json_decodevalue_dyn(pnext, &value, &value_alloc, &type);

			if (0 != msg_offset)
				zbx_chrcpy_alloc(msg, &msg_alloc, &msg_offset, ' ');

			zbx_snprintf_alloc(msg, &msg_alloc, &msg_offset, "%s:%s", key,
					(ZBX_JSON_TYPE_NULL != type ? value: "null"));
		}
	}

	zbx_free(value);
}

/******************************************************************************
 *                                                                            *
 * Function: diag_log_memory_info                                             *
 *                                                                            *
 * Purpose: log shared memory information                                     *
 *                                                                            *
 * Parameters: jp    - [IN] the section json                                  *
 *             field - [OUT] the memory field name                            *
 *             path  - [OUT] the json path to the memory data                 *
 *                                                                            *
 ******************************************************************************/
static void	diag_log_memory_info(struct zbx_json_parse *jp, const char *field, const char *path)
{
	struct zbx_json_parse	jp_memory, jp_size, jp_chunks;
	char			*msg = NULL;

	if (FAIL == zbx_json_open_path(jp, path, &jp_memory))
		return;

	zabbix_log(LOG_LEVEL_INFORMATION, "%s:", field);
	if (SUCCEED == zbx_json_brackets_by_name(&jp_memory, "size", &jp_size))
	{
		diag_get_simple_values(&jp_size, &msg);
		zabbix_log(LOG_LEVEL_INFORMATION, "  size: %s", msg);
		zbx_free(msg);
	}

	if (SUCCEED == zbx_json_brackets_by_name(&jp_memory, "chunks", &jp_chunks))
	{
		struct zbx_json_parse	jp_buckets, jp_bucket;

		diag_get_simple_values(&jp_chunks, &msg);
		zabbix_log(LOG_LEVEL_INFORMATION, "  chunks: %s", msg);
		zbx_free(msg);

		if (SUCCEED == zbx_json_brackets_by_name(&jp_chunks, "buckets", &jp_buckets))
		{
			const char	*pnext;

			zabbix_log(LOG_LEVEL_INFORMATION, "    buckets:");

			for (pnext = NULL; NULL != (pnext = zbx_json_next(&jp_buckets, pnext));)
			{
				if (SUCCEED == zbx_json_brackets_open(pnext, &jp_bucket))
				{
					diag_get_simple_values(&jp_bucket, &msg);
					zabbix_log(LOG_LEVEL_INFORMATION, "      %s", msg);
					zbx_free(msg);
				}
			}
		}
	}
}

/******************************************************************************
 *                                                                            *
 * Function: diag_log_top_view                                                *
 *                                                                            *
 * Purpose: log top view                                                      *
 *                                                                            *
 * Parameters: jp    - [IN] the section json                                  *
 *             field - [OUT] the top field name                               *
 *             path  - [OUT] the json path to the top view                    *
 *                                                                            *
 ******************************************************************************/
static void	diag_log_top_view(struct zbx_json_parse *jp, const char *field, const char *path)
{
	struct zbx_json_parse	jp_top, jp_row;
	const char		*pnext;
	char			*msg = NULL;

	if (NULL == path)
	{
		jp_top = *jp;
	}
	else if (FAIL == zbx_json_open_path(jp, path, &jp_top))
		return;

	zabbix_log(LOG_LEVEL_INFORMATION, "%s:", field);

	for (pnext = NULL; NULL != (pnext = zbx_json_next(&jp_top, pnext));)
	{
		if (SUCCEED == zbx_json_brackets_open(pnext, &jp_row))
		{
			diag_get_simple_values(&jp_row, &msg);
			zabbix_log(LOG_LEVEL_INFORMATION, "  %s", msg);
			zbx_free(msg);
		}
	}
}

/******************************************************************************
 *                                                                            *
 * Function: diag_log_history_cache                                           *
 *                                                                            *
 * Purpose: log history cache diagnostic information                          *
 *                                                                            *
 ******************************************************************************/
static void	diag_log_history_cache(struct zbx_json_parse *jp)
{
	char	*msg = NULL;

	zabbix_log(LOG_LEVEL_INFORMATION, "== history cache diagnostic information ==");

	diag_get_simple_values(jp, &msg);
	zabbix_log(LOG_LEVEL_INFORMATION, "%s", msg);
	zbx_free(msg);

	diag_log_memory_info(jp, "memory.data", "$.memory.data");
	diag_log_memory_info(jp, "memory.index", "$.memory.index");

	diag_log_top_view(jp, "top.values", "$.top.values");

	zabbix_log(LOG_LEVEL_INFORMATION, "==");
}

/******************************************************************************
 *                                                                            *
 * Function: diag_log_value_cache                                             *
 *                                                                            *
 * Purpose: log value cache diagnostic information                            *
 *                                                                            *
 ******************************************************************************/
static void	diag_log_value_cache(struct zbx_json_parse *jp)
{
	char	*msg = NULL;

	zabbix_log(LOG_LEVEL_INFORMATION, "== value cache diagnostic information ==");

	diag_get_simple_values(jp, &msg);
	zabbix_log(LOG_LEVEL_INFORMATION, "%s", msg);
	zbx_free(msg);

	diag_log_memory_info(jp, "memory", "$.memory");

	diag_log_top_view(jp, "top.values", "$.top.values");
	diag_log_top_view(jp, "top.request.values", "$.top['request.values']");

	zabbix_log(LOG_LEVEL_INFORMATION, "==");
}

/******************************************************************************
 *                                                                            *
 * Function: diag_log_preprocessing                                           *
 *                                                                            *
 * Purpose: log preprocessing diagnostic information                          *
 *                                                                            *
 ******************************************************************************/
static void	diag_log_preprocessing(struct zbx_json_parse *jp)
{
	char	*msg = NULL;

	zabbix_log(LOG_LEVEL_INFORMATION, "== preprocessing diagnostic information ==");

	diag_get_simple_values(jp, &msg);
	zabbix_log(LOG_LEVEL_INFORMATION, "%s", msg);
	zbx_free(msg);

	diag_log_top_view(jp, "top.values", "$.top.values");

	zabbix_log(LOG_LEVEL_INFORMATION, "==");
}

/******************************************************************************
 *                                                                            *
 * Function: diag_log_lld                                                     *
 *                                                                            *
 * Purpose: log LLD diagnostic information                                    *
 *                                                                            *
 ******************************************************************************/

static void	diag_log_lld(struct zbx_json_parse *jp)
{
	char	*msg = NULL;

	zabbix_log(LOG_LEVEL_INFORMATION, "== LLD diagnostic information ==");

	diag_get_simple_values(jp, &msg);
	zabbix_log(LOG_LEVEL_INFORMATION, "%s", msg);
	zbx_free(msg);

	diag_log_top_view(jp, "top.values", "$.top.values");

	zabbix_log(LOG_LEVEL_INFORMATION, "==");
}

/******************************************************************************
 *                                                                            *
 * Function: diag_log_alerting                                                *
 *                                                                            *
 * Purpose: log alerting diagnostic information                               *
 *                                                                            *
 ******************************************************************************/
static void	diag_log_alerting(struct zbx_json_parse *jp)
{
	char	*msg = NULL;

	zabbix_log(LOG_LEVEL_INFORMATION, "== alerting diagnostic information ==");

	diag_get_simple_values(jp, &msg);
	zabbix_log(LOG_LEVEL_INFORMATION, "%s", msg);
	zbx_free(msg);

	diag_log_top_view(jp, "media.alerts", "$.top['media.alerts']");
	diag_log_top_view(jp, "source.alerts", "$.top['source.alerts']");

	zabbix_log(LOG_LEVEL_INFORMATION, "==");
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_diag_log_info                                                *
 *                                                                            *
 * Purpose: log diagnostic information                                        *
 *                                                                            *
 * Parameters: flags - [IN] flags describing section to log                   *
 *                                                                            *
 ******************************************************************************/
void	zbx_diag_log_info(unsigned int flags)
{
	struct zbx_json		j;
	struct zbx_json_parse	jp;
	char			*info = NULL;

	zbx_json_init(&j, 1024);

	diag_prepare_default_request(&j, flags);
	if (FAIL == zbx_json_open(j.buffer, &jp))
	{
		THIS_SHOULD_NEVER_HAPPEN;
		goto out;
	}

	if (SUCCEED == zbx_diag_get_info(&jp, &info))
	{
		char			section[ZBX_DIAG_SECTION_MAX + 1];
		struct zbx_json_parse	jp_section;
		const char		*pnext = NULL;

		if (FAIL == zbx_json_open(info, &jp))
		{
			THIS_SHOULD_NEVER_HAPPEN;
			goto out;
		}

		while (NULL != (pnext = zbx_json_pair_next(&jp, pnext, section, sizeof(section))))
		{
			if (FAIL == zbx_json_brackets_open(pnext, &jp_section))
			{
				THIS_SHOULD_NEVER_HAPPEN;
				continue;
			}

			if (0 == strcmp(section, ZBX_DIAG_HISTORYCACHE))
				diag_log_history_cache(&jp_section);
			else if (0 == strcmp(section, ZBX_DIAG_VALUECACHE))
				diag_log_value_cache(&jp_section);
			else if (0 == strcmp(section, ZBX_DIAG_PREPROCESSING))
				diag_log_preprocessing(&jp_section);
			else if (0 == strcmp(section, ZBX_DIAG_LLD))
				diag_log_lld(&jp_section);
			else if (0 == strcmp(section, ZBX_DIAG_ALERTING))
				diag_log_alerting(&jp_section);
			else if (0 == strcmp(section, ZBX_DIAG_LOCKS))
			{
				zabbix_log(LOG_LEVEL_INFORMATION, "== locks diagnostic information ==");
				diag_log_top_view(&jp_section, ZBX_DIAG_LOCKS, NULL);
				zabbix_log(LOG_LEVEL_INFORMATION, "==");
			}
		}
	}
	else
		zabbix_log(LOG_LEVEL_INFORMATION, "cannot obtain diagnostic information: %s", info);
out:
	zbx_free(info);
	zbx_json_free(&j);
}

