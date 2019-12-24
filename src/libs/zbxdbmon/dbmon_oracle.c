#include "common.h"
#include "threads.h"
#include "log.h"
#include "zbxdbmon.h"

#if defined(HAVE_ORACLE)
#	include "oci.h"

/**
 * Oracle SQL handle
 */
struct zbx_db_oracle
{
	char			*host;
	char			*user;
	char			*passwd;
	char			*db;
	unsigned int	port;
	OCIEnv			*envhp;
	OCIError		*errhp;
	OCISvcCtx		*svchp;
	OCIServer		*srvhp;
	OCIStmt			*stmthp;
	OCISession		*authp;
	pthread_mutex_t	lock;
};

struct zbx_db_ora_result
{
	OCIStmt			*stmthp;
	unsigned int	ncolumn;
	text			**colname;
	ub4				*colname_alloc;
	char			**values;
	ub4				*values_alloc;
	OCILobLocator	**clobs;
};

typedef struct zbx_db_ora_result	*ORA_DB_RESULT;

/* server status: OCI_SERVER_NORMAL or OCI_SERVER_NOT_CONNECTED */
static ub4 zbx_db_oci_dbserver_status(const struct zbx_db_connection *conn)
{
	sword	err;
	ub4	server_status = OCI_SERVER_NOT_CONNECTED;

	err = OCIAttrGet((void *)((struct zbx_db_oracle *)conn->connection)->srvhp, OCI_HTYPE_SERVER, (void *)&server_status,
		(ub4 *)0, OCI_ATTR_SERVER_STATUS, (OCIError *)((struct zbx_db_oracle *)conn->connection)->errhp);

	if (OCI_SUCCESS != err)
		zabbix_log(LOG_LEVEL_WARNING, "Cannot determine Oracle server status, assuming not connected");

	return server_status;
}

static const char *zbx_db_oci_error(const struct zbx_db_connection *conn, sword status, sb4 *err)
{
	static char	errbuf[512];
	sb4			errcode, *perrcode;

	perrcode = (NULL == err ? &errcode : err);

	errbuf[0] = '\0';
	*perrcode = 0;

	switch (status)
	{
		case OCI_SUCCESS_WITH_INFO:
			OCIErrorGet((dvoid *)((struct zbx_db_oracle *)conn->connection)->errhp, (ub4)1, (text *)NULL, perrcode,
						(text *)errbuf, (ub4)sizeof(errbuf), OCI_HTYPE_ERROR);
			break;
		case OCI_NEED_DATA:
			zbx_snprintf(errbuf, sizeof(errbuf), "%s", "OCI_NEED_DATA");
			break;
		case OCI_NO_DATA:
			zbx_snprintf(errbuf, sizeof(errbuf), "%s", "OCI_NODATA");
			break;
		case OCI_ERROR:
			OCIErrorGet((dvoid *)((struct zbx_db_oracle *)conn->connection)->errhp, (ub4)1, (text *)NULL, perrcode,
						(text *)errbuf, (ub4)sizeof(errbuf), OCI_HTYPE_ERROR);
			break;
		case OCI_INVALID_HANDLE:
			zbx_snprintf(errbuf, sizeof(errbuf), "%s", "OCI_INVALID_HANDLE");
			break;
		case OCI_STILL_EXECUTING:
			zbx_snprintf(errbuf, sizeof(errbuf), "%s", "OCI_STILL_EXECUTING");
			break;
		case OCI_CONTINUE:
			zbx_snprintf(errbuf, sizeof(errbuf), "%s", "OCI_CONTINUE");
			break;
	}

	zbx_rtrim(errbuf, ZBX_WHITESPACE);

	return errbuf;
}

static int	zbx_db_oci_handle_sql_error(const struct zbx_db_connection *conn, int zerrcode, sword oci_error, const char *sql)
{
	sb4	errcode;
	int	ret = ZBX_DB_ERROR_CONNECTION;

	zbx_db_err_log(ZBX_DB_TYPE_ORACLE, zerrcode, oci_error, zbx_db_oci_error(conn, oci_error, &errcode), sql);

	/* after ORA-02396 (and consequent ORA-01012) errors the OCI_SERVER_NORMAL server status is still returned */
	switch (errcode)
	{
		case 1012:	/* ORA-01012: not logged on */
		case 2396:	/* ORA-02396: exceeded maximum idle time */
			goto out;
	}

	if (OCI_SERVER_NORMAL == zbx_db_oci_dbserver_status(conn))
		ret = ZBX_DB_ERROR;
out:
	return ret;
}

