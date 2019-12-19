#include "common.h"
#include "sysinfo.h"
#include "log.h"
#include "module.h"
#include "zbxdbmon.h"
#include <zbxjson.h>

int make_discovery_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result)
{
	int				ret = SYSINFO_RET_FAIL;
	unsigned int	col, row;
	struct			zbx_json j;
	char			buffer[MAX_STRING_LEN];
	char			*c = NULL;
	char			*value_str;
	char			date_buf[64];

	zabbix_log(LOG_LEVEL_DEBUG, "Start in %s(%s)", __func__, request->key);

	zbx_json_init(&j, ZBX_JSON_STAT_BUF_LEN);
	zbx_json_addarray(&j, ZBX_PROTO_TAG_DATA);

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Rows: %u, Cols: %u", __func__, request->key, db_result.nb_rows, db_result.nb_columns);

	for (row = 0; row < db_result.nb_rows; row++)
	{
		zbx_json_addobject(&j, NULL);

		for (col = 0; col < db_result.nb_columns; col++)
		{
			zbx_snprintf(buffer, sizeof(buffer), "{#%s}", zbx_strdup(NULL, (((struct zbx_db_type_text *)db_result.fields[0][col].t_data)->value)));

			for (c = &buffer[0]; *c; c++)
				*c = toupper(*c);

			switch (db_result.data[row][col].type)
			{
				case ZBX_COL_TYPE_INT:
					value_str = zbx_dsprintf(NULL, ZBX_FS_UI64, (((struct zbx_db_type_int *)db_result.data[row][col].t_data)->value));
					zbx_json_addstring(&j, buffer, zbx_strdup(NULL, value_str), ZBX_JSON_TYPE_STRING);
					break;
				case ZBX_COL_TYPE_DOUBLE:
					value_str = zbx_dsprintf(NULL, ZBX_FS_DBL, (((struct zbx_db_type_double *)db_result.data[row][col].t_data)->value));
					zbx_json_addstring(&j, buffer, zbx_strdup(NULL, value_str), ZBX_JSON_TYPE_STRING);
					break;
				case ZBX_COL_TYPE_TEXT:
					zbx_json_addstring(&j, buffer, zbx_strdup(NULL, (((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value)), ZBX_JSON_TYPE_STRING);
					break;
				case ZBX_COL_TYPE_DATE:
					strftime(date_buf, 64, "%Y-%m-%d %H:%M:%S", &((struct zbx_db_type_datetime *)db_result.data[row][col].t_data)->value);
					zbx_json_addstring(&j, buffer, zbx_strdup(NULL, date_buf), ZBX_JSON_TYPE_STRING);
					break;
				case ZBX_COL_TYPE_BLOB:
					zbx_json_addstring(&j, buffer, zbx_strdup(NULL, "[BLOB]"), ZBX_JSON_TYPE_STRING);
					break;
				case ZBX_COL_TYPE_NULL:
				default:
					zbx_json_addstring(&j, buffer, zbx_strdup(NULL, "[NULL]"), ZBX_JSON_TYPE_STRING);
					break;
			}
		}

		zbx_json_close(&j);
	}

	zbx_json_close(&j);
	SET_STR_RESULT(result, strdup(j.buffer));
	zbx_json_free(&j);

	ret = SYSINFO_RET_OK;

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __func__, request->key);

	return ret;
}

int make_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result)
{
	int				ret = SYSINFO_RET_FAIL;
	unsigned int	col, row;
	char			buf[64];

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);
	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Rows: %u, Cols: %u", __func__, request->key, db_result.nb_rows, db_result.nb_columns);

	if (0 == db_result.nb_rows)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "The request returned a null result."));
		ret = SYSINFO_RET_FAIL;
	}
	else
	{
		for (col = 0; col < db_result.nb_columns; col++)
		{
			zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): ColNum: %u, ColName: %s", __func__, request->key, col, (((struct zbx_db_type_text *)db_result.fields[0][col].t_data)->value));
		}

		for (row = 0; row < db_result.nb_rows; row++)
		{
			for (col = 0; col < db_result.nb_columns; col++)
			{
				switch (db_result.data[row][col].type)
				{
				case ZBX_COL_TYPE_INT:
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Row: %d, Col(INT): %d, Value: %I64u", __func__, request->key, row, col, ((struct zbx_db_type_int *)db_result.data[row][col].t_data)->value);
					SET_UI64_RESULT(result, ((struct zbx_db_type_int *)db_result.data[row][col].t_data)->value);
					ret = SYSINFO_RET_OK;
					break;
				case ZBX_COL_TYPE_DOUBLE:
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Row: %d, Col(DOUBLE): %d, Value: %f", __func__, request->key, row, col, ((struct zbx_db_type_double *)db_result.data[row][col].t_data)->value);
					SET_DBL_RESULT(result, ((struct zbx_db_type_double *)db_result.data[row][col].t_data)->value);
					ret = SYSINFO_RET_OK;
					break;
				case ZBX_COL_TYPE_TEXT:
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Row: %d, Col(TEXT): %d, Value: %s", __func__, request->key, row, col, ((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value);
					SET_TEXT_RESULT(result, zbx_strdup(NULL, ((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value));
					ret = SYSINFO_RET_OK;
					break;
				case ZBX_COL_TYPE_DATE:
					strftime(buf, 64, "%Y-%m-%d %H:%M:%S", &((struct zbx_db_type_datetime *)db_result.data[row][col].t_data)->value);
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Row: %d, Col(DATE): %d, Value: %s", __func__, request->key, row, col, buf);
					SET_STR_RESULT(result, buf);
					ret = SYSINFO_RET_OK;
					break;
				case ZBX_COL_TYPE_BLOB:
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Row: %d, Col(BLOB): %d, Value: [BLOB]", __func__, request->key, row, col);
					SET_STR_RESULT(result, zbx_strdup(NULL, "[BLOB]"));
					ret = SYSINFO_RET_OK;
					break;
				case ZBX_COL_TYPE_NULL:
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Row: %d, Col(NULL): %d, Value: [NULL]", __func__, request->key, row, col);
					SET_STR_RESULT(result, zbx_strdup(NULL, "[NULL]"));
					ret = SYSINFO_RET_OK;
					break;
				}
			}
		}
	}

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __func__, request->key);

	return ret;
}

