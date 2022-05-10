/*
** Zabbix
** Copyright (C) 2019-2021 Mikhail Grigorev
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
#if defined(HAVE_POSTGRESQL)
#include <libpq-fe.h>

struct zbx_db_pg_type
{
	Oid            pg_type;
	unsigned short col_type;
};

/**
 * Postgre SQL handle
 */
struct zbx_db_pgsql
{
	char					*conninfo;
	PGconn					*db_handle;
	unsigned int			nb_type;
	struct zbx_db_pg_type	*list_type;
	pthread_mutex_t			lock;
};

static void	zbx_db_pgsql_error(char **error, const PGresult *pg_result)
{
	char	*result_error_msg;
	size_t	error_alloc = 0, error_offset = 0;

	zbx_snprintf_alloc(error, &error_alloc, &error_offset, "%s", PQresStatus(PQresultStatus(pg_result)));

	result_error_msg = PQresultErrorMessage(pg_result);

	if ('\0' != *result_error_msg)
		zbx_snprintf_alloc(error, &error_alloc, &error_offset, ":%s", result_error_msg);
}

/**
 * Return the type of a column given its Oid
 * If type is not found, return ZBX_COL_TYPE_TEXT
 */
static unsigned short zbx_db_get_type_from_oid(const struct zbx_db_connection *conn, Oid pg_type)
{
	unsigned int i;

	for (i = 0; i < ((struct zbx_db_pgsql *)conn->connection)->nb_type; i++)
	{
		if (((struct zbx_db_pgsql *)conn->connection)->list_type[i].pg_type == pg_type)
		{
			return ((struct zbx_db_pgsql *)conn->connection)->list_type[i].col_type;
		}
	}

	pthread_mutex_unlock(&(((struct zbx_db_pgsql *)conn->connection)->lock));

	return ZBX_COL_TYPE_TEXT;
}

/**
 * zbx_db_execute_query_pgsql
 * Execute a query on a pgsql connection, set the result structure with the returned values
 * Should not be executed by the user because all parameters are supposed to be correct
 * if result is NULL, the query is executed but no value will be returned
 * return ZBX_DB_OK on success
 */
int zbx_db_execute_query_pgsql(const struct zbx_db_connection *conn, struct zbx_db_result *p_result, const char *query)
{
	PGresult *res;
	int nfields, ntuples, i, j, h_res, f_res, ret = ZBX_DB_OK;
	struct zbx_db_data *db_data, *cur_row = NULL;
	struct zbx_db_fields *db_fields, *cur_field = NULL;
	char *error = NULL;

	if (pthread_mutex_lock(&(((struct zbx_db_pgsql *)conn->connection)->lock)))
	{
		ret = ZBX_DB_ERROR_QUERY;
	}
	else
	{
		res = PQexec(((struct zbx_db_pgsql *)conn->connection)->db_handle, query);

		if (NULL == res)
			zbx_db_err_log(ZBX_DB_TYPE_POSTGRESQL, ERR_Z3005, 0, "result is NULL", query);

		if (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res))
		{
			zbx_db_pgsql_error(&error, res);
			zbx_db_err_log(ZBX_DB_TYPE_POSTGRESQL, ERR_Z3005, 0, error, query);
			zbx_free(error);
			ret = ZBX_DB_ERROR_QUERY;
		}
		else
		{
			if (NULL != p_result)
			{
				nfields = PQnfields(res);
				ntuples = PQntuples(res);

				zabbix_log(LOG_LEVEL_TRACE, "In %s(): nfields = %d, ntuples = %d", __func__, nfields, ntuples);

				p_result->nb_rows = 0;
				p_result->nb_columns = nfields;
				p_result->fields = NULL;
				p_result->data = NULL;

				for (i = 0; ret == ZBX_DB_OK && i < ntuples; i++)
				{
					cur_row = NULL;
					cur_field = NULL;
					for (j = 0; ret == ZBX_DB_OK && j < nfields; j++)
					{
						char *val = NULL;
						
						val = PQgetvalue(res, i, j);

						if (NULL == val)
						{
							db_data = zbx_db_new_data_null();
						}
						else
						{
							switch (zbx_db_get_type_from_oid(conn, PQftype(res, j)))
							{
								case ZBX_COL_TYPE_INT:
									db_data = zbx_db_new_data_int(strtoll(val, NULL, 10));
									break;
								case ZBX_COL_TYPE_DOUBLE:
									db_data = zbx_db_new_data_double(strtod(val, NULL));
									break;
								case ZBX_COL_TYPE_BLOB:
									db_data = zbx_db_new_data_blob(val, PQgetlength(res, i, j));
									break;
								case ZBX_COL_TYPE_BOOL:
									if (0 == zbx_db_strcasecmp(val, "t"))
									{
										db_data = zbx_db_new_data_int(1);
									}
									else if (0 == zbx_db_strcasecmp(val, "f"))
									{
										db_data = zbx_db_new_data_int(0);
									}
									else
									{
										db_data = zbx_db_new_data_null();
									}
									break;
								case ZBX_COL_TYPE_DATE:
								case ZBX_COL_TYPE_TEXT:
								default:
									db_data = zbx_db_new_data_text(val, PQgetlength(res, i, j));
									break;
							}
						}

						if (0 == i)
						{
							zabbix_log(LOG_LEVEL_TRACE, "In %s(): PQfsize=%d, PQfsize=%zu, PQfname=%s", __func__, PQfsize(res, j), strlen((const char*)PQfname(res, j)), PQfname(res, j));

							db_fields = zbx_db_fields_value(PQfname(res, j), strlen((const char*)PQfname(res, j)));
							f_res = zbx_db_row_add_fields(&cur_field, db_fields, j);
							zbx_db_clean_fields_full(db_fields);

							if (ZBX_DB_OK != f_res)
							{
								PQclear(res);
								ret = f_res;
							}
						}

						zabbix_log(LOG_LEVEL_TRACE, "In %s(): OID=%d, PQgetlength=%d, PQgetvalue=%s", __func__, zbx_db_get_type_from_oid(conn, PQftype(res, j)), PQgetlength(res, i, j), val);

						h_res = zbx_db_row_add_data(&cur_row, db_data, j);
						zbx_db_clean_data_full(db_data);

						if (ZBX_DB_OK != h_res)
						{
							PQclear(res);
							ret = h_res;
						}
					}

					if (0 == i)
					{
						f_res = zbx_db_result_add_fields(p_result, cur_field);

						if (ZBX_DB_OK != f_res)
						{
							PQclear(res);
							ret = f_res;
						}
					}

					h_res = zbx_db_result_add_row(p_result, cur_row, i);

					if (ZBX_DB_OK != h_res)
					{
						PQclear(res);
						ret = h_res;
					}
				}
			}
		}

		PQclear(res);

		pthread_mutex_unlock(&(((struct zbx_db_pgsql *)conn->connection)->lock));
	}

	return ret;
}

