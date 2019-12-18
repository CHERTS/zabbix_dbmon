#include "common.h"
#include "threads.h"
#include "log.h"
#include "zbxdbmon.h"

#ifdef _WINDOWS
extern int bsd_strptime(const char *s, const char *format, struct tm *tm);
#else
#include <time.h>
#endif

#if defined(HAVE_MYSQL)
#	include "mysql.h"
#	include "errmsg.h"
#	include "mysqld_error.h"
#endif

#if defined(HAVE_POSTGRESQL)
#	include <libpq-fe.h>
#endif

#if defined(HAVE_ORACLE)
#	include "oci.h"
#	include "dbschema.h"
#endif

#if defined(HAVE_MSSQL)
#include <sqlfront.h>
#include <sqldb.h>
#endif

static char	*last_db_strerror = NULL;
static char	*last_mysql_db_strerror = NULL;
static char	*last_pgsql_db_strerror = NULL;
static char	*last_oracle_db_strerror = NULL;
static char	*last_mssql_db_strerror = NULL;

void *zbx_db_malloc(size_t size)
{
	if (!size)
	{
		return NULL;
	}
	else
	{
		return (*do_malloc)(size);
	}
}

void *zbx_db_realloc(void *ptr, size_t size)
{
	if (!size)
	{
		return NULL;
	}
	else
	{
		return (*do_realloc)(ptr, size);
	}
}

void zbx_db_free(void *ptr)
{
	if (ptr == NULL)
	{
		return;
	}
	else
	{
		(*do_free)(ptr);
	}
}

/**
 * zbx_db_strcmp
 * A modified strcmp function that don't crash when p1 is NULL or p2 us NULL
 */
int zbx_db_strcmp(const char *p1, const char *p2)
{
	if (p1 == NULL && p2 == NULL)
	{
		return 0;
	}
	else if (p1 != NULL && p2 == NULL)
	{
		return -1;
	}
	else if (p1 == NULL)
	{
		return 1;
	}
	else
	{
		return strcmp(p1, p2);
	}
}

/**
 * zbx_db_strncmp
 * A modified strncmp function that don't crash when p1 is NULL or p2 us NULL
 */
int zbx_db_strncmp(const char * p1, const char * p2, size_t n)
{
	if ((p1 == NULL && p2 == NULL) || n <= 0)
	{
		return 0;
	}
	else if (p1 != NULL && p2 == NULL)
	{
		return -1;
	}
	else if (p1 == NULL)
	{
		return 1;
	}
	else
	{
		return strncmp(p1, p2, n);
	}
}

/**
 * zbx_db_strlen
 * A modified version of strlen that don't crash when s is NULL
 */
size_t zbx_db_strlen(const char *s)
{
	if (s == NULL)
	{
		return 0;
	}
	else
	{
		return strlen(s);
	}
}

/**
 * zbx_db_strcasecmp
 * A modified strcasecmp function that don't crash when p1 is NULL or p2 us NULL
 */
int zbx_db_strcasecmp(const char *p1, const char *p2)
{
	if (p1 == NULL && p2 == NULL)
	{
		return 0;
	}
	else if (p1 != NULL && p2 == NULL)
	{
		return -1;
	}
	else if (p1 == NULL && p2 != NULL)
	{
		return 1;
	}
	else
	{
		return strcasecmp(p1, p2);
	}
}

