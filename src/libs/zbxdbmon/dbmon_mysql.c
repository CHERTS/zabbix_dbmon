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
#include "threads.h"
#include "log.h"
#include "zbxdbmon.h"

#if defined(HAVE_DBMON)
#if defined(HAVE_MYSQL)
#include "mysql.h"
#include "errmsg.h"
#include "mysqld_error.h"

#ifdef _WINDOWS
#include "strptime.h"
#else
#include <time.h>
#endif

/**
 * MySQL handle
 */
struct zbx_db_mysql
{
	char			*host;
	char			*user;
	char			*passwd;
	char			*db;
	unsigned int	port;
	char			*unix_socket;
	unsigned long	flags;
	MYSQL			*db_handle;
	pthread_mutex_t	lock;
};

/**
 * zbx_db_get_mysql_value
 * convert value into a struct zbx_db_data * depening on the m_type given
 * returned value must be free'd with zbx_db_clean_data_full after use
 */
struct zbx_db_data *zbx_db_get_mysql_value(const char *value, const unsigned long length, const int m_type)
{
	struct zbx_db_data		*data = NULL;
	long long int			i_value;
	double					d_value;
	struct tm				tm_value;
	char					*endptr;

	if (value != NULL)
	{
		switch (m_type)
		{
			case FIELD_TYPE_DECIMAL:
			case FIELD_TYPE_NEWDECIMAL:
			case FIELD_TYPE_TINY:
			case FIELD_TYPE_SHORT:
			case FIELD_TYPE_LONG:
			case FIELD_TYPE_LONGLONG:
			case FIELD_TYPE_INT24:
			case FIELD_TYPE_YEAR:
				i_value = strtoll(value, &endptr, 10);
				if (endptr != value)
				{
					data = zbx_db_new_data_int(i_value);
				}
				else
				{
					data = zbx_db_new_data_null();
				}
				break;
			case FIELD_TYPE_BIT:
				i_value = strtol(value, &endptr, 2);
				if (endptr != value)
				{
					data = zbx_db_new_data_int(i_value);
				}
				else
				{
					data = zbx_db_new_data_null();
				}
				break;
			case FIELD_TYPE_FLOAT:
			case FIELD_TYPE_DOUBLE:
				d_value = strtod(value, &endptr);
				if (endptr != value)
				{
					data = zbx_db_new_data_double(d_value);
				}
				else
				{
					data = zbx_db_new_data_null();
				}
				break;
			case FIELD_TYPE_NULL:
				data = zbx_db_new_data_null();
				break;
			case FIELD_TYPE_DATE:
				// %F - Equivalent to %Y-%m-%d, the ISO 8601 date format.
				if (NULL == strptime(value, "%F", &tm_value))
				{
					data = zbx_db_new_data_null();
				}
				else 
				{
					data = zbx_db_new_data_datetime(&tm_value);
				}
				break;
			case FIELD_TYPE_TIME:
				// %T - Equivalent to %H:%M:%S
				if (NULL == strptime(value, "%T", &tm_value))
				{
					data = zbx_db_new_data_null();
				}
				else
				{
					data = zbx_db_new_data_datetime(&tm_value);
				}
				break;
			case FIELD_TYPE_TIMESTAMP:
			case FIELD_TYPE_DATETIME:
			case FIELD_TYPE_NEWDATE:
				// %F - Equivalent to %Y-%m-%d, the ISO 8601 date format.
				// %T - Equivalent to %H:%M:%S
				// Format - %Y-%m-%d %H:%M:%S
				if (NULL == strptime(value, "%F %T", &tm_value))
				{
					data = zbx_db_new_data_null();
				}
				else
				{
					data = zbx_db_new_data_datetime(&tm_value);
				}
				break;
			case FIELD_TYPE_TINY_BLOB:
			case FIELD_TYPE_MEDIUM_BLOB:
			case FIELD_TYPE_LONG_BLOB:
			case FIELD_TYPE_BLOB:
				if (length > 0)
				{
					data = zbx_db_new_data_blob(value, length);
				}
				else
				{
					data = zbx_db_new_data_null();
				}
				break;
			case FIELD_TYPE_VAR_STRING:
			case FIELD_TYPE_ENUM:
			case FIELD_TYPE_SET:
			case FIELD_TYPE_GEOMETRY:
			default:
				data = zbx_db_new_data_text(value, length);
				break;
		}
	}
	else
	{
		data = zbx_db_new_data_null();
	}

	return data;
}

/**
 * zbx_db_execute_query_mysql
 * Execute a query on a mysql connection, set the result structure with the returned values
 * Should not be executed by the user because all parameters are supposed to be correct
 * if result is NULL, the query is executed but no value will be returned
 * Return ZBX_DB_OK on success
 */