static void	zbx_db_oci_clean_result(ORA_DB_RESULT result)
{
	if (NULL == result)
		return;

	if (NULL != result->values)
	{
		unsigned int	i;

		for (i = 0; i < result->ncolumn; i++)
		{
			zbx_free(result->values[i]);

			/* deallocate the lob locator variable */
			if (NULL != result->clobs[i])
			{
				OCIDescriptorFree((dvoid *)result->clobs[i], OCI_DTYPE_LOB);
				result->clobs[i] = NULL;
			}
		}

		zbx_free(result->values);
		zbx_free(result->clobs);
		zbx_free(result->values_alloc);
	}

	if (result->stmthp)
	{
		OCIHandleFree((dvoid *)result->stmthp, OCI_HTYPE_STMT);
		result->stmthp = NULL;
	}
}

void	zbx_db_free_result_oracle(ORA_DB_RESULT result)
{
	if (NULL == result)
		return;

	zbx_db_oci_clean_result(result);

	zbx_free(result);
}

ORA_DB_RESULT zbx_db_select_oracle(const struct zbx_db_connection *conn, const char *fmt, va_list args)
{
	char			*sql = NULL;
	ORA_DB_RESULT	result = NULL;
	double			sec = 0;
	sword			err = OCI_SUCCESS;
	ub4				prefetch_rows = 200, counter, col_name_len;
	text			*col_name;

	sql = zbx_dvsprintf(sql, fmt, args);

	zabbix_log(LOG_LEVEL_TRACE, "In %s: SQL: %s", __func__, sql);

	result = (ORA_DB_RESULT)zbx_malloc(NULL, sizeof(struct zbx_db_ora_result));
	memset(result, 0, sizeof(struct zbx_db_ora_result));

	err = OCIHandleAlloc((dvoid *)((struct zbx_db_oracle *)conn->connection)->envhp, (dvoid **)&result->stmthp, OCI_HTYPE_STMT, (size_t)0, (dvoid **)0);

	/* Prefetching when working with Oracle is needed because otherwise it fetches only 1 row at a time when doing */
	/* selects (default behavior). There are 2 ways to do prefetching: memory based and rows based. Based on the   */
	/* study optimal (speed-wise) memory based prefetch is 2 MB. But in case of many subsequent selects CPU usage  */
	/* jumps up to 100 %. Using rows prefetch with up to 200 rows does not affect CPU usage, it is the same as     */
	/* without prefetching at all. See ZBX-5920, ZBX-6493 for details.                                             */
	/*                                                                                                             */
	/* Tested on Oracle 11gR2.                                                                                     */
	/*                                                                                                             */
	/* Oracle info: docs.oracle.com/cd/B28359_01/appdev.111/b28395/oci04sql.htm                                    */

	if (OCI_SUCCESS == err)
	{
		err = OCIAttrSet(result->stmthp, OCI_HTYPE_STMT, &prefetch_rows, sizeof(prefetch_rows),
			OCI_ATTR_PREFETCH_ROWS, ((struct zbx_db_oracle *)conn->connection)->errhp);
	}

	if (OCI_SUCCESS == err)
	{
		err = OCIStmtPrepare(result->stmthp, ((struct zbx_db_oracle *)conn->connection)->errhp, (text *)sql, (ub4)strlen((char *)sql),
			(ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
	}

	if (OCI_SUCCESS == err)
	{
		err = OCIStmtExecute(((struct zbx_db_oracle *)conn->connection)->svchp, result->stmthp, ((struct zbx_db_oracle *)conn->connection)->errhp, (ub4)0, (ub4)0,
			(CONST OCISnapshot *)NULL, (OCISnapshot *)NULL, OCI_DEFAULT);
	}

	if (OCI_SUCCESS == err)
	{
		/* Get the number of columns in the query */
		err = OCIAttrGet((void *)result->stmthp, OCI_HTYPE_STMT, (void *)&result->ncolumn,
			(ub4 *)0, OCI_ATTR_PARAM_COUNT, ((struct zbx_db_oracle *)conn->connection)->errhp);
	}

	if (OCI_SUCCESS != err)
		goto error;

	assert(0 < result->ncolumn);

	result->clobs = (OCILobLocator **)zbx_malloc(NULL, result->ncolumn * sizeof(OCILobLocator *));
	result->colname = (text **)zbx_malloc(NULL, result->ncolumn * sizeof(text *));
	result->colname_alloc = (ub4 *)zbx_malloc(NULL, result->ncolumn * sizeof(ub4));
	result->values = (char **)zbx_malloc(NULL, result->ncolumn * sizeof(char *));
	result->values_alloc = (ub4 *)zbx_malloc(NULL, result->ncolumn * sizeof(ub4));
	memset(result->clobs, 0, result->ncolumn * sizeof(OCILobLocator *));
	memset(result->colname, 0, result->ncolumn * sizeof(text *));
	memset(result->colname_alloc, 0, result->ncolumn * sizeof(ub4));
	memset(result->values, 0, result->ncolumn * sizeof(char *));
	memset(result->values_alloc, 0, result->ncolumn * sizeof(ub4));

	for (counter = 1; OCI_SUCCESS == err && counter <= (ub4)result->ncolumn; counter++)
	{
		OCIParam	*parmdp = NULL;
		OCIDefine	*defnp = NULL;
		ub4			char_semantics;
		ub2			col_width = 0, data_type = 0;

		/* Request a parameter descriptor for position 1 in the select-list */
		err = OCIParamGet((void *)result->stmthp, OCI_HTYPE_STMT, ((struct zbx_db_oracle *)conn->connection)->errhp, (void **)&parmdp, (ub4)counter);

		if (OCI_SUCCESS == err)
		{
			/* Retrieve the datatype attribute for the column */
			err = OCIAttrGet((void *)parmdp, OCI_DTYPE_PARAM, (dvoid *)&data_type, (ub4 *)NULL,
				(ub4)OCI_ATTR_DATA_TYPE, (OCIError *)((struct zbx_db_oracle *)conn->connection)->errhp);
		}

		if (SQLT_CLOB == data_type)
		{
			if (OCI_SUCCESS == err)
			{
				/* Allocate the lob locator variable */
				err = OCIDescriptorAlloc((dvoid *)((struct zbx_db_oracle *)conn->connection)->envhp, (dvoid **)&result->clobs[counter - 1],
					OCI_DTYPE_LOB, (size_t)0, (dvoid **)0);
			}

			if (OCI_SUCCESS == err)
			{
				/* Associate clob var with its define handle */
				err = OCIDefineByPos((OCIStmt *)result->stmthp, &defnp, (OCIError *)((struct zbx_db_oracle *)conn->connection)->errhp,
					(ub4)counter, (dvoid *)&result->clobs[counter - 1], (sb4)-1,
					data_type, (dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)OCI_DEFAULT);
			}
		}
		else
		{
			if (OCI_SUCCESS == err)
			{
				/* Retrieve the column name attribute */
				col_name_len = 0;
				err = OCIAttrGet((void*)parmdp, (ub4)OCI_DTYPE_PARAM, (void**)&col_name, (ub4 *)&col_name_len,
					(ub4)OCI_ATTR_NAME, (OCIError *)((struct zbx_db_oracle *)conn->connection)->errhp);
				if (OCI_SUCCESS == err)
				{
					col_name_len++; /* Add 1 byte for terminating '\0' */
					result->colname[counter - 1] = (text *)zbx_malloc(NULL, col_name_len);
					*result->colname[counter - 1] = '\0';
					result->colname_alloc[counter - 1] = col_name_len;
					memcpy(result->colname[counter - 1], col_name, col_name_len);
				}
			}

			if (OCI_SUCCESS == err)
			{
				/* Retrieve the length semantics for the column */
				char_semantics = 0;
				err = OCIAttrGet((void *)parmdp, (ub4)OCI_DTYPE_PARAM, (void *)&char_semantics,
					(ub4 *)NULL, (ub4)OCI_ATTR_CHAR_USED, (OCIError *)((struct zbx_db_oracle *)conn->connection)->errhp);
			}

			if (OCI_SUCCESS == err)
			{
				if (0 != char_semantics)
				{
					/* Retrieve the column width in characters */
					err = OCIAttrGet((void *)parmdp, (ub4)OCI_DTYPE_PARAM, (void *)&col_width,
						(ub4 *)NULL, (ub4)OCI_ATTR_CHAR_SIZE, (OCIError *)((struct zbx_db_oracle *)conn->connection)->errhp);

					/* Adjust for UTF-8 */
					col_width *= 4;
				}
				else
				{
					/* Retrieve the column width in bytes */
					err = OCIAttrGet((void *)parmdp, (ub4)OCI_DTYPE_PARAM, (void *)&col_width,
						(ub4 *)NULL, (ub4)OCI_ATTR_DATA_SIZE, (OCIError *)((struct zbx_db_oracle *)conn->connection)->errhp);
				}
			}
			col_width++;	/* Add 1 byte for terminating '\0' */

			result->values[counter - 1] = (char *)zbx_malloc(NULL, col_width);
			*result->values[counter - 1] = '\0';
			result->values_alloc[counter - 1] = col_width;

			if (OCI_SUCCESS == err)
			{
				/* Represent any data as characters */
				err = OCIDefineByPos(result->stmthp, &defnp, ((struct zbx_db_oracle *)conn->connection)->errhp, counter,
					(dvoid *)result->values[counter - 1], col_width, SQLT_STR,
					(dvoid *)0, (ub2 *)0, (ub2 *)0, OCI_DEFAULT);
			}
		}

		/* Free cell descriptor */
		OCIDescriptorFree(parmdp, OCI_DTYPE_PARAM);
		parmdp = NULL;
	}
error:
	if (OCI_SUCCESS != err)
	{
		int	server_status;

		server_status = zbx_db_oci_handle_sql_error(conn, ERR_Z3005, err, sql);
		zbx_db_free_result_oracle(result);

		result = (ZBX_DB_ERROR_CONNECTION == server_status ? (ORA_DB_RESULT)(intptr_t)server_status : NULL);
	}

	zbx_free(sql);

	return result;
}

char **zbx_db_fetch_oracle(const struct zbx_db_connection *conn, ORA_DB_RESULT result)
{
	unsigned int	i;
	sword			rc;
	static char		errbuf[512];
	sb4				errcode;

	if (NULL == result)
		return NULL;

	if (NULL == result->stmthp)
		return NULL;

	if (OCI_NO_DATA == (rc = OCIStmtFetch2(result->stmthp, ((struct zbx_db_oracle *)conn->connection)->errhp, 1, OCI_FETCH_NEXT, 0, OCI_DEFAULT)))
		return NULL;

	if (OCI_SUCCESS != rc)
	{
		ub4	rows_fetched;
		ub4	sizep = sizeof(ub4);

		if (OCI_SUCCESS != (rc = OCIErrorGet((dvoid *)((struct zbx_db_oracle *)conn->connection)->errhp, (ub4)1, (text *)NULL,
			&errcode, (text *)errbuf, (ub4)sizeof(errbuf), OCI_HTYPE_ERROR)))
		{
			zbx_db_err_log(ZBX_DB_TYPE_ORACLE, ERR_Z3006, rc, zbx_db_oci_error(conn, rc, NULL), NULL);
			return NULL;
		}

		switch (errcode)
		{
			case 1012:	/* ORA-01012: not logged on */
			case 2396:	/* ORA-02396: exceeded maximum idle time */
			case 3113:	/* ORA-03113: end-of-file on communication channel */
			case 3114:	/* ORA-03114: not connected to ORACLE */
				zbx_db_err_log(ZBX_DB_TYPE_ORACLE, ERR_Z3006, errcode, errbuf, NULL);
				return NULL;
			default:
				rc = OCIAttrGet((void *)result->stmthp, (ub4)OCI_HTYPE_STMT, (void *)&rows_fetched,
					(ub4 *)&sizep, (ub4)OCI_ATTR_ROWS_FETCHED, (OCIError *)((struct zbx_db_oracle *)conn->connection)->errhp);

				if (OCI_SUCCESS != rc || 1 != rows_fetched)
				{
					zbx_db_err_log(ZBX_DB_TYPE_ORACLE, ERR_Z3006, errcode, errbuf, NULL);
					return NULL;
				}
		}
	}

	for (i = 0; i < result->ncolumn; i++)
	{
		ub4		alloc, amount;
		ub1		csfrm;
		sword	rc2;

		if (NULL == result->clobs[i])
			continue;

		if (OCI_SUCCESS != (rc2 = OCILobGetLength(((struct zbx_db_oracle *)conn->connection)->svchp, ((struct zbx_db_oracle *)conn->connection)->errhp, result->clobs[i], &amount)))
		{
			/* If the LOB is NULL, the length is undefined. */
			/* In this case the function returns OCI_INVALID_HANDLE. */
			if (OCI_INVALID_HANDLE != rc2)
			{
				zbx_db_err_log(ZBX_DB_TYPE_ORACLE, ERR_Z3006, rc2, zbx_db_oci_error(conn, rc2, NULL), NULL);
				return NULL;
			}
			else
				amount = 0;
		}
		else if (OCI_SUCCESS != (rc2 = OCILobCharSetForm(((struct zbx_db_oracle *)conn->connection)->envhp, ((struct zbx_db_oracle *)conn->connection)->errhp, result->clobs[i], &csfrm)))
		{
			zbx_db_err_log(ZBX_DB_TYPE_ORACLE, ERR_Z3006, rc2, zbx_db_oci_error(conn, rc2, NULL), NULL);
			return NULL;
		}

		if (result->values_alloc[i] < (alloc = amount * ZBX_MAX_BYTES_IN_UTF8_CHAR + 1))
		{
			result->values_alloc[i] = alloc;
			result->values[i] = (char *)zbx_realloc(result->values[i], result->values_alloc[i]);
		}

		if (OCI_SUCCESS == rc2)
		{
			if (OCI_SUCCESS != (rc2 = OCILobRead(((struct zbx_db_oracle *)conn->connection)->svchp, ((struct zbx_db_oracle *)conn->connection)->errhp, result->clobs[i], &amount,
				(ub4)1, (dvoid *)result->values[i], (ub4)(result->values_alloc[i] - 1),
				(dvoid *)NULL, (OCICallbackLobRead)NULL, (ub2)0, csfrm)))
			{
				zbx_db_err_log(ZBX_DB_TYPE_ORACLE, ERR_Z3006, rc2, zbx_db_oci_error(conn, rc2, NULL), NULL);
				return NULL;
			}
		}

		result->values[i][amount] = '\0';
	}

	return result->values;
}

/**
 * zbx_db_execute_query_oracle
 * Execute a query on a oracle connection, set the result structure with the returned values
 * Should not be executed by the user because all parameters are supposed to be correct
 * if result is NULL, the query is executed but no value will be returned
 * Return ZBX_DB_OK on success
 */
int zbx_db_execute_query_oracle(const struct zbx_db_connection *conn, struct zbx_db_result *o_result, const char *fmt, va_list args) {
	struct zbx_db_data		*data, *cur_row = NULL;
	struct zbx_db_fields	*db_fields, *cur_field = NULL;
	ORA_DB_RESULT			result = NULL;
	char					**row;
	unsigned int			i, cur_row_num = 0;
	int						res, f_res;

	if (pthread_mutex_lock(&(((struct zbx_db_oracle *)conn->connection)->lock)))
	{
		return ZBX_DB_ERROR_QUERY;
	}

	if (NULL != o_result)
	{
		result = zbx_db_select_oracle(conn, fmt, args);

		if (NULL == result)
		{
			pthread_mutex_unlock(&(((struct zbx_db_oracle *)conn->connection)->lock));

			return ZBX_DB_ERROR_QUERY;
		}

		o_result->nb_rows = 0;
		o_result->nb_columns = result->ncolumn;
		o_result->fields = NULL;
		o_result->data = NULL;

		while (NULL != (row = zbx_db_fetch_oracle(conn, result)))
		{
			cur_row = NULL;
			cur_field = NULL;

			for (i = 0; i < result->ncolumn; i++)
			{
				data = zbx_db_new_data_text(row[i], result->values_alloc[i]);

				db_fields = zbx_db_fields_value((char *)result->colname[i], result->colname_alloc[i]);
				f_res = zbx_db_row_add_fields(&cur_field, db_fields, i);
				zbx_db_clean_fields_full(db_fields);

				if (ZBX_DB_OK != f_res)
				{
					zbx_db_free_result_oracle(result);
					pthread_mutex_unlock(&(((struct zbx_db_oracle *)conn->connection)->lock));
					return f_res;
				}

				res = zbx_db_row_add_data(&cur_row, data, i);
				zbx_db_clean_data_full(data);

				if (ZBX_DB_OK != res)
				{
					zbx_db_free_result_oracle(result);
					pthread_mutex_unlock(&(((struct zbx_db_oracle *)conn->connection)->lock));
					return res;
				}
			}

			f_res = zbx_db_result_add_fields(o_result, cur_field);

			if (ZBX_DB_OK != f_res)
			{
				zbx_db_free_result_oracle(result);
				pthread_mutex_unlock(&(((struct zbx_db_oracle *)conn->connection)->lock));
				return f_res;
			}

			res = zbx_db_result_add_row(o_result, cur_row, cur_row_num);
			cur_row_num++;

			if (ZBX_DB_OK != res)
			{
				zbx_db_free_result_oracle(result);
				pthread_mutex_unlock(&(((struct zbx_db_oracle *)conn->connection)->lock));
				return res;
			}
		}

		zbx_db_free_result_oracle(result);
	}

	pthread_mutex_unlock(&(((struct zbx_db_oracle *)conn->connection)->lock));

	return ZBX_DB_OK;
}
/**
 * zbx_db_connect_oracle
 * Opens a database connection to a Oracle database
 * Return pointer to a struct zbx_db_connection * on sucess, NULL on error
 */
struct zbx_db_connection *zbx_db_connect_oracle(const char *host, const char *user, const char *passwd, const char *dbname,
												const unsigned int port, unsigned int mode)
{
	struct zbx_db_connection	*conn = NULL;
	char						*connect = NULL;
	int							ret = ZBX_DB_OK;
	sword						err = OCI_SUCCESS;
	static ub2					csid = 0;
	pthread_mutexattr_t			mutexattr;

	if ('\0' != *host)
	{
		/*
			EZconnect syntax connection string:
			[//]host[/service name]
			or
			[//]host[:port][/service name]
		*/
		connect = zbx_strdcatf(connect, "//%s", host);
		if (0 != port)
			connect = zbx_strdcatf(connect, ":%d", port);
		if (NULL != dbname && '\0' != *dbname)
			connect = zbx_strdcatf(connect, "/%s", dbname);
	}
	else if (NULL != dbname && '\0' != *dbname)
	{
		/* Use tnsname */
		connect = _strdup(dbname);
	}
	else
		return NULL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(): Oracle connection string: %s", __func__, connect);

	if (NULL != connect)
	{
		conn = (struct zbx_db_connection *)malloc(sizeof(struct zbx_db_connection));

		if (NULL == conn)
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for conn", __func__);
			return NULL;
		}

		conn->type = ZBX_DB_TYPE_ORACLE;
		conn->connection = malloc(sizeof(struct zbx_db_oracle));

		if (NULL == conn->connection)
		{
			zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error allocating memory for conn->connection", __func__);
			free(conn);
			return NULL;
		}

		while (ZBX_DB_OK == ret)
		{
			/* Create environment */
			if (OCI_SUCCESS == (err = OCIEnvNlsCreate((OCIEnv **)&((struct zbx_db_oracle *)conn->connection)->envhp, (ub4)OCI_DEFAULT, (dvoid *)0,
				(dvoid * (*)(dvoid *, size_t))0, (dvoid * (*)(dvoid *, dvoid *, size_t))0,
				(void(*)(dvoid *, dvoid *))0, (size_t)0, (dvoid **)0, csid, csid)))
			{
				if (0 != csid)
					break;	/* environment with UTF8 character set successfully created */

				/* Try to find out the id of UTF8 character set */
				if (0 == (csid = OCINlsCharSetNameToId(((struct zbx_db_oracle *)conn->connection)->envhp, (const oratext *)"UTF8")))
				{
					zabbix_log(LOG_LEVEL_CRIT, "In %s(): Cannot find out the ID of \"UTF8\" character set."
						" Relying on current \"NLS_LANG\" settings.", __func__);
					break;	/* use default environment with character set derived from NLS_LANG */
				}

				/* Get rid of this environment to create a better one on the next iteration */
				OCIHandleFree((dvoid *)((struct zbx_db_oracle *)conn->connection)->envhp, OCI_HTYPE_ENV);
				((struct zbx_db_oracle *)conn->connection)->envhp = NULL;
			}
			else
			{
				zabbix_log(LOG_LEVEL_CRIT, "In %s(): Error create Oracle environment", __func__);
				zbx_db_err_log(ZBX_DB_TYPE_ORACLE, ERR_Z3001, err, zbx_db_oci_error(conn, err, NULL), connect);
				ret = ZBX_DB_ERROR_CONNECTION;
			}
		}

		if (ZBX_DB_OK == ret)
		{
			zabbix_log(LOG_LEVEL_TRACE, "In %s(): Connect to Oracle database: Stage 1 (Create environment) - done", __func__);

			/* Allocate an error handle */
			(void)OCIHandleAlloc((dvoid *)((struct zbx_db_oracle *)conn->connection)->envhp,
				(dvoid **)&((struct zbx_db_oracle *)conn->connection)->errhp, OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0);

			/* Allocate server contexts */
			(void)OCIHandleAlloc((dvoid *)((struct zbx_db_oracle *)conn->connection)->envhp, (void *)&((struct zbx_db_oracle *)conn->connection)->srvhp,
				OCI_HTYPE_SERVER, (size_t)0, (dvoid **)0);

			/* Allocate service contexts */
			(void)OCIHandleAlloc((dvoid *)((struct zbx_db_oracle *)conn->connection)->envhp, (void *)&((struct zbx_db_oracle *)conn->connection)->svchp,
				OCI_HTYPE_SVCCTX, (size_t)0, (dvoid **)0);

			/* Create a server context */
			(void)OCIServerAttach(((struct zbx_db_oracle *)conn->connection)->srvhp, ((struct zbx_db_oracle *)conn->connection)->errhp,
				(text *)connect, (ub4)strlen(connect), 0);

			/* Set attribute server context in the service context */
			(void)OCIAttrSet(((struct zbx_db_oracle *)conn->connection)->svchp, OCI_HTYPE_SVCCTX, ((struct zbx_db_oracle *)conn->connection)->srvhp,
				(ub4)0, OCI_ATTR_SERVER, ((struct zbx_db_oracle *)conn->connection)->errhp);

			/* Allocate session handle */
			(void)OCIHandleAlloc((dvoid *)((struct zbx_db_oracle *)conn->connection)->envhp,
				(dvoid **)&((struct zbx_db_oracle *)conn->connection)->authp, OCI_HTYPE_AUTHINFO, (size_t)0, (dvoid **)NULL);

			/* Setup username and password */
			OCIAttrSet(((struct zbx_db_oracle *)conn->connection)->authp, OCI_HTYPE_SESSION, (text *)user, (ub4)(NULL != user ? strlen(user) : 0),
				OCI_ATTR_USERNAME, ((struct zbx_db_oracle *)conn->connection)->errhp);

			OCIAttrSet(((struct zbx_db_oracle *)conn->connection)->authp, OCI_HTYPE_SESSION, (text *)passwd, (ub4)(NULL != passwd ? strlen(passwd) : 0),
				OCI_ATTR_PASSWORD, ((struct zbx_db_oracle *)conn->connection)->errhp);

			// mode is OCI_DEFAULT or OCI_SYSDBA or OCI_SYSOPER or OCI_SYSASM or OCI_SYSDGD
			err = OCISessionBegin(((struct zbx_db_oracle *)conn->connection)->svchp, ((struct zbx_db_oracle *)conn->connection)->errhp,
				((struct zbx_db_oracle *)conn->connection)->authp, OCI_CRED_RDBMS, (ub4)mode);

			if (OCI_SUCCESS != err)
			{
				zbx_db_err_log(ZBX_DB_TYPE_ORACLE, ERR_Z3001, err, zbx_db_oci_error(conn, err, NULL), connect);
				ret = ZBX_DB_ERROR_CONNECTION;
			}
		}

		if (ZBX_DB_OK == ret)
		{
			(void)OCIAttrSet(((struct zbx_db_oracle *)conn->connection)->svchp, (ub4)OCI_HTYPE_SVCCTX, ((struct zbx_db_oracle *)conn->connection)->authp,
				(ub4)0,	(ub4)OCI_ATTR_SESSION, ((struct zbx_db_oracle *)conn->connection)->errhp);

			zabbix_log(LOG_LEVEL_TRACE, "In %s(): Connect to Oracle database: Stage 2 (Get the session) - done", __func__);

			/* Initialize statement handle */
			err = OCIHandleAlloc((dvoid *)((struct zbx_db_oracle *)conn->connection)->envhp, (dvoid **)&((struct zbx_db_oracle *)conn->connection)->stmthp, OCI_HTYPE_STMT,
				(size_t)0, (dvoid **)0);

			if (OCI_SUCCESS != err)
			{
				zbx_db_err_log(ZBX_DB_TYPE_ORACLE, ERR_Z3001, err, zbx_db_oci_error(conn, err, NULL), connect);
				ret = ZBX_DB_ERROR_CONNECTION;
			}
		}

		if (ZBX_DB_OK == ret)
		{
			zabbix_log(LOG_LEVEL_TRACE, "In %s(): Connect to Oracle database: Stage 3 (Initialize statement handle) - done", __func__);

			/* Initialize MUTEX for connection */
			pthread_mutexattr_init(&mutexattr);
			pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);

			if (0 != pthread_mutex_init(&(((struct zbx_db_oracle *)conn->connection)->lock), &mutexattr))
			{
				zabbix_log(LOG_LEVEL_CRIT, "In %s(): Impossible to initialize Mutex Lock for Oracle connection", __func__);
			}

			pthread_mutexattr_destroy(&mutexattr);
		}
		else
		{
			free(conn);
			conn = NULL;
		}
	}

	return conn;
}