void zbx_db_err_log(int db_type, zbx_err_codes_t zbx_errno, int db_errno, const char *db_error, const char *context)
{
	char	*s;

#if defined(HAVE_MYSQL)
	if (ZBX_DB_TYPE_MYSQL == db_type)
	{
		if (NULL != db_error)
			last_mysql_db_strerror = zbx_strdup(last_mysql_db_strerror, db_error);
		else
			last_mysql_db_strerror = zbx_strdup(last_mysql_db_strerror, "");

		last_db_strerror = last_mysql_db_strerror;
	}
#endif
#if defined(HAVE_POSTGRESQL)
	if (ZBX_DB_TYPE_POSTGRESQL == db_type)
	{
		if (NULL != db_error)
			last_pgsql_db_strerror = zbx_strdup(last_pgsql_db_strerror, db_error);
		else
			last_pgsql_db_strerror = zbx_strdup(last_pgsql_db_strerror, "");

		last_db_strerror = last_pgsql_db_strerror;
	}
#endif
#if defined(HAVE_ORACLE)
	if (ZBX_DB_TYPE_ORACLE == db_type)
	{
		if (NULL != db_error)
			last_oracle_db_strerror = zbx_strdup(last_oracle_db_strerror, db_error);
		else
			last_oracle_db_strerror = zbx_strdup(last_oracle_db_strerror, "");

		last_db_strerror = last_oracle_db_strerror;
	}
#endif
#if defined(HAVE_MSSQL)
	if (ZBX_DB_TYPE_MSSQL == db_type)
	{
		if (NULL != db_error)
			last_mssql_db_strerror = zbx_strdup(last_mssql_db_strerror, db_error);
		else
			last_mssql_db_strerror = zbx_strdup(last_mssql_db_strerror, "");

		last_db_strerror = last_mssql_db_strerror;
	}
#endif

	switch (zbx_errno)
	{
	case ERR_Z3001:
		s = zbx_dsprintf(NULL, "connection to database '%s' failed: [%d] %s", context, db_errno,
			last_db_strerror);
		break;
	case ERR_Z3002:
		s = zbx_dsprintf(NULL, "cannot create database '%s': [%d] %s", context, db_errno,
			last_db_strerror);
		break;
	case ERR_Z3003:
		s = zbx_strdup(NULL, "no connection to the database");
		break;
	case ERR_Z3004:
		s = zbx_dsprintf(NULL, "cannot close database: [%d] %s", db_errno, last_db_strerror);
		break;
	case ERR_Z3005:
		s = zbx_dsprintf(NULL, "query failed: [%d] %s [%s]", db_errno, last_db_strerror, context);
		break;
	case ERR_Z3006:
		s = zbx_dsprintf(NULL, "fetch failed: [%d] %s", db_errno, last_db_strerror);
		break;
	case ERR_Z3007:
		s = zbx_dsprintf(NULL, "query failed: [%d] %s", db_errno, last_db_strerror);
		break;
	default:
		s = zbx_strdup(NULL, "unknown error");
	}

	zabbix_log(LOG_LEVEL_ERR, "[%s][Z%04d] %s", DB_TYPE[db_type-1], (int)zbx_errno, s);

	zbx_free(s);
}

/**
 * zbx_db_clean_data
 * Free memory allocated by the struct zbx_db_data
 * Return ZBX_DB_OK on success
 */
int zbx_db_clean_data(struct zbx_db_data * e_data)
{
	if (NULL != e_data)
	{
		if (ZBX_COL_TYPE_TEXT == e_data->type)
		{
			zbx_db_free(((struct zbx_db_type_text *)e_data->t_data)->value);
		}
		else if (ZBX_COL_TYPE_BLOB == e_data->type)
		{
			zbx_db_free(((struct zbx_db_type_blob *)e_data->t_data)->value);
		}
		if (NULL != e_data->t_data)
		{
			zbx_db_free(e_data->t_data);
		}

		return ZBX_DB_OK;
	}
	else
	{
		return ZBX_DB_ERROR_PARAMS;
	}
}

/**
 * zbx_db_clean_data_full
 * Free memory allocated by the struct zbx_db_data and the struct zbx_db_data pointer
 * Return ZBX_DB_OK on success
 */
int zbx_db_clean_data_full(struct zbx_db_data * e_data)
{
	if (NULL != e_data)
	{
		zbx_db_clean_data(e_data);
		zbx_db_free(e_data);

		return ZBX_DB_OK;
	}
	else
	{
		return ZBX_DB_ERROR_PARAMS;
	}
}

/**
 * zbx_db_clean_fields
 * Free memory allocated by the struct zbx_db_fields
 * Return ZBX_DB_OK on success
 */
int zbx_db_clean_fields(struct zbx_db_fields * e_data)
{
	if (NULL != e_data)
	{
		zbx_db_free(((struct zbx_db_type_text *)e_data->t_data)->value);

		return ZBX_DB_OK;
	}
	else
	{
		return ZBX_DB_ERROR_PARAMS;
	}
}

/**
 * zbx_db_clean_fields_full
 * Free memory allocated by the struct zbx_db_fields and the struct zbx_db_fields pointer
 * Return ZBX_DB_OK on success
 */