int	zbx_db_get_version_pgsql(const struct zbx_db_connection *conn)
{
	int	version = 0;

	if (pthread_mutex_lock(&(((struct zbx_db_pgsql *)conn->connection)->lock)))
	{
		return 0;
	}

	version = PQserverVersion(((struct zbx_db_pgsql *)conn->connection)->db_handle);

	if (0 > version)
		version = 0;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(): PostgreSQL version: %d", __func__, version);

	pthread_mutex_unlock(&(((struct zbx_db_pgsql *)conn->connection)->lock));

	return version;
}

/**
 * zbx_db_connect_pgsql
 * Opens a database connection to a PostgreSQL server
 * Return pointer to a struct zbx_db_connection * on sucess, NULL on error
 */
struct zbx_db_connection *zbx_db_connect_pgsql(const char *conn_string)
{
	struct zbx_db_connection* conn = NULL;
	int ntuples, i;
	PGresult *res;
	pthread_mutexattr_t mutexattr;

	if (NULL != conn_string)
	{
		conn = (struct zbx_db_connection *)zbx_db_malloc(sizeof(struct zbx_db_connection));

		if (NULL == conn)
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for conn", __func__);
			return NULL;
		}

		conn->type = ZBX_DB_TYPE_POSTGRESQL;
		conn->connection = (struct zbx_db_pgsql *)zbx_db_malloc(sizeof(struct zbx_db_pgsql));