/**
 * Close connection to Oracle database
 */
void zbx_db_close_oracle(struct zbx_db_connection *conn)
{
	sword	err = OCI_SUCCESS;

	err = OCISessionEnd(((struct zbx_db_oracle *)conn->connection)->svchp, ((struct zbx_db_oracle *)conn->connection)->errhp,
			((struct zbx_db_oracle *)conn->connection)->authp, (ub4)OCI_DEFAULT);

	if (OCI_SUCCESS != err)
	{
		zbx_db_err_log(ZBX_DB_TYPE_ORACLE, ERR_Z3004, err, zbx_db_oci_error(conn, err, NULL), NULL);
	}

	err = OCIServerDetach(((struct zbx_db_oracle *)conn->connection)->srvhp, ((struct zbx_db_oracle *)conn->connection)->errhp, OCI_DEFAULT);

	if (OCI_SUCCESS != err)
	{
		zbx_db_err_log(ZBX_DB_TYPE_ORACLE, ERR_Z3004, err, zbx_db_oci_error(conn, err, NULL), NULL);
	}

	/* Deallocate statement handle */
	if (NULL != ((struct zbx_db_oracle *)conn->connection)->stmthp)
	{
		OCIHandleFree((dvoid *)((struct zbx_db_oracle *)conn->connection)->stmthp, OCI_HTYPE_STMT);
		((struct zbx_db_oracle *)conn->connection)->stmthp = NULL;
	}

	/* Deallocate error handle */
	if (NULL != ((struct zbx_db_oracle *)conn->connection)->errhp)
	{
		OCIHandleFree(((struct zbx_db_oracle *)conn->connection)->errhp, OCI_HTYPE_ERROR);
		((struct zbx_db_oracle *)conn->connection)->errhp = NULL;
	}

	/* Deallocate server handle */
	if (NULL != ((struct zbx_db_oracle *)conn->connection)->srvhp)
	{
		OCIHandleFree(((struct zbx_db_oracle *)conn->connection)->srvhp, OCI_HTYPE_SERVER);
		((struct zbx_db_oracle *)conn->connection)->srvhp = NULL;
	}

	/* Deallocate service handle */
	if (NULL != ((struct zbx_db_oracle *)conn->connection)->svchp)
	{
		OCIHandleFree(((struct zbx_db_oracle *)conn->connection)->svchp, OCI_HTYPE_SVCCTX);
		((struct zbx_db_oracle *)conn->connection)->svchp = NULL;
	}

	/* Deallocate session handle */
	if (NULL != ((struct zbx_db_oracle *)conn->connection)->authp)
	{
		OCIHandleFree(((struct zbx_db_oracle *)conn->connection)->authp, OCI_HTYPE_SESSION);
		((struct zbx_db_oracle *)conn->connection)->authp = NULL;
	}

	if (NULL != ((struct zbx_db_oracle *)conn->connection)->envhp)
	{
		/* delete the environment handle, which deallocates all other handles associated with it */
		OCIHandleFree((dvoid *)((struct zbx_db_oracle *)conn->connection)->envhp, OCI_HTYPE_ENV);
		((struct zbx_db_oracle *)conn->connection)->envhp = NULL;
	}

	pthread_mutex_destroy(&((struct zbx_db_oracle *)conn->connection)->lock);
}

#endif