int zbx_db_clean_fields_full(struct zbx_db_fields * fields)
{
	if (NULL != fields)
	{
		zbx_db_clean_fields(fields);
		zbx_db_free(fields);

		return ZBX_DB_OK;
	}
	else
	{
		return ZBX_DB_ERROR_PARAMS;
	}
}

/**
 * zbx_db_new_data_int
 * Allocate memory for a new struct zbx_db_data * containing an int
 * Return pointer to the new structure
 * Return NULL on error
 */
struct zbx_db_data * zbx_db_new_data_int(const long long int e_value)
{
	struct zbx_db_data * data = (struct zbx_db_data *)zbx_malloc(NULL, sizeof(struct zbx_db_data));

	if (NULL != data)
	{
		data->t_data = (void *)zbx_db_malloc(sizeof(struct zbx_db_type_int));

		if (NULL == data->t_data)
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for data->t_data", __func__);
			zbx_db_free(data);

			return NULL;
		}

		data->type = ZBX_COL_TYPE_INT;
		((struct zbx_db_type_int *)data->t_data)->value = e_value;
	}
	else
	{
		zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for data", __func__);
	}

	return data;
}

/**
 * zbx_db_new_data_null
 * Allocate memory for a new struct zbx_db_data * containing a null value
 * Return pointer to the new structure
 * Return NULL on error
 */
struct zbx_db_data * zbx_db_new_data_null()
{
	struct zbx_db_data * data = (struct zbx_db_data *)zbx_db_malloc(sizeof(struct zbx_db_data));

	if (NULL != data)
	{
		data->type = ZBX_COL_TYPE_NULL;
		data->t_data = NULL;
	}
	else
	{
		zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for data", __func__);
	}

	return data;
}

/**
 * zbx_db_new_data_double
 * Allocate memory for a new struct zbx_db_data * containing a double
 * Return pointer to the new structure
 * Return NULL on error
 */
struct zbx_db_data * zbx_db_new_data_double(const double e_value)
{
	struct zbx_db_data * data = (struct zbx_db_data *)zbx_db_malloc(sizeof(struct zbx_db_data));

	if (NULL != data)
	{
		data->t_data = (void *)zbx_db_malloc(sizeof(struct zbx_db_type_double));

		if (NULL == data->t_data)
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for data->t_data", __func__);
			zbx_db_free(data);
			return NULL;
		}

		data->type = ZBX_COL_TYPE_DOUBLE;
		((struct zbx_db_type_double *)data->t_data)->value = e_value;
	}
	else
	{
		zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for data", __func__);
	}

	return data;
}

/**
 * zbx_db_new_data_datetime
 * Allocate memory for a new struct zbx_db_data * containing a date time structure
 * Return pointer to the new structure
 * Return NULL on error
 */
struct zbx_db_data * zbx_db_new_data_datetime(const struct tm * datetime)
{
	struct zbx_db_data * data = NULL;

	if (NULL != datetime)
	{
		data = (struct zbx_db_data *)zbx_db_malloc(sizeof(struct zbx_db_data));

		if (NULL != data)
		{
			data->type = ZBX_COL_TYPE_DATE;
			data->t_data = (void *)zbx_db_malloc(sizeof(struct zbx_db_type_datetime));

			if (NULL == data->t_data)
			{
				zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for data->t_data", __func__);
				zbx_db_free(data);
				return NULL;
			}

			((struct zbx_db_type_datetime *)data->t_data)->value = *datetime;
		}
		else
		{
			zabbix_log(LOG_LEVEL_ERR, "In %s(): Error allocating memory for data", __func__);
		}
	}

	return data;
}

/**
 * zbx_db_new_data_blob
 * Allocate memory for a new struct zbx_db_data * containing a blob
 * Return pointer to the new structure
 * Return NULL on error
 */
struct zbx_db_data * zbx_db_new_data_blob(const void * e_value, const size_t e_length)
{
	struct zbx_db_data * data = (struct zbx_db_data *)zbx_db_malloc(sizeof(struct zbx_db_data));

	if (NULL != data)
	{
		data->t_data = (void *)zbx_db_malloc(sizeof(struct zbx_db_type_blob));

		if (NULL == data->t_data)
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for data->t_data", __func__);
			zbx_db_free(data);
			return NULL;
		}

		data->type = ZBX_COL_TYPE_BLOB;
		((struct zbx_db_type_blob *)data->t_data)->length = e_length;
		((struct zbx_db_type_blob *)data->t_data)->value = zbx_malloc(NULL, e_length);