		if (NULL == conn->connection)
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for conn->connection", __func__);
			zbx_db_free(conn);
			conn = NULL;
			return NULL;
		}

		((struct zbx_db_pgsql *)conn->connection)->db_handle = PQconnectdb(conn_string);
		((struct zbx_db_pgsql *)conn->connection)->nb_type = 0;
		((struct zbx_db_pgsql *)conn->connection)->list_type = NULL;

		if (CONNECTION_OK != PQstatus(((struct zbx_db_pgsql *)conn->connection)->db_handle))
		{
			zbx_db_err_log(ZBX_DB_TYPE_POSTGRESQL, ERR_Z3001, 0, PQerrorMessage(((struct zbx_db_pgsql *)conn->connection)->db_handle), conn_string);
			PQfinish(((struct zbx_db_pgsql *)conn->connection)->db_handle);
			zbx_db_free(conn->connection);
			zbx_db_free(conn);
			conn = NULL;
			return NULL;
		}
		else
		{
			res = PQexec(((struct zbx_db_pgsql *)conn->connection)->db_handle, "select oid, typname from pg_type");

			if (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res) && 2 == PQnfields(res))
			{
				zbx_db_err_log(ZBX_DB_TYPE_POSTGRESQL, ERR_Z3005, 0, PQerrorMessage(((struct zbx_db_pgsql *)conn->connection)->db_handle), "\"select oid, typname from pg_type\"");
				PQclear(res);
				PQfinish(((struct zbx_db_pgsql *)conn->connection)->db_handle);
				zbx_db_free(conn->connection);
				zbx_db_free(conn);
				conn = NULL;
				return NULL;
			}
			else
			{
				ntuples = PQntuples(res);
				((struct zbx_db_pgsql *)conn->connection)->list_type = zbx_db_malloc((ntuples + 1) * sizeof(struct zbx_db_pg_type));

				if (((struct zbx_db_pgsql *)conn->connection)->list_type != NULL)
				{
					((struct zbx_db_pgsql *)conn->connection)->nb_type = ntuples;
					for (i = 0; i < ntuples; i++)
					{
						char *cur_type_name = PQgetvalue(res, i, 1);
						((struct zbx_db_pgsql *)conn->connection)->list_type[i].pg_type = strtol(PQgetvalue(res, i, 0), NULL, 10);

						if (zbx_db_strcmp(cur_type_name, "bool") == 0)
						{
							((struct zbx_db_pgsql *)conn->connection)->list_type[i].col_type = ZBX_COL_TYPE_BOOL;
						}
						else if (zbx_db_strncmp(cur_type_name, "int", 3) == 0 || (zbx_db_strncmp(cur_type_name + 1, "id", 2) == 0 && zbx_db_strlen(cur_type_name) == 3))
						{
							((struct zbx_db_pgsql *)conn->connection)->list_type[i].col_type = ZBX_COL_TYPE_INT;
						}
						else if (zbx_db_strcmp(cur_type_name, "numeric") == 0 || zbx_db_strncmp(cur_type_name, "float", 5) == 0)
						{
							((struct zbx_db_pgsql *)conn->connection)->list_type[i].col_type = ZBX_COL_TYPE_DOUBLE;
						}
						else if (zbx_db_strcmp(cur_type_name, "date") == 0 || zbx_db_strncmp(cur_type_name, "time", 4) == 0)
						{
							((struct zbx_db_pgsql *)conn->connection)->list_type[i].col_type = ZBX_COL_TYPE_DATE;
						}
						else if (zbx_db_strcmp(cur_type_name, "bytea") == 0)
						{
							((struct zbx_db_pgsql *)conn->connection)->list_type[i].col_type = ZBX_COL_TYPE_BLOB;
						}
						else
						{
							((struct zbx_db_pgsql *)conn->connection)->list_type[i].col_type = ZBX_COL_TYPE_TEXT;
						}
					}

					/* Initialize MUTEX for connection */
					pthread_mutexattr_init(&mutexattr);
					pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);

					if (0 != pthread_mutex_init(&(((struct zbx_db_pgsql *)conn->connection)->lock), &mutexattr))
					{
						zabbix_log(LOG_LEVEL_CRIT, "In %s(): Impossible to initialize Mutex Lock for PostgreSQL connection", __func__);
					}

					pthread_mutexattr_destroy(&mutexattr);
				}
				else
				{
					zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating resources for list_type", __func__);
					PQclear(res);
					PQfinish(((struct zbx_db_pgsql *)conn->connection)->db_handle);
					zbx_db_free(conn->connection);
					zbx_db_free(conn);
					conn = NULL;
					return NULL;
				}

				PQclear(res);
			}
		}
	}

	return conn;
}

/**
 * Close connection to PostgreSQL database
 */
void zbx_db_close_pgsql(struct zbx_db_connection *conn)
{
	PQfinish(((struct zbx_db_pgsql *)conn->connection)->db_handle);
	zbx_db_free(((struct zbx_db_pgsql *)conn->connection)->list_type);
	((struct zbx_db_pgsql *)conn->connection)->list_type = NULL;
	((struct zbx_db_pgsql *)conn->connection)->nb_type = 0;
	pthread_mutex_destroy(&((struct zbx_db_pgsql *)conn->connection)->lock);
}

#endif
#endif
