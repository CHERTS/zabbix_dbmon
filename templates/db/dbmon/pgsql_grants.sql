Create user zabbixmon
---------------------

Connect to database postgres from user postgres:
# psql -U postgres -d postgres -w

For PostgreSQL version 10 and above:

CREATE USER zabbixmon WITH PASSWORD 'zabbixmon' INHERIT;
GRANT pg_monitor TO zabbixmon;

For PostgreSQL version 9.6 and below:
CREATE USER zabbixmon WITH PASSWORD 'zabbixmon';
GRANT SELECT ON pg_stat_database TO zabbixmon;

-- To collect WAL metrics.
GRANT EXECUTE ON FUNCTION pg_catalog.pg_ls_dir(text) TO zabbixmon;
GRANT EXECUTE ON FUNCTION pg_catalog.pg_stat_file(text) TO zabbixmon;

Edit pg_hba.conf to allow connections from Zabbix agent (add these lines to the beginning of the file):

Add rows (for example):
host all zabbixmon 127.0.0.1/32 trust
host all zabbixmon 0.0.0.0/0 md5
host all zabbixmon ::0/0 md5

Reload pg settings:
su - postgres -c "psql -c 'SELECT pg_reload_conf();'"

If you need to monitor the remote server then create .pgpass file in Zabbix agent home directory /var/lib/zabbix/ and 
add the connection details with the instance, port, database, user and password information in the below
format https://www.postgresql.org/docs/current/libpq-pgpass.html

Example 1:
<REMOTE_HOST1>:5432:postgres:zabbixmon:<PASSWORD>
<REMOTE_HOST2>:5432:postgres:zabbixmon:<PASSWORD>
...
<REMOTE_HOSTN>:5432:postgres:zabbixmon:<PASSWORD>

Example 2:
*:5432:postgres:zabbixmon:zabbixmon

Set file permition and owner:
chown zabbix:zabbix /var/lib/zabbix/.pgpass
chmod 600 /var/lib/zabbix/.pgpass

Revoke grants and drop user zabbixmon
-------------------------------------

# psql -U postgres -d postgres -w

1) Check Ownership:
postgres=# select * from pg_shdepend where refclassid = 'pg_authid'::regclass and refobjid = (select oid from pg_roles where rolname = 'zabbixmon');

2) Reassigning Ownership:
postgres=# REASSIGN OWNED BY zabbixmon TO postgres;
or
postgres=# DROP OWNED BY zabbixmon;

3) Executing REVOKE PRIVILEGES:
REVOKE ALL PRIVILEGES ON ALL TABLES IN SCHEMA public FROM zabbixmon;
REVOKE ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public FROM zabbixmon;
REVOKE ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA public FROM zabbixmon;

4) Dropping the user:
postgres=# DROP USER zabbixmon;

PS: You might not need to execute both Step 2 and 3, just one of the two steps might be usually enough.