		if (NULL == ((struct zbx_db_type_blob *)data->t_data)->value)
		{
			zabbix_log(LOG_LEVEL_CRIT, "End of %s(): Error allocating memory for data->value", __func__);
			zbx_db_free(data);
			return NULL;
		}
		else
		{
			memcpy(((struct zbx_db_type_blob *)data->t_data)->value, e_value, e_length);
		}
	}
	else
	{
		zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for data", __func__);
	}

	return data;
}

/**
 * zbx_db_new_data_text
 * Allocate memory for a new struct zbx_db_data * containing a text
 * Return pointer to the new structure
 * Return NULL on error
 */
struct zbx_db_data * zbx_db_new_data_text(const char * e_value, const size_t e_length)
{
	struct zbx_db_data * data = (struct zbx_db_data *)zbx_db_malloc(sizeof(struct zbx_db_data));

	if (NULL != data)
	{
		data->t_data = (void *)zbx_db_malloc(sizeof(struct zbx_db_type_text));

		if (NULL == data->t_data)
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for data->t_data", __func__);
			zbx_db_free(data);
			return NULL;
		}

		data->type = ZBX_COL_TYPE_TEXT;
		((struct zbx_db_type_text *)data->t_data)->value = (char *)zbx_db_malloc(e_length + sizeof(char));

		if (NULL == ((struct zbx_db_type_text *)data->t_data)->value)
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for data->value", __func__);
			zbx_db_free(data);
			return NULL;
		}
		else
		{
			memcpy(((struct zbx_db_type_text *)data->t_data)->value, e_value, e_length);
			((struct zbx_db_type_text *)data->t_data)->length = e_length;
			((struct zbx_db_type_text *)data->t_data)->value[e_length] = '\0';
		}
	}
	else
	{
		zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for data", __func__);
	}

	return data;
}

/**
 * zbx_db_fields_value
 * Allocate memory for a new struct zbx_db_fields * containing a text
 * Return pointer to the new structure
 * Return NULL on error
 */
struct zbx_db_fields * zbx_db_fields_value(const char * e_value, const size_t e_length)
{
	struct zbx_db_fields * data = (struct zbx_db_fields *)zbx_db_malloc(sizeof(struct zbx_db_fields));

	if (NULL != data)
	{
		data->t_data = (void *)zbx_db_malloc(sizeof(struct zbx_db_type_text));

		if (NULL == data->t_data)
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for data->name", __func__);
			zbx_db_free(data);
			return NULL;
		}

		((struct zbx_db_type_text *)data->t_data)->value = (char *)zbx_db_malloc(e_length + sizeof(char));

		if (NULL == ((struct zbx_db_type_text *)data->t_data)->value)
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for data->value", __func__);
			zbx_db_free(data);
			return NULL;
		}
		else
		{
			memcpy(((struct zbx_db_type_text *)data->t_data)->value, e_value, e_length);
			((struct zbx_db_type_text *)data->t_data)->length = e_length;
			((struct zbx_db_type_text *)data->t_data)->value[e_length] = '\0';
		}
	}
	else
	{
		zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for data", __func__);
	}

	return data;
}

/**
 * zbx_db_row_add_fields
 * Add a new struct zbx_db_fields * to an array of struct zbx_db_fields *, which already has cols columns
 * Return ZBX_DB_OK on success
 */
int zbx_db_row_add_fields(struct zbx_db_fields ** row, struct zbx_db_fields * e_data, int cols)
{
	struct zbx_db_fields * tmp = zbx_db_realloc(*row, (cols + 1) * sizeof(struct zbx_db_fields));

	*row = tmp;

	if (NULL == tmp)
	{
		zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error reallocating memory", __func__);
		return ZBX_DB_ERROR_MEMORY;
	}
	else
	{
		tmp[cols].t_data = zbx_db_malloc(sizeof(struct zbx_db_type_text));
		if (NULL == tmp[cols].t_data)
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for tmp[cols].t_data", __func__);
			return ZBX_DB_ERROR_MEMORY;
		}
		else
		{
			((struct zbx_db_type_text *)tmp[cols].t_data)->value = (char *)zbx_db_malloc(((struct zbx_db_type_text *)e_data->t_data)->length + sizeof(char));

			if (NULL == ((struct zbx_db_type_text *)tmp[cols].t_data)->value)
			{
				zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for ((struct zbx_db_type_text *)tmp[cols].t_data)->value", __func__);
				zbx_db_free(tmp[cols].t_data);
				return ZBX_DB_ERROR_MEMORY;
			}
			memcpy(((struct zbx_db_type_text *)tmp[cols].t_data)->value, ((struct zbx_db_type_text *)e_data->t_data)->value, (((struct zbx_db_type_text *)e_data->t_data)->length + 1));
			((struct zbx_db_type_text *)tmp[cols].t_data)->length = ((struct zbx_db_type_text *)e_data->t_data)->length;
			return ZBX_DB_OK;
		}
		return ZBX_DB_OK;
	}
}