int make_onerow_json_result(AGENT_REQUEST *request, AGENT_RESULT *result, struct zbx_db_result db_result)
{
	int				ret = SYSINFO_RET_FAIL;
	unsigned int	col, row;
	char			date_buf[64];
	struct			zbx_json json;
	char			buffer[MAX_STRING_LEN];
	char			*c = NULL;
	char			*value_str;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __func__, request->key);
	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Rows: %u, Cols: %u", __func__, request->key, db_result.nb_rows, db_result.nb_columns);

	if (0 == db_result.nb_rows)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "The request returned a null result."));
		ret = SYSINFO_RET_FAIL;
	}
	else if (1 < db_result.nb_rows)
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, "The query returned more than one row of the result."));
		ret = SYSINFO_RET_FAIL;
	}
	else
	{
		for (col = 0; col < db_result.nb_columns; col++)
		{
			zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): ColNum: %u, ColName: %s", __func__, request->key, col, (((struct zbx_db_type_text *)db_result.fields[0][col].t_data)->value));
		}

		zbx_json_init(&json, ZBX_JSON_STAT_BUF_LEN);

		for (row = 0; row < db_result.nb_rows; row++)
		{
			//zbx_json_addobject(&json, NULL);

			for (col = 0; col < db_result.nb_columns; col++)
			{
				zbx_snprintf(buffer, sizeof(buffer), "{#%s}", zbx_strdup(NULL, (((struct zbx_db_type_text *)db_result.fields[0][col].t_data)->value)));

				for (c = &buffer[0]; *c; c++)
					*c = toupper(*c);

				switch (db_result.data[row][col].type)
				{
				case ZBX_COL_TYPE_INT:
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Row: %d, Col(INT): %d, Value: %I64u", __func__, request->key, row, col, ((struct zbx_db_type_int *)db_result.data[row][col].t_data)->value);
					value_str = zbx_dsprintf(NULL, ZBX_FS_UI64, (((struct zbx_db_type_int *)db_result.data[row][col].t_data)->value));
					zbx_json_addstring(&json, buffer, zbx_strdup(NULL, value_str), ZBX_JSON_TYPE_STRING);
					break;
				case ZBX_COL_TYPE_DOUBLE:
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Row: %d, Col(DOUBLE): %d, Value: %f", __func__, request->key, row, col, ((struct zbx_db_type_double *)db_result.data[row][col].t_data)->value);
					value_str = zbx_dsprintf(NULL, ZBX_FS_DBL, (((struct zbx_db_type_double *)db_result.data[row][col].t_data)->value));
					zbx_json_addstring(&json, buffer, zbx_strdup(NULL, value_str), ZBX_JSON_TYPE_STRING);
					break;
				case ZBX_COL_TYPE_TEXT:
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Row: %d, Col(TEXT): %d, Value: %s", __func__, request->key, row, col, ((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value);
					zbx_json_addstring(&json, buffer, zbx_strdup(NULL, (((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value)), ZBX_JSON_TYPE_STRING);
					break;
				case ZBX_COL_TYPE_DATE:
					strftime(date_buf, 64, "%Y-%m-%d %H:%M:%S", &((struct zbx_db_type_datetime *)db_result.data[row][col].t_data)->value);
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Row: %d, Col(DATE): %d, Value: %s", __func__, request->key, row, col, date_buf);
					zbx_json_addstring(&json, buffer, zbx_strdup(NULL, date_buf), ZBX_JSON_TYPE_STRING);
					break;
				case ZBX_COL_TYPE_BLOB:
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Row: %d, Col(BLOB): %d, Value: [BLOB]", __func__, request->key, row, col);
					zbx_json_addstring(&json, buffer, zbx_strdup(NULL, "[BLOB]"), ZBX_JSON_TYPE_STRING);
					break;
				case ZBX_COL_TYPE_NULL:
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Row: %d, Col(NULL): %d, Value: [NULL]", __func__, request->key, row, col);
					zbx_json_addstring(&json, buffer, zbx_strdup(NULL, "[NULL]"), ZBX_JSON_TYPE_STRING);
					break;
				}
			}

			//zbx_json_close(&json);
		}

		zbx_json_close(&json);
		SET_STR_RESULT(result, strdup(json.buffer));
		zbx_json_free(&json);

		ret = SYSINFO_RET_OK;

	}

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __func__, request->key);

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
		zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): ColNum: %u, ColName: %s", __func__, request->key, col, (((struct zbx_db_type_text *)db_result.fields[0][col].t_data)->value));

		if (row < db_result.nb_rows && col < db_result.nb_columns)
		{
			switch (db_result.data[row][col].type)
			{
				case ZBX_COL_TYPE_TEXT:
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Row: %d, Col(TEXT): %d, Value: %s", __func__, request->key, row, col, ((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value);
					ret = zbx_strdup(NULL, ((struct zbx_db_type_text *)db_result.data[row][col].t_data)->value);
					break;
				default:
					zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s): Row: %d, Col: %d, Value: Not text", __func__, request->key, row, col);
					ret = NULL;
					break;
			}
		}
		else
		{
			ret = NULL;
		}

	}

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __func__, request->key);

	return ret;
}

