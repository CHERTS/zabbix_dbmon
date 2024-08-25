/*
** Zabbix
** Copyright (C) 2019-2022 Mikhail Grigorev
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
#include "zbxjson.h"
#include "zbxdbmon.h"
#include "dbmon_common.h"

int make_nojson_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result)
{
	int				ret = SYSINFO_RET_FAIL;
	unsigned int	col, row;
	char			date_buf[64];

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);
	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Rows: %u, Cols: %u", __func__, request->key, db_result.nb_rows, db_result.nb_columns);

	for (col = 0; col < db_result.nb_columns; col++)
	{
		zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): ColNum: %u, ColName: %s", __func__, request->key, col, (((struct zbx_db_type_text *)db_result.fields[0][col].t_data)->value));
	}

	for (row = 0; row < db_result.nb_rows; row++)
	{
		for (col = 0; col < db_result.nb_columns; col++)
		{
			switch (db_result.data[row][col].type)
			{
				case ZBX_COL_TYPE_INT:
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(INT): %d, Value: %lld", __func__, request->key, row, col, ((struct zbx_db_type_int *)db_result.data[row][col].t_data)->value);
					SET_UI64_RESULT(result, ((struct zbx_db_type_int *)db_result.data[row][col].t_data)->value);
					ret = SYSINFO_RET_OK;
					break;
				case ZBX_COL_TYPE_DOUBLE:
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(DOUBLE): %d, Value: %f", __func__, request->key, row, col, ((struct zbx_db_type_double *)db_result.data[row][col].t_data)->value);
					SET_DBL_RESULT(result, ((struct zbx_db_type_double *)db_result.data[row][col].t_data)->value);
					ret = SYSINFO_RET_OK;
					break;
				case ZBX_COL_TYPE_TEXT:
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(TEXT): %d, Value: %s", __func__, request->key, row, col, ((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value);
					SET_TEXT_RESULT(result, zbx_strdup(NULL, ((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value));
					ret = SYSINFO_RET_OK;
					break;
				case ZBX_COL_TYPE_DATE:
					strftime(date_buf, sizeof(date_buf), "%Y-%m-%d %H:%M:%S", &((struct zbx_db_type_datetime *)db_result.data[row][col].t_data)->value);
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(DATE): %d, Value: %s", __func__, request->key, row, col, date_buf);
					SET_STR_RESULT(result, zbx_strdup(NULL, date_buf));
					ret = SYSINFO_RET_OK;
					break;
				case ZBX_COL_TYPE_BLOB:
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(BLOB): %d, Value: BLOB", __func__, request->key, row, col);
					SET_STR_RESULT(result, zbx_strdup(NULL, "BLOB"));
					ret = SYSINFO_RET_OK;
					break;
				case ZBX_COL_TYPE_NULL:
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(NULL): %d, Value: NULL", __func__, request->key, row, col);
					SET_STR_RESULT(result, zbx_strdup(NULL, "NULL"));
					ret = SYSINFO_RET_OK;
					break;
			}
		}
	}

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s): %s", __func__, request->key, zbx_sysinfo_ret_string(ret));

	return ret;
}

int make_onerow_json_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result)
{
	int				ret = SYSINFO_RET_FAIL;
	struct zbx_json	json;
	unsigned int	col, row;
	char			buffer[MAX_STRING_LEN], date_buf[64];
	char			*value_str, *c = NULL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);
	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Rows: %u, Cols: %u", __func__, request->key, db_result.nb_rows, db_result.nb_columns);

	for (col = 0; col < db_result.nb_columns; col++)
	{
		zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): ColNum: %u, ColName: %s", __func__, request->key, col, (((struct zbx_db_type_text *)db_result.fields[0][col].t_data)->value));
	}

	zbx_json_init(&json, ZBX_JSON_STAT_BUF_LEN);

	for (row = 0; row < db_result.nb_rows; row++)
	{
		for (col = 0; col < db_result.nb_columns; col++)
		{
			zbx_snprintf(buffer, sizeof(buffer), "%s", zbx_strdup(NULL, (((struct zbx_db_type_text *)db_result.fields[0][col].t_data)->value)));

			for (c = &buffer[0]; *c; c++)
			{
				if (0 != isalpha((unsigned char)*c))
					*c = toupper((unsigned char)*c);
			}

			zbx_snprintf(buffer, sizeof(buffer), "%s", zbx_strdup(NULL, buffer));

			switch (db_result.data[row][col].type)
			{
				case ZBX_COL_TYPE_INT:
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(INT): %d, Value: %lld", __func__, request->key, row, col, ((struct zbx_db_type_int *)db_result.data[row][col].t_data)->value);
					value_str = zbx_dsprintf(NULL, "%lld", (((struct zbx_db_type_int *)db_result.data[row][col].t_data)->value));
					zbx_json_addstring(&json, buffer, zbx_strdup(NULL, value_str), ZBX_JSON_TYPE_STRING);
					zbx_free(value_str);
					break;
				case ZBX_COL_TYPE_DOUBLE:
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(DOUBLE): %d, Value: " ZBX_FS_DBL, __func__, request->key, row, col, ((struct zbx_db_type_double *)db_result.data[row][col].t_data)->value);
					value_str = zbx_dsprintf(NULL, ZBX_FS_DBL, (((struct zbx_db_type_double *)db_result.data[row][col].t_data)->value));
					zbx_json_addstring(&json, buffer, zbx_strdup(NULL, value_str), ZBX_JSON_TYPE_STRING);
					zbx_free(value_str);
					break;
				case ZBX_COL_TYPE_TEXT:
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(TEXT): %d, Value: %s", __func__, request->key, row, col, ((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value);
					zbx_json_addstring(&json, buffer, zbx_strdup(NULL, (((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value)), ZBX_JSON_TYPE_STRING);
					break;
				case ZBX_COL_TYPE_DATE:
					strftime(date_buf, sizeof(date_buf), "%Y-%m-%d %H:%M:%S", &((struct zbx_db_type_datetime *)db_result.data[row][col].t_data)->value);
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(DATE): %d, Value: %s", __func__, request->key, row, col, date_buf);
					value_str = zbx_strdup(NULL, date_buf);
					zbx_json_addstring(&json, buffer, value_str, ZBX_JSON_TYPE_STRING);
					zbx_free(value_str);
					break;
				case ZBX_COL_TYPE_BLOB:
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(BLOB): %d, Value: BLOB", __func__, request->key, row, col);
					zbx_json_addstring(&json, buffer, zbx_strdup(NULL, "BLOB"), ZBX_JSON_TYPE_STRING);
					break;
				case ZBX_COL_TYPE_NULL:
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(NULL): %d, Value: NULL", __func__, request->key, row, col);
					zbx_json_addstring(&json, buffer, zbx_strdup(NULL, "NULL"), ZBX_JSON_TYPE_STRING);
					break;
			}
		}
	}

	zbx_json_close(&json);
	SET_STR_RESULT(result, strdup(json.buffer));
	zbx_json_free(&json);
	ret = SYSINFO_RET_OK;

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s): %s", __func__, request->key, zbx_sysinfo_ret_string(ret));

	return ret;
}

int make_multirow_twocoll_json_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result)
{
	int				ret = SYSINFO_RET_FAIL;
	struct zbx_json	json;
	unsigned int	row;
	char			buffer[MAX_STRING_LEN], date_buf[64];
	char			*value_str, *c = NULL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);
	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Rows: %u, Cols: %u", __func__, request->key, db_result.nb_rows, db_result.nb_columns);

	zbx_json_init(&json, ZBX_JSON_STAT_BUF_LEN);

	for (row = 0; row < db_result.nb_rows; row++)
	{
		switch (db_result.data[row][0].type)
		{
			case ZBX_COL_TYPE_TEXT:
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(TEXT): %d, Value: %s", __func__, request->key, row, 0, ((struct zbx_db_type_text *)db_result.data[row][0].t_data)->value);
				zbx_snprintf(buffer, sizeof(buffer), "%s", zbx_strdup(NULL, (((struct zbx_db_type_text *)db_result.data[row][0].t_data)->value)));
				break;
			default:
				zbx_snprintf(buffer, sizeof(buffer), "%s", zbx_strdup(NULL, (((struct zbx_db_type_text *)db_result.fields[0][0].t_data)->value)));
				break;
		}

		for (c = &buffer[0]; *c; c++)
		{
			if (0 != isalpha((unsigned char)*c))
				*c = toupper((unsigned char)*c);
		}

		zbx_snprintf(buffer, sizeof(buffer), "%s", zbx_strdup(NULL, buffer));

		switch (db_result.data[row][1].type)
		{
			case ZBX_COL_TYPE_INT:
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(INT): %d, Value: %lld", __func__, request->key, row, 1, ((struct zbx_db_type_int *)db_result.data[row][1].t_data)->value);
				value_str = zbx_dsprintf(NULL, "%lld", (((struct zbx_db_type_int *)db_result.data[row][1].t_data)->value));
				char *tmp = zbx_strdup(NULL, value_str);
				zbx_json_addstring(&json, buffer, tmp, ZBX_JSON_TYPE_STRING);
				zbx_free(tmp);
				zbx_free(value_str);
				break;
			case ZBX_COL_TYPE_DOUBLE:
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(DOUBLE): %d, Value: " ZBX_FS_DBL, __func__, request->key, row, 1, ((struct zbx_db_type_double *)db_result.data[row][1].t_data)->value);
				value_str = zbx_dsprintf(NULL, ZBX_FS_DBL, (((struct zbx_db_type_double *)db_result.data[row][1].t_data)->value));
				zbx_json_addstring(&json, buffer, zbx_strdup(NULL, value_str), ZBX_JSON_TYPE_STRING);
				zbx_free(value_str);
				break;
			case ZBX_COL_TYPE_TEXT:
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(TEXT): %d, Value: %s", __func__, request->key, row, 1, ((struct zbx_db_type_text *)db_result.data[row][1].t_data)->value);
				zbx_json_addstring(&json, buffer, zbx_strdup(NULL, (((struct zbx_db_type_text *)db_result.data[row][1].t_data)->value)), ZBX_JSON_TYPE_STRING);
				break;
			case ZBX_COL_TYPE_DATE:
				strftime(date_buf, sizeof(date_buf), "%Y-%m-%d %H:%M:%S", &((struct zbx_db_type_datetime *)db_result.data[row][1].t_data)->value);
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(DATE): %d, Value: %s", __func__, request->key, row, 1, date_buf);
				zbx_json_addstring(&json, buffer, zbx_strdup(NULL, date_buf), ZBX_JSON_TYPE_STRING);
				break;
			case ZBX_COL_TYPE_BLOB:
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(BLOB): %d, Value: BLOB", __func__, request->key, row, 1);
				zbx_json_addstring(&json, buffer, zbx_strdup(NULL, "BLOB"), ZBX_JSON_TYPE_STRING);
				break;
			case ZBX_COL_TYPE_NULL:
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col(NULL): %d, Value: NULL", __func__, request->key, row, 1);
				zbx_json_addstring(&json, buffer, zbx_strdup(NULL, "NULL"), ZBX_JSON_TYPE_STRING);
				break;
		}
	}

	zbx_json_close(&json);
	SET_STR_RESULT(result, strdup(json.buffer));
	zbx_json_free(&json);
	ret = SYSINFO_RET_OK;

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s): %s", __func__, request->key, zbx_sysinfo_ret_string(ret));

	return ret;
}

int make_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result, zbx_db_result_type mode)
{
	int				ret = SYSINFO_RET_FAIL;
	struct zbx_json	json;
	unsigned int	col, row;
	char			buffer[MAX_STRING_LEN], date_buf[64];
	char			*value_str, *c = NULL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);

	if (0 == db_result.nb_rows && ZBX_DB_RES_TYPE_DISCOVERY != mode)
	{
		if (ZBX_DB_RES_TYPE_ONEROW == mode)
		{
			SET_STR_RESULT(result, zbx_strdup(NULL, "{}"));
			zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Return {}", __func__, request->key);
			ret = SYSINFO_RET_OK;
		}
		else if (ZBX_DB_RES_TYPE_TWOCOLL == mode)
		{
			SET_STR_RESULT(result, zbx_strdup(NULL, "{}"));
			zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Return {}", __func__, request->key);
			ret = SYSINFO_RET_OK;
		}
		else if (ZBX_DB_RES_TYPE_MULTIROW == mode)
		{
			SET_STR_RESULT(result, zbx_strdup(NULL, "[]"));
			zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Return []", __func__, request->key);
			ret = SYSINFO_RET_OK;
		}
		else
		{
			SET_MSG_RESULT(result, zbx_strdup(NULL, "The request returned a null result."));
			zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): The request returned a null result.", __func__, request->key);
		}
		goto out;
	}

	if (ZBX_DB_RES_TYPE_NOJSON == mode)
	{
		ret = make_nojson_result(request, result, db_result);
		goto out;
	}
	else if (ZBX_DB_RES_TYPE_ONEROW == mode)
	{
		if (1 < db_result.nb_rows)
		{
			SET_MSG_RESULT(result, zbx_strdup(NULL, "The query returned more than one row of the result."));
			zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): The query returned more than one row of the result.", __func__, request->key);
		}
		else
		{
			ret = make_onerow_json_result(request, result, db_result);
		}
		goto out;
	}
	else if (ZBX_DB_RES_TYPE_TWOCOLL == mode)
	{
		if (2 < db_result.nb_columns)
		{
			SET_MSG_RESULT(result, zbx_strdup(NULL, "The query returned more than two columns of the result."));
			zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): The query returned more than two columns of the result.", __func__, request->key);
		}
		else if (2 > db_result.nb_columns)
		{
			SET_MSG_RESULT(result, zbx_strdup(NULL, "The query returned less than two columns of the result."));
			zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): The query returned less than two columns of the result.", __func__, request->key);
		}
		else
		{
			ret = make_multirow_twocoll_json_result(request, result, db_result);
		}
		goto out;
	}
	else if (ZBX_DB_RES_TYPE_MULTIROW == mode || ZBX_DB_RES_TYPE_DISCOVERY == mode)
	{
		zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Rows: %u, Cols: %u", __func__, request->key, db_result.nb_rows, db_result.nb_columns);

		if (0 == db_result.nb_rows)
		{
			SET_STR_RESULT(result, zbx_strdup(NULL, "[]"));
		}
		else
		{
			for (col = 0; col < db_result.nb_columns; col++)
			{
				zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): ColNum: %u, ColName: %s", __func__, request->key, col, (((struct zbx_db_type_text *)db_result.fields[0][col].t_data)->value));
			}

			zbx_json_initarray(&json, ZBX_JSON_STAT_BUF_LEN);

			for (row = 0; row < db_result.nb_rows; row++)
			{
				zbx_json_addobject(&json, NULL);

				for (col = 0; col < db_result.nb_columns; col++)
				{
					zbx_snprintf(buffer, sizeof(buffer), "%s", zbx_strdup(NULL, (((struct zbx_db_type_text *)db_result.fields[0][col].t_data)->value)));

					for (c = &buffer[0]; *c; c++)
					{
						if (0 != isalpha((unsigned char)*c))
							*c = toupper((unsigned char)*c);

						if (ZBX_DB_RES_TYPE_DISCOVERY == mode)
						{
							if (SUCCEED != is_macro_char((unsigned char)*c))
							{
								zabbix_log(LOG_LEVEL_WARNING, "In %s(%s): Cannot convert column number %u, column name %s to macro", __func__, request->key, col, (((struct zbx_db_type_text *)db_result.fields[0][col].t_data)->value));
								SET_STR_RESULT(result, zbx_strdup(NULL, "Cannot convert column to macro."));
								goto out;
							}
						}
					}

					if (ZBX_DB_RES_TYPE_DISCOVERY == mode)
						zbx_snprintf(buffer, sizeof(buffer), "{#%s}", zbx_strdup(NULL, buffer));
					else
						zbx_snprintf(buffer, sizeof(buffer), "%s", zbx_strdup(NULL, buffer));

					switch (db_result.data[row][col].type)
					{
						case ZBX_COL_TYPE_INT:
							zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col: %d, Value(INT): %lld", __func__, request->key, row, col, ((struct zbx_db_type_int *)db_result.data[row][col].t_data)->value);
							value_str = zbx_dsprintf(NULL, "%lld", (((struct zbx_db_type_int *)db_result.data[row][col].t_data)->value));
							zbx_json_addstring(&json, buffer, zbx_strdup(NULL, value_str), ZBX_JSON_TYPE_STRING);
							zbx_free(value_str);
							break;
						case ZBX_COL_TYPE_DOUBLE:
							zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col: %d, Value(DOUBLE): " ZBX_FS_DBL, __func__, request->key, row, col, ((struct zbx_db_type_double *)db_result.data[row][col].t_data)->value);
							value_str = zbx_dsprintf(NULL, ZBX_FS_DBL, (((struct zbx_db_type_double *)db_result.data[row][col].t_data)->value));
							zbx_json_addstring(&json, buffer, zbx_strdup(NULL, value_str), ZBX_JSON_TYPE_STRING);
							zbx_free(value_str);
							break;
						case ZBX_COL_TYPE_TEXT:
							zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col: %d, Value(TEXT): %s", __func__, request->key, row, col, ((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value);
							zbx_json_addstring(&json, buffer, zbx_strdup(NULL, (((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value)), ZBX_JSON_TYPE_STRING);
							break;
						case ZBX_COL_TYPE_DATE:
							strftime(date_buf, sizeof(date_buf), "%Y-%m-%d %H:%M:%S", &((struct zbx_db_type_datetime *)db_result.data[row][col].t_data)->value);
							zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col: %d, Value(DATE): %s", __func__, request->key, row, col, date_buf);
							zbx_json_addstring(&json, buffer, zbx_strdup(NULL, date_buf), ZBX_JSON_TYPE_STRING);
							break;
						case ZBX_COL_TYPE_BLOB:
							zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col: %d, Value(BLOB): [BLOB]", __func__, request->key, row, col);
							zbx_json_addstring(&json, buffer, zbx_strdup(NULL, "[BLOB]"), ZBX_JSON_TYPE_STRING);
							break;
						case ZBX_COL_TYPE_NULL:
							zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col: %d, Value(NULL): NULL", __func__, request->key, row, col);
							zbx_json_addstring(&json, buffer, zbx_strdup(NULL, "NULL"), ZBX_JSON_TYPE_STRING);
							break;
					}
				}

				zbx_json_close(&json);
			}

			zbx_json_close(&json);
			SET_STR_RESULT(result, strdup(json.buffer));
			zbx_json_free(&json);
		}

		ret = SYSINFO_RET_OK;
	}
	else
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "Unknown result mode."));
		zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Unknown result mode.", __func__, request->key);
	}

out:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s): %s", __func__, request->key, zbx_sysinfo_ret_string(ret));

	return ret;
}

char *get_str_one_result(AGENT_REQUEST *request, AGENT_RESULT *result, const unsigned int row,
						const unsigned int col, struct zbx_db_result db_result)
 {
	char *ret;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);
	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Rows: %u, Cols: %u", __func__, request->key, db_result.nb_rows, db_result.nb_columns);

	if (0 == db_result.nb_rows)
	{
		ret = NULL;
	}
	else
	{
		zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): ColNum: %u, ColName: %s", __func__, request->key, col, (((struct zbx_db_type_text *)db_result.fields[0][col].t_data)->value));

		if (row < db_result.nb_rows && col < db_result.nb_columns)
		{
			switch (db_result.data[row][col].type)
			{
				case ZBX_COL_TYPE_TEXT:
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col: %d, Value(TEXT): %s", __func__, request->key, row, col, ((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value);
					ret = zbx_strdup(NULL, ((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value);
					break;
				default:
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col: %d, Value: Not text", __func__, request->key, row, col);
					ret = NULL;
					break;
			}
		}
		else
		{
			ret = NULL;
		}

	}

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s): %s", __func__, request->key, ret);

	return ret;
}

unsigned int get_int_one_result(AGENT_REQUEST *request, AGENT_RESULT *result, const unsigned int row,
	const unsigned int col, struct zbx_db_result db_result)
{
	unsigned int ret = 0;
	char *tmp;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);
	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Rows: %u, Cols: %u", __func__, request->key, db_result.nb_rows, db_result.nb_columns);

	if (0 == db_result.nb_rows)
	{
		ret = 0;
	}
	else
	{
		zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): ColNum: %u, ColName: %s", __func__, request->key, col, (((struct zbx_db_type_text *)db_result.fields[0][col].t_data)->value));

		if (row < db_result.nb_rows && col < db_result.nb_columns)
		{
			switch (db_result.data[row][col].type)
			{
				case ZBX_COL_TYPE_TEXT:
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col: %d, Value(TEXT): %s", __func__, request->key, row, col, ((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value);
					tmp = zbx_strdup(NULL, ((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value);
					ret = atoi(tmp);
					zbx_free(tmp);
					break;
				case ZBX_COL_TYPE_INT:
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col: %d, Value(INT): %lld", __func__, request->key, row, col, ((struct zbx_db_type_int *)db_result.data[row][col].t_data)->value);
					ret = (unsigned int)((struct zbx_db_type_int *)db_result.data[row][col].t_data)->value;
					break;
				default:
					zabbix_log(LOG_LEVEL_TRACE, "In %s(%s): Row: %d, Col: %d, Value(NOT_INT): NULL", __func__, request->key, row, col);
					ret = 0;
					break;
			}
		}
		else
		{
			ret = 0;
		}
	}

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s): %d", __func__, request->key, ret);

	return ret;
}

/*
 * Log an error to the agent log file and set the result message sent back to
 * the server.
 */
int dbmon_log_result(AGENT_RESULT *result, int level, const char *format, ...)
{
	va_list args;
	char    msg[MAX_STRING_LEN];

	va_start(args, format);
	zbx_vsnprintf(msg, sizeof(msg), format, args);

	zabbix_log(level, "In %s: %s", __func__, msg);

	if (NULL != result)
		SET_MSG_RESULT(result, zbx_strdup(NULL, msg));

	va_end(args);

	return SYSINFO_RET_FAIL;
}