/**
 * zbx_db_row_add_data
 * Add a new struct zbx_db_data * to an array of struct zbx_db_data *, which already has cols columns
 * Return ZBX_DB_OK on success
 */
int zbx_db_row_add_data(struct zbx_db_data ** row, struct zbx_db_data * e_data, int cols)
{
	struct zbx_db_data * tmp = zbx_db_realloc(*row, (cols + 1) * sizeof(struct zbx_db_data));

	*row = tmp;

	if (NULL == tmp)
	{
		zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error reallocating memory", __func__);
		return ZBX_DB_ERROR_MEMORY;
	}
	else
	{
		switch (e_data->type)
		{
		case ZBX_COL_TYPE_INT:
			tmp[cols].type = ZBX_COL_TYPE_INT;
			tmp[cols].t_data = zbx_db_malloc(sizeof(struct zbx_db_type_int));
			if (NULL == tmp[cols].t_data)
			{
				zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for tmp[cols].t_data", __func__);
				return ZBX_DB_ERROR_MEMORY;
			}
			else
			{
				((struct zbx_db_type_int *)tmp[cols].t_data)->value = ((struct zbx_db_type_int *)e_data->t_data)->value;
				return ZBX_DB_OK;
			}
			break;
		case ZBX_COL_TYPE_DOUBLE:
			tmp[cols].type = ZBX_COL_TYPE_DOUBLE;
			tmp[cols].t_data = zbx_db_malloc(sizeof(struct zbx_db_type_double));
			if (NULL == tmp[cols].t_data)
			{
				zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for tmp[cols].t_data", __func__);
				return ZBX_DB_ERROR_MEMORY;
			}
			else
			{
				((struct zbx_db_type_double *)tmp[cols].t_data)->value = ((struct zbx_db_type_double *)e_data->t_data)->value;
				return ZBX_DB_OK;
			}
			break;
		case ZBX_COL_TYPE_TEXT:
			tmp[cols].type = ZBX_COL_TYPE_TEXT;
			tmp[cols].t_data = zbx_db_malloc(sizeof(struct zbx_db_type_text));
			if (tmp[cols].t_data == NULL)
			{
				zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for tmp[cols].t_data", __func__);
				return ZBX_DB_ERROR_MEMORY;
			}
			else
			{
				((struct zbx_db_type_text *)tmp[cols].t_data)->value = zbx_db_malloc(((struct zbx_db_type_text *)e_data->t_data)->length + sizeof(char));

				if (NULL == ((struct zbx_db_type_text *)tmp[cols].t_data)->value)
				{
					zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for ((struct zbx_db_type_text *)tmp[cols].t_data)->value", __func__);
					zbx_db_free(tmp[cols].t_data);
					return ZBX_DB_ERROR_MEMORY;
				}

				memcpy(((struct zbx_db_type_text *)tmp[cols].t_data)->value, ((struct zbx_db_type_text *)e_data->t_data)->value, (((struct zbx_db_type_text *)e_data->t_data)->length + 1));
				((struct zbx_db_type_text *)tmp[cols].t_data)->length = ((struct zbx_db_type_text *)e_data->t_data)->length;

				return ZBX_DB_OK;
			}
			break;
		case ZBX_COL_TYPE_BLOB:
			tmp[cols].type = ZBX_COL_TYPE_BLOB;
			tmp[cols].t_data = zbx_db_malloc(sizeof(struct zbx_db_type_blob));
			if (NULL == tmp[cols].t_data)
			{
				zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for tmp[cols].t_data", __func__);
				return ZBX_DB_ERROR_MEMORY;
			}
			else
			{
				((struct zbx_db_type_blob *)tmp[cols].t_data)->length = ((struct zbx_db_type_blob *)e_data->t_data)->length;
				((struct zbx_db_type_blob *)tmp[cols].t_data)->value = zbx_db_malloc(((struct zbx_db_type_blob *)e_data->t_data)->length);

				if (NULL == ((struct zbx_db_type_blob *)tmp[cols].t_data)->value)
				{
					zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for ((struct zbx_db_type_text *)tmp[cols].t_data)->value", __func__);
					zbx_db_free(tmp[cols].t_data);
					return ZBX_DB_ERROR_MEMORY;
				}

				memcpy(((struct zbx_db_type_blob *)tmp[cols].t_data)->value, ((struct zbx_db_type_blob *)e_data->t_data)->value, ((struct zbx_db_type_blob *)e_data->t_data)->length);

				return ZBX_DB_OK;
			}
			break;
		case ZBX_COL_TYPE_DATE:
			tmp[cols].type = ZBX_COL_TYPE_DATE;
			tmp[cols].t_data = zbx_db_malloc(sizeof(struct zbx_db_type_datetime));
			if (NULL == tmp[cols].t_data)
			{
				zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for tmp[cols].t_data", __func__);
				return ZBX_DB_ERROR_MEMORY;
			}
			else
			{
				((struct zbx_db_type_datetime *)tmp[cols].t_data)->value = ((struct zbx_db_type_datetime *)e_data->t_data)->value;
				return ZBX_DB_OK;
			}
			break;
		case ZBX_COL_TYPE_NULL:
			tmp[cols].type = ZBX_COL_TYPE_NULL;
			tmp[cols].t_data = NULL;
			break;
		default:
			return ZBX_DB_ERROR_PARAMS;
			break;
		}
		return ZBX_DB_OK;
	}
}