/*
 *  Utility function to compare each substring of version1 and version2 
 */
int zbx_db_compare_substr(char *substr_version1, char *substr_version2,	int len_substr_version1, int len_substr_version2)
{
	int i = 0, j = 0;

	// if length of substring of version 1 is greater then 
	// it means value of substr of version1 is also greater 
	if (len_substr_version1 > len_substr_version2)
		return 1;
	else if (len_substr_version1 < len_substr_version2)
		return -1;
	// when length of the substrings of both versions is same. 
	else
	{
		// compare each character of both substrings and return 
		// accordingly. 
		while (i < len_substr_version1)
		{
			if (substr_version1[i] < substr_version2[j])
				return -1;
			else if (substr_version1[i] > substr_version2[j])
				return 1;
			i++, j++;
		}
		return 0;
	}
}

/*
 *  Function to compare two versions
 *  Return 1  if version1 > version2
 *  Return -1 if version1 < version2
 *  Return 0  if version1 = version2
 */
int zbx_db_compare_version(const char *version1, const char *version2)
{
	int		res = 0, i = 0, j = 0;
	size_t	len_version1, len_version2;
	char	*substr_version1 = NULL, *substr_version2 = NULL;

	len_version1 = zbx_db_strlen(version1);
	len_version2 = zbx_db_strlen(version2);
	substr_version1 = (char *)zbx_db_malloc(sizeof(char) * len_version1);
	substr_version2 = (char *)zbx_db_malloc(sizeof(char) * len_version2);

	// loop until both strings are exhausted. 
	// and extract the substrings from version1 and version2 
	while (i < len_version1 || j < len_version2)
	{
		int p = 0, q = 0;

		// skip the leading zeros in version1 string. 
		while (version1[i] == '0')
			i++;

		// skip the leading zeros in version2 string. 
		while (version2[j] == '0')
			j++;

		// extract the substring from version1. 
		while (version1[i] != '.' && i < len_version1)
			substr_version1[p++] = version1[i++];

		//extract the substring from version2. 
		while (version2[j] != '.' && j < len_version2)
			substr_version2[q++] = version2[j++];

		res = zbx_db_compare_substr(substr_version1, substr_version2, p, q);

		// if res is either -1 or +1 then simply return. 
		if (res)
			break;
		i++;
		j++;
	}

	zbx_db_free(substr_version1);
	zbx_db_free(substr_version2);

	return res;
}