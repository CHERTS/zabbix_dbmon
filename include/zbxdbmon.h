#ifndef ZABBIX_ZBXDB_H
#define ZABBIX_ZBXDB_H

#include "common.h"
#include <pthread.h>

#ifdef HAVE_MYSQL
#define ZBX_DB_TYPE_MYSQL 1
#endif

#ifdef HAVE_POSTGRESQL
#define ZBX_DB_TYPE_POSTGRESQL 2
#endif

#ifdef HAVE_ORACLE
#define ZBX_DB_TYPE_ORACLE 3
#endif

#ifdef HAVE_MSSQL
#define ZBX_DB_TYPE_MSSQL 4
#endif

// function to determine if a string is null or empty
#define strisnull(c)	(NULL == c || '\0' == *c)

#if defined(HAVE_DBMON)
static char *DB_TYPE[] = {
	"MySQL",
	"PostgreSQL",
	"Oracle",
	"MSSQL"
};
#endif

#define ZBX_COL_TYPE_INT    0
#define ZBX_COL_TYPE_DOUBLE 1
#define ZBX_COL_TYPE_TEXT   2
#define ZBX_COL_TYPE_DATE   3
#define ZBX_COL_TYPE_BLOB   4
#define ZBX_COL_TYPE_BOOL   5
#define ZBX_COL_TYPE_NULL   5

#define ZBX_DB_OK				0	/* No error */
#define ZBX_DB_ERROR			1	/* Generic error */
#define ZBX_DB_ERROR_PARAMS		2	/* Error in input parameters */
#define ZBX_DB_ERROR_CONNECTION	3	/* Error in database connection */
#define ZBX_DB_ERROR_QUERY		4	/* Error executing query */
#define ZBX_DB_ERROR_MEMORY		99	/* Error allocating memory */

/**
 * sql value integer type
 */
struct zbx_db_type_int {
	long long int value;
};

/**
 * sql value double type
 */
struct zbx_db_type_double {
	double value;
};

/**
 * sql value date/time type
 */
struct zbx_db_type_datetime {
	struct tm value;
};

/**
 * sql value string type
 */
struct zbx_db_type_text {
	size_t length;
	char *value;
};

/**
 * sql value blob type
 */
struct zbx_db_type_blob {
	size_t length;
	void *value;
};

/**
 * handle container
 */
struct zbx_db_connection {
	int type;
	void *connection;
};

/**
 * sql data container
 */
struct zbx_db_data {
	int    type;
	void *t_data;
};

/**
 * sql fields container
 */
struct zbx_db_fields {
	void *t_data;
};

/**
 * sql result structure
 */
struct zbx_db_result {
	unsigned int      nb_rows;
	unsigned int      nb_columns;
	struct zbx_db_fields **fields;
	struct zbx_db_data **data;
};

#if defined(HAVE_DBMON)
typedef void *(*zbx_dbs_malloc_t)(size_t);
typedef void *(*zbx_dbs_realloc_t)(void *, size_t);
typedef void(*zbx_dbs_free_t)(void *);

static zbx_dbs_malloc_t do_malloc = malloc;
static zbx_dbs_realloc_t do_realloc = realloc;
static zbx_dbs_free_t do_free = free;

void *zbx_db_malloc(size_t size);
void *zbx_db_realloc(void *ptr, size_t size);
void zbx_db_free(void *ptr);
int zbx_db_strcmp(const char *p1, const char *p2);
int zbx_db_strncmp(const char *p1, const char *p2, size_t n);
size_t zbx_db_strlen(const char *s);
int zbx_db_strcasecmp(const char *p1, const char *p2);
void zbx_db_err_log(int db_type, zbx_err_codes_t zbx_errno, int db_errno, const char *db_error, const char *context);

int zbx_db_clean_data(struct zbx_db_data *e_data);
int zbx_db_clean_data_full(struct zbx_db_data *e_data);
int zbx_db_clean_fields(struct zbx_db_fields *e_data);
int zbx_db_clean_fields_full(struct zbx_db_fields *fields);
struct zbx_db_data *zbx_db_new_data_int(const long long int e_value);
struct zbx_db_data *zbx_db_new_data_null();
struct zbx_db_data *zbx_db_new_data_double(const double e_value);
struct zbx_db_data *zbx_db_new_data_datetime(const struct tm *datetime);
struct zbx_db_data *zbx_db_new_data_blob(const void *e_value, const size_t e_length);
struct zbx_db_data *zbx_db_new_data_text(const char *e_value, const size_t e_length);
struct zbx_db_fields *zbx_db_fields_value(const char *e_value, const size_t e_length);
int zbx_db_row_add_fields(struct zbx_db_fields **row, struct zbx_db_fields *e_data, int cols);
int zbx_db_row_add_data(struct zbx_db_data **row, struct zbx_db_data *e_data, int cols);
int zbx_db_result_add_row(struct zbx_db_result *result, struct zbx_db_data *row, int rows);
int zbx_db_result_add_fields(struct zbx_db_result *result, struct zbx_db_fields *field);
int zbx_db_query_select(const struct zbx_db_connection *conn, struct zbx_db_result *e_result, const char *fmt, ...) __zbx_attr_format_printf(3, 4);
unsigned long zbx_db_version(const struct zbx_db_connection *conn);
int zbx_db_close_db(struct zbx_db_connection *conn);
int zbx_db_clean_connection(struct zbx_db_connection *conn);
int zbx_db_clean_result(struct zbx_db_result *e_result);
#endif

#if defined(HAVE_MYSQL)
int zbx_db_execute_query_mysql(const struct zbx_db_connection *conn, struct zbx_db_result *m_result, const char *query);
unsigned long zbx_db_get_version_mysql(const struct zbx_db_connection *conn);
struct zbx_db_connection *zbx_db_connect_mysql(const char *host, const char *user, const char *passwd, const char *dbname, const unsigned int port, const char *dbsocket, const unsigned int timeout);
void zbx_db_close_mysql(struct zbx_db_connection *conn);
#endif

#if defined(HAVE_POSTGRESQL)
int zbx_db_execute_query_pgsql(const struct zbx_db_connection *conn, struct zbx_db_result *p_result, const char *query);
int zbx_db_get_version_pgsql(const struct zbx_db_connection *conn);
struct zbx_db_connection *zbx_db_connect_pgsql(const char *conn_string);
void zbx_db_close_pgsql(struct zbx_db_connection *conn);
#endif

#if defined(HAVE_ORACLE)
#define ZBX_DB_OCI_DEFAULT		0x00000000
#define ZBX_DB_OCI_SYSDBA		0x00000002
#define ZBX_DB_OCI_SYSOPER		0x00000004
#define ZBX_DB_OCI_SYSASM		0x00008000
#define ZBX_DB_OCI_SYSDGD		0x00040000
#define ZBX_DB_OCI_PRELIM_AUTH	0x00000008

#define MAX_ZBX_DB_ORACLE_INSTANCE_LEN 8

unsigned int zbx_db_get_oracle_mode(int ora_mode);
int zbx_db_execute_query_oracle(const struct zbx_db_connection *conn, struct zbx_db_result *o_result, const char *fmt, va_list args);
struct zbx_db_connection *zbx_db_connect_oracle(const char *conn_string, const char *username, const char *userpasswd, unsigned int mode);
void zbx_db_close_oracle(struct zbx_db_connection * conn);
int	zbx_check_oracle_instance_name(const char *instance, char **error);
char *zbx_check_oracle_conn_string(char *conn_string);
#endif

#endif