/**
 * zbx_db_result_add_row
 * Add a new row of struct zbx_db_data * in a struct zbx_db_result *
 * Return ZBX_DB_OK on success
 */
int zbx_db_result_add_row(struct zbx_db_result * result, struct zbx_db_data * row, int rows)
{
	result->data = zbx_db_realloc(result->data, (rows + 1) * sizeof(struct zbx_db_data *));

	if (NULL == result->data)
	{
		zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error reallocating memory for result->data", __func__);
		return ZBX_DB_ERROR_MEMORY;
	}
	else 
	{
		result->data[rows] = row;
		result->nb_rows++;
		return ZBX_DB_OK;
	}
}

/**
 * zbx_db_result_add_fields
 * Add a new field of struct zbx_db_fields * in a struct zbx_db_fields *
 * Return ZBX_DB_OK on success
 */
int zbx_db_result_add_fields(struct zbx_db_result * result, struct zbx_db_fields * field)
{
	result->fields = zbx_db_realloc(result->fields, sizeof(struct zbx_db_fields *));

	if (NULL == result->fields)
	{
		zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error reallocating memory for result->fields", __func__);
		return ZBX_DB_ERROR_MEMORY;
	}
	else
	{
		result->fields[0] = field;
		return ZBX_DB_OK;
	}
}

/**
 * zbx_db_execute_query
 * Execute a query, set the result structure with the returned values if available
 * if result is NULL, the query is executed but no value will be returned
 * Return ZBX_DB_OK on success
 */
int zbx_db_query_select(const struct zbx_db_connection *conn, struct zbx_db_result *e_result, const char *fmt, ...)
{
	int		rc = ZBX_DB_ERROR_PARAMS;
	va_list	args;
	char	*sql = NULL;

	va_start(args, fmt);
	sql = zbx_dvsprintf(sql, fmt, args);

	zabbix_log(LOG_LEVEL_TRACE, "In %s: SQL: %s", __func__, sql);

	if (NULL != conn && NULL != conn->connection) {
		if (0) {
			/* Not happening */
#if defined(HAVE_MYSQL)
		}
		else if (conn->type == ZBX_DB_TYPE_MYSQL)
		{
			rc = zbx_db_execute_query_mysql(conn, e_result, (const char *)sql);
#endif
#if defined(HAVE_POSTGRESQL)
		}
		else if (conn->type == ZBX_DB_TYPE_POSTGRESQL)
		{
			rc = zbx_db_execute_query_pgsql(conn, e_result, (const char *)sql);
#endif
#if defined(HAVE_ORACLE)
		}
		else if (conn->type == ZBX_DB_TYPE_ORACLE)
		{
			rc = zbx_db_execute_query_oracle(conn, e_result, fmt, args);
#endif
#if defined(HAVE_MSSQL)
		}
		else if (conn->type == ZBX_DB_TYPE_MSSQL)
		{
			rc = zbx_db_execute_query_mssql(conn, result, fmt, args);
#endif
		}
		else
		{
			rc = ZBX_DB_ERROR_PARAMS;
		}
	}
	else
	{
		rc = ZBX_DB_ERROR_PARAMS;
	}

	va_end(args);

	return rc;
}