int zbx_db_execute_query_mysql(const struct zbx_db_connection *conn, struct zbx_db_result *m_result, const char *query)
{
	MYSQL_RES				*result;
	unsigned int			num_fields, col, row;
	MYSQL_ROW				m_row;
	MYSQL_FIELD				*fields;
	struct zbx_db_data		*data, *cur_row = NULL;
	struct zbx_db_fields	*db_fields, *cur_field = NULL;
	unsigned long			*lengths;
	int						res;

	if (pthread_mutex_lock(&(((struct zbx_db_mysql *)conn->connection)->lock)))
	{
		return ZBX_DB_ERROR_QUERY;
	}

	if (mysql_query(((struct zbx_db_mysql *)conn->connection)->db_handle, query))
	{
		zbx_db_err_log(ZBX_DB_TYPE_MYSQL, ERR_Z3005, mysql_errno(((struct zbx_db_mysql *)conn->connection)->db_handle), mysql_error(((struct zbx_db_mysql *)conn->connection)->db_handle), query);
		pthread_mutex_unlock(&(((struct zbx_db_mysql *)conn->connection)->lock));
		return ZBX_DB_ERROR_QUERY;
	}

	if (NULL != m_result) 
	{
		result = mysql_store_result(((struct zbx_db_mysql *)conn->connection)->db_handle);

		if (NULL == result)
		{
			zbx_db_err_log(ZBX_DB_TYPE_MYSQL, ERR_Z3005, mysql_errno(((struct zbx_db_mysql *)conn->connection)->db_handle), mysql_error(((struct zbx_db_mysql *)conn->connection)->db_handle), NULL);
			pthread_mutex_unlock(&(((struct zbx_db_mysql *)conn->connection)->lock));
			return ZBX_DB_ERROR_QUERY;
		}

		num_fields = mysql_num_fields(result);
		fields = mysql_fetch_fields(result);

		m_result->nb_rows = 0;
		m_result->nb_columns = num_fields;
		m_result->fields = NULL;
		m_result->data = NULL;

		// Get fields name
		cur_field = NULL;
		for (col = 0; col < num_fields; col++)
		{
			db_fields = zbx_db_fields_value(fields[col].name, fields[col].name_length);
			res = zbx_db_row_add_fields(&cur_field, db_fields, col);
			zbx_db_clean_fields_full(db_fields);

			if (ZBX_DB_OK != res)
			{
				mysql_free_result(result);
				pthread_mutex_unlock(&(((struct zbx_db_mysql *)conn->connection)->lock));
				return res;
			}

			zabbix_log(LOG_LEVEL_TRACE, "In %s(): Col_name: %s, Col_lenght: %d", __func__, fields[col].name, fields[col].name_length);
		}

		res = zbx_db_result_add_fields(m_result, cur_field);

		if (ZBX_DB_OK != res)
		{
			mysql_free_result(result);
			pthread_mutex_unlock(&(((struct zbx_db_mysql *)conn->connection)->lock));
			return res;
		}

		mysql_field_seek(result, 0);
		fields = mysql_fetch_fields(result);

		for (row = 0; (m_row = mysql_fetch_row(result)) != NULL; row++)
		{
			cur_row = NULL;
			lengths = mysql_fetch_lengths(result);

			for (col = 0; col < num_fields; col++)
			{
				data = zbx_db_get_mysql_value(m_row[col], lengths[col], fields[col].type);
				res = zbx_db_row_add_data(&cur_row, data, col);
				zbx_db_clean_data_full(data);

				if (ZBX_DB_OK != res)
				{
					mysql_free_result(result);
					pthread_mutex_unlock(&(((struct zbx_db_mysql *)conn->connection)->lock));
					return res;
				}
			}

			res = zbx_db_result_add_row(m_result, cur_row, row);

			if (ZBX_DB_OK != res)
			{
				mysql_free_result(result);
				pthread_mutex_unlock(&(((struct zbx_db_mysql *)conn->connection)->lock));
				return res;
			}
		}

		mysql_free_result(result);
	}

	pthread_mutex_unlock(&(((struct zbx_db_mysql *)conn->connection)->lock));

	return ZBX_DB_OK;
}

unsigned long	zbx_db_get_version_mysql(const struct zbx_db_connection *conn)
{
	unsigned long	version = 0L;

	if (pthread_mutex_lock(&(((struct zbx_db_mysql *)conn->connection)->lock)))
	{
		return 0;
	}

	version = mysql_get_server_version(((struct zbx_db_mysql *)conn->connection)->db_handle);

	zabbix_log(LOG_LEVEL_TRACE, "In %s(): MySQL version: %lu", __func__, version);

	pthread_mutex_unlock(&(((struct zbx_db_mysql *)conn->connection)->lock));

	return version;
}

/**
 * zbx_db_connect_mysql
 * Opens a database connection to a MySQL server
 * Return pointer to a struct zbx_db_connection * on sucess, NULL on error
 */
struct zbx_db_connection *zbx_db_connect_mysql(const char *host, const char *user, const char *passwd, 
												const char *dbname, const unsigned int port, const char *dbsocket)
{
	struct zbx_db_connection	*conn = NULL;
	my_bool						reconnect = 1;
	my_bool						connect_timeout = 3;
	pthread_mutexattr_t			mutexattr;

	if (NULL != host && NULL != dbname)
	{
		conn = (struct zbx_db_connection *)zbx_db_malloc(sizeof(struct zbx_db_connection));
		if (NULL == conn)
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for conn", __func__);
			return NULL;
		}

		conn->type = ZBX_DB_TYPE_MYSQL;
		conn->connection = (struct zbx_db_mysql *)zbx_db_malloc(sizeof(struct zbx_db_mysql));

		if (NULL == conn->connection)
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for conn->connection", __func__);
			zbx_db_free(conn);
			conn = NULL;
			return NULL;
		}

		if (mysql_library_init(0, NULL, NULL))
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): mysql_library_init error, aborting", __func__);
			zbx_db_free(conn->connection);
			zbx_db_free(conn);
			conn = NULL;
			return NULL;
		}

		((struct zbx_db_mysql *)conn->connection)->db_handle = mysql_init(NULL);

		if (NULL == ((struct zbx_db_mysql *)conn->connection)->db_handle)
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): mysql_init error, aborting", __func__);
			zbx_db_free(conn->connection);
			zbx_db_free(conn);
			conn = NULL;
			return NULL;
		}

		if (NULL == mysql_real_connect(((struct zbx_db_mysql *)conn->connection)->db_handle,
			host, user, passwd, dbname, port, dbsocket, CLIENT_MULTI_STATEMENTS))
		{
			zbx_db_err_log(ZBX_DB_TYPE_MYSQL, ERR_Z3001, mysql_errno(((struct zbx_db_mysql *)conn->connection)->db_handle), mysql_error(((struct zbx_db_mysql *)conn->connection)->db_handle), dbname);
			mysql_close(((struct zbx_db_mysql *)conn->connection)->db_handle);
			zbx_db_free(conn->connection);
			zbx_db_free(conn);
			conn = NULL;
			return NULL;
		}
		else
		{
			if (0 != mysql_set_character_set(((struct zbx_db_mysql *)conn->connection)->db_handle, "utf8"))
				zabbix_log(LOG_LEVEL_WARNING, "In %s(): Cannot set MySQL character set to \"utf8\"", __func__);

			/* Set MYSQL_OPT_RECONNECT to true to reconnect automatically when connection is closed by the server (to avoid CR_SERVER_GONE_ERROR) */
			if (0 != mysql_options(((struct zbx_db_mysql *)conn->connection)->db_handle, MYSQL_OPT_RECONNECT, &reconnect))
				zabbix_log(LOG_LEVEL_WARNING, "In %s(): Cannot set MySQL options MYSQL_OPT_RECONNECT", __func__);

			/* Set MYSQL_OPT_CONNECT_TIMEOUT to reconnect automatically when connection is closed by the server (to avoid CR_SERVER_GONE_ERROR) */
			if (0 != mysql_options(((struct zbx_db_mysql *)conn->connection)->db_handle, MYSQL_OPT_CONNECT_TIMEOUT, &connect_timeout))
				zabbix_log(LOG_LEVEL_WARNING, "In %s(): Cannot set MySQL options MYSQL_OPT_CONNECT_TIMEOUT", __func__);

			/* Initialize MUTEX for connection */
			pthread_mutexattr_init(&mutexattr);
			pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);

			if (0 != pthread_mutex_init(&(((struct zbx_db_mysql *)conn->connection)->lock), &mutexattr))
			{
				zabbix_log(LOG_LEVEL_CRIT, "In %s(): Impossible to initialize Mutex Lock for MySQL connection", __func__);
			}

			pthread_mutexattr_destroy(&mutexattr);

			return conn;
		}
	}

	return conn;
}

/**
 * Close connection to MySQL database
 */
void zbx_db_close_mysql(struct zbx_db_connection *conn)
{
	mysql_close(((struct zbx_db_mysql *)conn->connection)->db_handle);
	mysql_library_end();
	pthread_mutex_destroy(&((struct zbx_db_mysql *)conn->connection)->lock);
}

#endif
#endif