/**
 * Close a database connection
 * Return ZBX_DB_OK on success
 */
int zbx_db_close_db(struct zbx_db_connection * conn)
{
	if (NULL != conn && NULL != conn->connection)
	{
		if (0)
		{
			/* Not happening */
#if defined(HAVE_MYSQL)
		}
		else if (conn->type == ZBX_DB_TYPE_MYSQL)
		{
			zbx_db_close_mysql(conn);
			return ZBX_DB_OK;
#endif
#if defined(HAVE_POSTGRESQL)
		}
		else if (conn->type == ZBX_DB_TYPE_POSTGRESQL)
		{
			zbx_db_close_pgsql(conn);
			return ZBX_DB_OK;
#endif
#if defined(HAVE_ORACLE)
		}
		else if (conn->type == ZBX_DB_TYPE_ORACLE)
		{
			zbx_db_close_oracle(conn);
			return ZBX_DB_OK;
#endif
#if defined(HAVE_MSSQL)
		}
		else if (conn->type == ZBX_DB_TYPE_MSSQL)
		{
			zbx_db_close_mssql(conn);
			return ZBX_DB_OK;
#endif
		}
		else
		{
			return ZBX_DB_ERROR_PARAMS;
		}
	}
	else
	{
		return ZBX_DB_ERROR_PARAMS;
	}
}

/**
 * zbx_db_clean_connection
 * Free memory allocated by the struct zbx_db_connection
 * Return ZBX_DB_OK on success
 */
int zbx_db_clean_connection(struct zbx_db_connection * conn)
{
	if (NULL !=  conn)
	{
		zbx_db_free(conn->connection);
		zbx_db_free(conn);
		return ZBX_DB_OK;
	}
	else
	{
		return ZBX_DB_ERROR_PARAMS;
	}
}

/**
 * zbx_db_clean_result
 * Free all the memory allocated by the struct zbx_db_result
 * Return ZBX_DB_OK on success
 */
int zbx_db_clean_result(struct zbx_db_result * e_result) 
{
	unsigned int col, row;

	if (NULL != e_result)
	{
		if (NULL != e_result->data)
		{
			for (row = 0; row < e_result->nb_rows; row++)
			{
				for (col = 0; col < e_result->nb_columns; col++)
				{
					if (ZBX_DB_OK != zbx_db_clean_data(&e_result->data[row][col]))
					{
						return ZBX_DB_ERROR_MEMORY;
					}
				}
				zbx_db_free(e_result->data[row]);
			}
			zbx_db_free(e_result->data);
		}
		if (NULL != e_result->fields)
		{
			for (col = 0; col < e_result->nb_columns; col++) 
			{
				if (ZBX_DB_OK != zbx_db_clean_fields(&e_result->fields[0][col]))
				{
					return ZBX_DB_ERROR_MEMORY;
				}
			}
			zbx_db_free(e_result->fields);
		}
		return ZBX_DB_OK;
	}
	else
	{
		return ZBX_DB_ERROR_PARAMS;
	}
}

unsigned int zbx_db_get_oracle_mode(int ora_mode)
{
	int rc = ZBX_DB_OCI_DEFAULT;

	if (NULL == ora_mode)
	{ 
		rc = ZBX_DB_OCI_DEFAULT;
	}
	else
	{
		switch (ora_mode)
		{
			case 0:
			default:
				rc = ZBX_DB_OCI_DEFAULT;
				break;
			case 1:
				rc = ZBX_DB_OCI_SYSDBA;
				break;
			case 2:
				rc = ZBX_DB_OCI_SYSOPER;
				break;
			case 3:
				rc = ZBX_DB_OCI_SYSASM;
				break;
			case 4:
				rc = ZBX_DB_OCI_SYSDGD;
				break;
		}
	}

	return rc;
}