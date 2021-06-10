
# Template DB MongoDB node by Zabbix Agent 2

## Overview

For Zabbix version: 5.0 and higher  
The template to monitor single MongoDB server by Zabbix that work without any external scripts.
Most of the metrics are collected in one go, thanks to Zabbix bulk data collection.

`Template DB MongoDB node by Zabbix Agent 2` — collects metrics by polling zabbix-agent2.



This template was tested on:

- MongoDB, version 4.0.21, 4.4.3

## Setup

> See [Zabbix template operation](https://www.zabbix.com/documentation/5.0/manual/config/templates_out_of_the_box/zabbix_agent2) for basic instructions.

1. Setup and configure zabbix-agent2 compiled with the MongoDB monitoring plugin.
2. Set the {$MONGODB.CONNSTRING} such as <protocol(host:port)> or named session.
3. Set the user name and password in host macros ({$MONGODB.USER}, {$MONGODB.PASSWORD}) if you want to override parameters from the Zabbix agent configuration file.

**Note**, depending on the number of DBs and collections discovery operation may be expensive. Use filters with macros {$MONGODB.LLD.FILTER.DB.MATCHES}, {$MONGODB.LLD.FILTER.DB.NOT_MATCHES}, {$MONGODB.LLD.FILTER.COLLECTION.MATCHES}, {$MONGODB.LLD.FILTER.COLLECTION.NOT_MATCHES}.

Test availability: `zabbix_get -s mongodb.node -k 'mongodb.ping["{$MONGODB.CONNSTRING}","{$MONGODB.USER}","{$MONGODB.PASSWORD}"]"`


## Zabbix configuration

No specific Zabbix configuration is required.

### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$MONGODB.CONNS.PCT.USED.MAX.WARN} |<p>Maximum percentage of used connections</p> |`80` |
|{$MONGODB.CONNSTRING} |<p>Connection string in the URI format (password is not used). This param overwrites a value configured in the "Server" option of the configuration file (if it's set), otherwise, the plugin's default value is used: "tcp://localhost:27017"</p> |`tcp://localhost:27017` |
|{$MONGODB.CURSOR.OPEN.MAX.WARN} |<p>Maximum number of open cursors</p> |`10000` |
|{$MONGODB.CURSOR.TIMEOUT.MAX.WARN} |<p>Maximum number of cursors timing out per second</p> |`1` |
|{$MONGODB.LLD.FILTER.COLLECTION.MATCHES} |<p>Filter of discoverable collections</p> |`.*` |
|{$MONGODB.LLD.FILTER.COLLECTION.NOT_MATCHES} |<p>Filter to exclude discovered collections</p> |`CHANGE_IF_NEEDED` |
|{$MONGODB.LLD.FILTER.DB.MATCHES} |<p>Filter of discoverable databases</p> |`.*` |
|{$MONGODB.LLD.FILTER.DB.NOT_MATCHES} |<p>Filter to exclude discovered databases</p> |`(admin|config|local)` |
|{$MONGODB.PASSWORD} |<p>MongoDB user password</p> |`` |
|{$MONGODB.REPL.LAG.MAX.WARN} |<p>Maximum replication lag in seconds</p> |`10s` |
|{$MONGODB.USER} |<p>MongoDB username</p> |`` |
|{$MONGODB.WIRED_TIGER.TICKETS.AVAILABLE.MIN.WARN} |<p>Minimum number of available WiredTiger read or write tickets remaining</p> |`5` |

## Template links

There are no template links in this template.

## Discovery rules

|Name|Description|Type|Key and additional info|
|----|-----------|----|----|
|Database discovery |<p>Collect database metrics.</p><p>Note, depending on the number of DBs this discovery operation may be expensive. Use filters with macros {$MONGODB.LLD.FILTER.DB.MATCHES}, {$MONGODB.LLD.FILTER.DB.NOT_MATCHES}.</p> |ZABBIX_PASSIVE |mongodb.db.discovery["{$MONGODB.CONNSTRING}","{$MONGODB.USER}","{$MONGODB.PASSWORD}"]<p>**Filter**:</p>AND <p>- A: {#DBNAME} MATCHES_REGEX `{$MONGODB.LLD.FILTER.DB.MATCHES}`</p><p>- B: {#DBNAME} NOT_MATCHES_REGEX `{$MONGODB.LLD.FILTER.DB.NOT_MATCHES}`</p> |
|Collection discovery |<p>Collect collections metrics.</p><p>Note, depending on the number of DBs and collections this discovery operation may be expensive. Use filters with macros {$MONGODB.LLD.FILTER.DB.MATCHES}, {$MONGODB.LLD.FILTER.DB.NOT_MATCHES}, {$MONGODB.LLD.FILTER.COLLECTION.MATCHES}, {$MONGODB.LLD.FILTER.COLLECTION.NOT_MATCHES}.</p> |ZABBIX_PASSIVE |mongodb.collections.discovery["{$MONGODB.CONNSTRING}","{$MONGODB.USER}","{$MONGODB.PASSWORD}"]<p>**Filter**:</p>AND <p>- A: {#DBNAME} MATCHES_REGEX `{$MONGODB.LLD.FILTER.DB.MATCHES}`</p><p>- B: {#DBNAME} NOT_MATCHES_REGEX `{$MONGODB.LLD.FILTER.DB.NOT_MATCHES}`</p><p>- C: {#COLLECTION} MATCHES_REGEX `{$MONGODB.LLD.FILTER.COLLECTION.MATCHES}`</p><p>- D: {#COLLECTION} NOT_MATCHES_REGEX `{$MONGODB.LLD.FILTER.COLLECTION.NOT_MATCHES}`</p> |
|Replication discovery |<p>Collect metrics by Zabbix agent if it exists</p> |DEPENDENT |mongodb.rs.discovery<p>**Preprocessing**:</p><p>- JAVASCRIPT: `Text is too long. Please see the template.`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|WiredTiger metrics |<p>Collect metrics of WiredTiger Storage Engine if it exists</p> |DEPENDENT |mongodb.wired_tiger.discovery<p>**Preprocessing**:</p><p>- JAVASCRIPT: `return JSON.stringify(JSON.parse(value).wiredTiger   ? [{'{#SINGLETON}': ''}] : []);`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |

## Items collected

|Group|Name|Description|Type|Key and additional info|
|-----|----|-----------|----|---------------------|
|MongoDB |MongoDB: Ping |<p>Test if a connection is alive or not.</p> |ZABBIX_PASSIVE |mongodb.ping["{$MONGODB.CONNSTRING}","{$MONGODB.USER}","{$MONGODB.PASSWORD}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `30m`</p> |
|MongoDB |MongoDB: MongoDB version |<p>Version of the MongoDB server.</p> |DEPENDENT |mongodb.version<p>**Preprocessing**:</p><p>- JSONPATH: `$.version`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `3h`</p> |
|MongoDB |MongoDB: Uptime |<p>Number of seconds that the mongod process has been active.</p> |DEPENDENT |mongodb.uptime<p>**Preprocessing**:</p><p>- JSONPATH: `$.uptime`</p> |
|MongoDB |MongoDB: Asserts: message, rate |<p>The number of message assertions raised per second.</p><p>Check the log file for more information about these messages.</p> |DEPENDENT |mongodb.asserts.msg.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.asserts.msg`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Asserts: user, rate |<p>The number of “user asserts” that have occurred per second.</p><p>These are errors that user may generate, such as out of disk space or duplicate key.</p> |DEPENDENT |mongodb.asserts.user.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.asserts.user`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Asserts: warning, rate |<p>The number of warnings raised per second.</p> |DEPENDENT |mongodb.asserts.warning.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.asserts.warning`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Asserts: regular, rate |<p>The number of regular assertions raised per second.</p><p>Check the log file for more information about these messages.</p> |DEPENDENT |mongodb.asserts.regular.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.asserts.regular`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Asserts: rollovers, rate |<p>Number of times that the rollover counters roll over per second.</p><p>The counters rollover to zero every 2^30 assertions.</p> |DEPENDENT |mongodb.asserts.rollovers.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.asserts.rollovers`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Active clients: writers |<p>The number of active client connections performing write operations.</p> |DEPENDENT |mongodb.active_clients.writers<p>**Preprocessing**:</p><p>- JSONPATH: `$.globalLock.activeClients.writers`</p> |
|MongoDB |MongoDB: Active clients: readers |<p>The number of the active client connections performing read operations.</p> |DEPENDENT |mongodb.active_clients.readers<p>**Preprocessing**:</p><p>- JSONPATH: `$.globalLock.activeClients.readers`</p> |
|MongoDB |MongoDB: Active clients: total |<p>The total number of internal client connections to the database including system threads as well as queued readers and writers.</p> |DEPENDENT |mongodb.active_clients.total<p>**Preprocessing**:</p><p>- JSONPATH: `$.globalLock.activeClients.total`</p> |
|MongoDB |MongoDB: Current queue: writers |<p>The number of operations that are currently queued and waiting for the write lock. </p><p> A consistently small write-queue, particularly of shorter operations, is no cause for concern.</p> |DEPENDENT |mongodb.current_queue.writers<p>**Preprocessing**:</p><p>- JSONPATH: `$.globalLock.currentQueue.writers`</p> |
|MongoDB |MongoDB: Current queue: readers |<p>The number of operations that are currently queued and waiting for the read lock.</p><p>A consistently small read-queue, particularly of shorter operations, should cause no concern.</p> |DEPENDENT |mongodb.current_queue.readers<p>**Preprocessing**:</p><p>- JSONPATH: `$.globalLock.currentQueue.readers`</p> |
|MongoDB |MongoDB: Current queue: total |<p>The total number of operations queued waiting for the lock.</p> |DEPENDENT |mongodb.current_queue.total<p>**Preprocessing**:</p><p>- JSONPATH: `$.globalLock.currentQueue.total`</p> |
|MongoDB |MongoDB: Operations: command, rate |<p>The number of commands issued to the database the mongod instance per second.</p><p>Counts all commands except the write commands: insert, update, and delete.</p> |DEPENDENT |mongodb.opcounters.command.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.opcounters.command`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Operations: delete, rate |<p>The number of delete operations the mongod instance per second.</p> |DEPENDENT |mongodb.opcounters.delete.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.opcounters.delete`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Operations: update, rate |<p>The number of update operations the mongod instance per second.</p> |DEPENDENT |mongodb.opcounters.update.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.opcounters.update`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Operations: query, rate |<p>The number of queries received the mongod instance per second.</p> |DEPENDENT |mongodb.opcounters.query.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.opcounters.query`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Operations: insert, rate |<p>The number of insert operations received since the mongod instance per second.</p> |DEPENDENT |mongodb.opcounters.insert.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.opcounters.insert`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Operations: getmore, rate |<p>The number of “getmore” operations since the mongod instance per second. This counter can be high even if the query count is low. </p><p>Secondary nodes send getMore operations as part of the replication process.</p> |DEPENDENT |mongodb.opcounters.getmore.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.opcounters.getmore`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Connections, current |<p>The number of incoming connections from clients to the database server.</p><p>This number includes the current shell session</p> |DEPENDENT |mongodb.connections.current<p>**Preprocessing**:</p><p>- JSONPATH: `$.connections.current`</p> |
|MongoDB |MongoDB: New connections, rate |<p>Rate of all incoming connections created to the server.</p> |DEPENDENT |mongodb.connections.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.connections.totalCreated`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Connections, available |<p>The number of unused incoming connections available.</p> |DEPENDENT |mongodb.connections.available<p>**Preprocessing**:</p><p>- JSONPATH: `$.connections.available`</p> |
|MongoDB |MongoDB: Connections, active |<p>The number of active client connections to the server.</p><p>Active client connections refers to client connections that currently have operations in progress.</p><p>Available starting in  4.0.7, 0 for older versions.</p> |DEPENDENT |mongodb.connections.active<p>**Preprocessing**:</p><p>- JSONPATH: `$.connections.active`</p><p>⛔️ON_FAIL: `DISCARD_VALUE -> `</p> |
|MongoDB |MongoDB: Bytes in, rate |<p>The total number of bytes that the server has received over network connections initiated by clients or other mongod/mongos instances per second.</p> |DEPENDENT |mongodb.network.bytes_in.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.network.bytesIn`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Bytes out, rate |<p>The total number of bytes that the server has sent over network connections initiated by clients or other mongod/mongos instances per second.</p> |DEPENDENT |mongodb.network.bytes_out.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.network.bytesOut`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Requests, rate |<p>Number of distinct requests that the server has received per second</p> |DEPENDENT |mongodb.network.numRequests.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.network.numRequests`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Document: deleted, rate |<p>Number of documents deleted per second.</p> |DEPENDENT |mongod.document.deleted.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.document.deleted`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Document: inserted, rate |<p>Number of documents inserted per second.</p> |DEPENDENT |mongod.document.inserted.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.document.inserted`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Document: returned, rate |<p>Number of documents returned by queries per second.</p> |DEPENDENT |mongod.document.returned.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.document.returned`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Document: updated, rate |<p>Number of documents updated per second.</p> |DEPENDENT |mongod.document.updated.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.document.updated`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Cursor: open no timeout |<p>Number of open cursors with the option DBQuery.Option.noTimeout set to prevent timeout after a period of inactivity.</p> |DEPENDENT |mongodb.metrics.cursor.open.no_timeout<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.cursor.open.noTimeout`</p> |
|MongoDB |MongoDB: Cursor: open pinned |<p>Number of pinned open cursors.</p> |DEPENDENT |mongodb.cursor.open.pinned<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.cursor.open.pinned`</p> |
|MongoDB |MongoDB: Cursor: open total |<p>Number of cursors that MongoDB is maintaining for clients.</p> |DEPENDENT |mongodb.cursor.open.total<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.cursor.open.total`</p> |
|MongoDB |MongoDB: Cursor: timed out, rate |<p>Number of cursors that time out, per second.</p> |DEPENDENT |mongodb.cursor.timed_out.rate<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.cursor.timedOut`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Architecture |<p>A number, either 64 or 32, that indicates whether the MongoDB instance is compiled for 64-bit or 32-bit architecture.</p> |DEPENDENT |mongodb.mem.bits<p>**Preprocessing**:</p><p>- JSONPATH: `$.mem.bits`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `3h`</p> |
|MongoDB |MongoDB: Memory: mapped |<p>Amount of mapped memory by the database.</p> |DEPENDENT |mongodb.mem.mapped<p>**Preprocessing**:</p><p>- JSONPATH: `$.mem.mapped`</p><p>⛔️ON_FAIL: `DISCARD_VALUE -> `</p><p>- MULTIPLIER: `1048576`</p> |
|MongoDB |MongoDB: Memory: mapped with journal |<p>The amount of mapped memory, including the memory used for journaling.</p> |DEPENDENT |mongodb.mem.mapped_with_journal<p>**Preprocessing**:</p><p>- JSONPATH: `$.mem.mappedWithJournal`</p><p>⛔️ON_FAIL: `DISCARD_VALUE -> `</p><p>- MULTIPLIER: `1048576`</p> |
|MongoDB |MongoDB: Memory: resident |<p>Amount of memory currently used by the database process.</p> |DEPENDENT |mongodb.mem.resident<p>**Preprocessing**:</p><p>- JSONPATH: `$.mem.resident`</p><p>- MULTIPLIER: `1048576`</p> |
|MongoDB |MongoDB: Memory: virtual |<p>Amount of virtual memory used by the mongod process.</p> |DEPENDENT |mongodb.mem.virtual<p>**Preprocessing**:</p><p>- JSONPATH: `$.mem.virtual`</p><p>- MULTIPLIER: `1048576`</p> |
|MongoDB |MongoDB {#DBNAME}: Objects, avg size |<p>The average size of each document in bytes.</p> |DEPENDENT |mongodb.db.size["{#DBNAME}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.avgObjSize`</p> |
|MongoDB |MongoDB {#DBNAME}: Size, data |<p>Total size of the data held in this database including the padding factor.</p> |DEPENDENT |mongodb.db.data_size["{#DBNAME}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.dataSize`</p> |
|MongoDB |MongoDB {#DBNAME}: Size, file |<p>Total size of the data held in this database including the padding factor (only available with the mmapv1 storage engine).</p> |DEPENDENT |mongodb.db.file_size["{#DBNAME}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.fileSize`</p><p>⛔️ON_FAIL: `DISCARD_VALUE -> `</p> |
|MongoDB |MongoDB {#DBNAME}: Size, index |<p>Total size of all indexes created on this database.</p> |DEPENDENT |mongodb.db.index_size["{#DBNAME}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.indexSize`</p> |
|MongoDB |MongoDB {#DBNAME}: Size, storage |<p>Total amount of space allocated to collections in this database for document storage.</p> |DEPENDENT |mongodb.db.storage_size["{#DBNAME}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.storageSize`</p> |
|MongoDB |MongoDB {#DBNAME}: Collections |<p>Contains a count of the number of collections in that database.</p> |DEPENDENT |mongodb.db.collections["{#DBNAME}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.collections`</p> |
|MongoDB |MongoDB {#DBNAME}: Objects, count |<p>Number of objects (documents) in the database across all collections.</p> |DEPENDENT |mongodb.db.objects["{#DBNAME}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.objects`</p> |
|MongoDB |MongoDB {#DBNAME}: Extents |<p>Contains a count of the number of extents in the database across all collections.</p> |DEPENDENT |mongodb.db.extents["{#DBNAME}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.numExtents`</p><p>⛔️ON_FAIL: `DISCARD_VALUE -> `</p> |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Size |<p>The total size in bytes of the data in the collection plus the size of every indexes on the mongodb.collection.</p> |DEPENDENT |mongodb.collection.size["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.size`</p> |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Objects, avg size |<p>The size of the average object in the collection in bytes.</p> |DEPENDENT |mongodb.collection.avg_obj_size["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.avgObjSize`</p><p>⛔️ON_FAIL: `DISCARD_VALUE -> `</p> |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Objects, count |<p>Total number of objects in the collection.</p> |DEPENDENT |mongodb.collection.count["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.count`</p> |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Capped: max number |<p>Maximum number of documents that may be present in a capped collection.</p> |DEPENDENT |mongodb.collection.max_number["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.max`</p><p>⛔️ON_FAIL: `DISCARD_VALUE -> `</p> |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Capped: max size |<p>Maximum size of a capped collection in bytes.</p> |DEPENDENT |mongodb.collection.max_size["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.maxSize`</p><p>⛔️ON_FAIL: `DISCARD_VALUE -> `</p> |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Storage size |<p>Total storage space allocated to this collection for document storage.</p> |DEPENDENT |mongodb.collection.storage_size["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.storageSize`</p> |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Indexes |<p>Total number of indices on the collection.</p> |DEPENDENT |mongodb.collection.nindexes["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.nindexes`</p> |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Capped |<p>Whether or not the collection is capped.</p> |DEPENDENT |mongodb.collection.capped["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.capped`</p><p>- BOOL_TO_DECIMAL<p>- DISCARD_UNCHANGED_HEARTBEAT: `3h`</p> |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Operations: total, rate |<p>The number of operations per second.</p> |DEPENDENT |mongodb.collection.ops.total.rate["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].total.count`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Read lock, rate |<p>The number of operations per second.</p> |DEPENDENT |mongodb.collection.read_lock.rate["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].readLock.count`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Write lock, rate |<p>The number of operations per second.</p> |DEPENDENT |mongodb.collection.write_lock.rate["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].writeLock.count`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Operations: queries, rate |<p>The number of operations per second.</p> |DEPENDENT |mongodb.collection.ops.queries.rate["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].queries.count`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Operations: getmore, rate |<p>The number of operations per second.</p> |DEPENDENT |mongodb.collection.ops.getmore.rate["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].getmore.count`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Operations: insert, rate |<p>The number of operations per second.</p> |DEPENDENT |mongodb.collection.ops.insert.rate["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].insert.count`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Operations: update, rate |<p>The number of operations per second.</p> |DEPENDENT |mongodb.collection.ops.update.rate["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].update.count`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Operations: remove, rate |<p>The number of operations per second.</p> |DEPENDENT |mongodb.collection.ops.remove.rate["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].remove.count`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Operations: commands, rate |<p>The number of operations per second.</p> |DEPENDENT |mongodb.collection.ops.commands.rate["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].commands.count`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Operations: total, ms/s |<p>Fraction of time (ms/s) the mongod has spent to operations.</p> |DEPENDENT |mongodb.collection.ops.total.ms["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].total.time`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Read lock, ms/s |<p>Fraction of time (ms/s) the mongod has spent to operations.</p> |DEPENDENT |mongodb.collection.read_lock.ms["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].readLock.time`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Write lock, ms/s |<p>Fraction of time (ms/s) the mongod has spent to operations.</p> |DEPENDENT |mongodb.collection.write_lock.ms["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].writeLock.time`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Operations: queries, ms/s |<p>Fraction of time (ms/s) the mongod has spent to operations.</p> |DEPENDENT |mongodb.collection.ops.queries.ms["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].queries.time`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Operations: getmore, ms/s |<p>Fraction of time (ms/s) the mongod has spent to operations.</p> |DEPENDENT |mongodb.collection.ops.getmore.ms["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].getmore.time`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Operations: insert, ms/s |<p>Fraction of time (ms/s) the mongod has spent to operations.</p> |DEPENDENT |mongodb.collection.ops.insert.ms["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].insert.time`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Operations: update, ms/s |<p>Fraction of time (ms/s) the mongod has spent to operations.</p> |DEPENDENT |mongodb.collection.ops.update.ms["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].update.time`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Operations: remove, ms/s |<p>Fraction of time (ms/s) the mongod has spent to operations.</p> |DEPENDENT |mongodb.collection.ops.remove.ms["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].remove.time`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#DBNAME}.{#COLLECTION}: Operations: commands, ms/s |<p>Fraction of time (ms/s) the mongod has spent to operations.</p> |DEPENDENT |mongodb.collection.ops.commands.ms["{#DBNAME}","{#COLLECTION}"]<p>**Preprocessing**:</p><p>- JSONPATH: `$.totals["{#DBNAME}.{#COLLECTION}"].commands.time`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Node state |<p>An integer between 0 and 10 that represents the replica state of the current member.</p> |DEPENDENT |mongodb.rs.state[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.myState`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|MongoDB |MongoDB: Replication lag |<p>Delay between a write operation on the primary and its copy to a secondary.</p> |DEPENDENT |mongodb.rs.lag[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.members[?(@.self == "true")].lag.first()`</p> |
|MongoDB |MongoDB: Number of replicas |<p>The number of replucated nodes in current ReplicaSet.</p> |DEPENDENT |mongodb.rs.total_nodes[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.members[?(@.self == "true")].totalNodes.first()`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|MongoDB |MongoDB: Number of unhealthy replicas |<p>The number of replicated nodes with member health value  = 0.</p> |DEPENDENT |mongodb.rs.unhealthy_count[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.members[?(@.self == "true")].unhealthyCount.first()`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|MongoDB |MongoDB: Unhealthy replicas |<p>The replicated nodes in current ReplicaSet with member health value  = 0.</p> |DEPENDENT |mongodb.rs.unhealthy[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.members[?(@.self == "true")].unhealthyNodes.first()`</p><p>- JAVASCRIPT: `var value = JSON.parse(value); return value.length ? JSON.stringify(value) : ''; `</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|MongoDB |MongoDB: Apply batches, rate |<p>Number of batches applied across all databases per second.</p> |DEPENDENT |mongodb.rs.apply.batches.rate[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.repl.apply.batches.num`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Apply batches, ms/s |<p>Fraction of time (ms/s) the mongod has spent applying operations from the oplog.</p> |DEPENDENT |mongodb.rs.apply.batches.ms.rate[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.repl.apply.batches.totalMillis`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Apply ops, rate |<p>Number of oplog operations applied per second.</p> |DEPENDENT |mongodb.rs.apply.rate[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.repl.apply.ops`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Buffer |<p>Number of operations in the oplog buffer.</p> |DEPENDENT |mongodb.rs.buffer.count[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.repl.buffer.count`</p> |
|MongoDB |MongoDB: Buffer, max size |<p>Maximum size of the buffer.</p> |DEPENDENT |mongodb.rs.buffer.max_size[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.repl.buffer.maxSizeBytes`</p> |
|MongoDB |MongoDB: Buffer, size |<p>Current size of the contents of the oplog buffer.</p> |DEPENDENT |mongodb.rs.buffer.size[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.repl.buffer.sizeBytes`</p> |
|MongoDB |MongoDB: Network bytes, rate |<p>Amount of data read from the replication sync source per second.</p> |DEPENDENT |mongodb.rs.network.bytes.rate[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.repl.network.bytes`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Network getmores, rate |<p>Number of getmore operations per second.</p> |DEPENDENT |mongodb.rs.network.getmores.rate[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.repl.network.getmores.num`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Network getmores, ms/s |<p>Fraction of time (ms/s) required to collect data from getmore operations.</p> |DEPENDENT |mongodb.rs.network.getmores.ms.rate[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.repl.network.getmores.totalMillis`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Network ops, rate |<p>Number of operations read from the replication source per second.</p> |DEPENDENT |mongodb.rs.network.ops.rate[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.repl.network.ops`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Network readers created, rate |<p>Number of oplog query processes created per second.</p> |DEPENDENT |mongodb.rs.network.readers.rate[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.repl.network.readersCreated`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB {#RS_NAME}: Oplog time diff |<p>Oplog window: difference between the first and last operation in the oplog. Only present if there are entries in the oplog.</p> |DEPENDENT |mongodb.rs.oplog.timediff[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.timediff`</p> |
|MongoDB |MongoDB: Preload docs, rate |<p>Number of documents loaded per second during the pre-fetch stage of replication.</p> |DEPENDENT |mongodb.rs.preload.docs.rate[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.repl.preload.docs.num`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Preload docs, ms/s |<p>Fraction of time (ms/s) spent loading documents as part of the pre-fetch stage of replication.</p> |DEPENDENT |mongodb.rs.preload.docs.ms.rate[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.repl.preload.docs.totalMillis`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Preload indexes, rate |<p>Number of index entries loaded by members before updating documents as part of the pre-fetch stage of replication.</p> |DEPENDENT |mongodb.rs.preload.indexes.rate[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.repl.preload.indexes.num`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: Preload indexes, ms/s |<p>Fraction of time (ms/s) spent loading documents as part of the pre-fetch stage of replication.</p> |DEPENDENT |mongodb.rs.preload.indexes.ms.rate[{#RS_NAME}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.metrics.repl.preload.indexes.totalMillis`</p><p>- CHANGE_PER_SECOND |
|MongoDB |MongoDB: WiredTiger cache: bytes |<p>Size of the data currently in cache.</p> |DEPENDENT |mongodb.wired_tiger.cache.bytes_in_cache[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.cache['bytes currently in the cache']`</p> |
|MongoDB |MongoDB: WiredTiger cache: in-memory page splits |<p>In-memory page splits.</p> |DEPENDENT |mongodb.wired_tiger.cache.splits[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.cache['in-memory page splits']`</p> |
|MongoDB |MongoDB: WiredTiger cache: bytes, max |<p>Maximum cache size.</p> |DEPENDENT |mongodb.wired_tiger.cache.maximum_bytes_configured[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.cache['maximum bytes configured']`</p> |
|MongoDB |MongoDB: WiredTiger cache: max page size at eviction |<p>Maximum page size at eviction.</p> |DEPENDENT |mongodb.wired_tiger.cache.max_page_size_eviction[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.cache['maximum page size at eviction']`</p> |
|MongoDB |MongoDB: WiredTiger cache: modified pages evicted |<p>Number of pages, that have been modified, evicted from the cache.</p> |DEPENDENT |mongodb.wired_tiger.cache.modified_pages_evicted[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.cache['modified pages evicted']`</p> |
|MongoDB |MongoDB: WiredTiger cache: pages read into cache |<p>Number of pages read into the cache.</p> |DEPENDENT |mongodb.wired_tiger.cache.pages_read[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.cache['pages read into cache']`</p> |
|MongoDB |MongoDB: WiredTiger cache: pages written from cache |<p>Number of pages writtent from the cache.</p> |DEPENDENT |mongodb.wired_tiger.cache.pages_written[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.cache['pages written from cache']`</p> |
|MongoDB |MongoDB: WiredTiger cache: pages held in cache |<p>Number of pages currently held in the cache.</p> |DEPENDENT |mongodb.wired_tiger.cache.pages_in_cache[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.cache['pages currently held in the cache']`</p> |
|MongoDB |MongoDB: WiredTiger cache: pages evicted by application threads, rate |<p>Number of page evicted by application threads per second.</p> |DEPENDENT |mongodb.wired_tiger.cache.pages_evicted_threads.rate[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.cache.['pages evicted by application threads']`</p> |
|MongoDB |MongoDB: WiredTiger cache: tracked dirty bytes in the cache |<p>Size of the dirty data in the cache.</p> |DEPENDENT |mongodb.wired_tiger.cache.tracked_dirty_bytes[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.cache.['tracked dirty bytes in the cache']`</p> |
|MongoDB |MongoDB: WiredTiger cache: unmodified pages evicted |<p>Number of pages, that were not modified, evicted from the cache.</p> |DEPENDENT |mongodb.wired_tiger.cache.unmodified_pages_evicted[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.cache.['unmodified pages evicted']`</p> |
|MongoDB |MongoDB: WiredTiger concurrent transactions: read, available |<p>Number of available read tickets (concurrent transactions) remaining.</p> |DEPENDENT |mongodb.wired_tiger.concurrent_transactions.read.available[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.concurrentTransactions.read.available`</p> |
|MongoDB |MongoDB: WiredTiger concurrent transactions: read, out |<p>Number of read tickets (concurrent transactions) in use.</p> |DEPENDENT |mongodb.wired_tiger.concurrent_transactions.read.out[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.concurrentTransactions.read.out`</p> |
|MongoDB |MongoDB: WiredTiger concurrent transactions: read, total tickets |<p>Total number of read tickets (concurrent transactions) available.</p> |DEPENDENT |mongodb.wired_tiger.concurrent_transactions.read.totalTickets[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.concurrentTransactions.read.totalTickets`</p> |
|MongoDB |MongoDB: WiredTiger concurrent transactions: write, available |<p>Number of available write tickets (concurrent transactions) remaining.</p> |DEPENDENT |mongodb.wired_tiger.concurrent_transactions.write.available[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.concurrentTransactions.write.available`</p> |
|MongoDB |MongoDB: WiredTiger concurrent transactions: write, out |<p>Number of write tickets (concurrent transactions) in use.</p> |DEPENDENT |mongodb.wired_tiger.concurrent_transactions.write.out[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.concurrentTransactions.write.out`</p> |
|MongoDB |MongoDB: WiredTiger concurrent transactions: write, total tickets |<p>Total number of write tickets (concurrent transactions) available.</p> |DEPENDENT |mongodb.wired_tiger.concurrent_transactions.write.totalTickets[{#SINGLETON}]<p>**Preprocessing**:</p><p>- JSONPATH: `$.wiredTiger.concurrentTransactions.write.totalTickets`</p> |
|Zabbix_raw_items |MongoDB: Get server status |<p>Returns a database’s state.</p> |ZABBIX_PASSIVE |mongodb.server.status["{$MONGODB.CONNSTRING}","{$MONGODB.USER}","{$MONGODB.PASSWORD}"] |
|Zabbix_raw_items |MongoDB: Get Replica Set status |<p>Returns the replica set status from the point of view of the member where the method is run.</p> |ZABBIX_PASSIVE |mongodb.rs.status["{$MONGODB.CONNSTRING}","{$MONGODB.USER}","{$MONGODB.PASSWORD}"] |
|Zabbix_raw_items |MongoDB: Get oplog stats |<p>Returns status of the replica set, using data polled from the oplog.</p> |ZABBIX_PASSIVE |mongodb.oplog.stats["{$MONGODB.CONNSTRING}","{$MONGODB.USER}","{$MONGODB.PASSWORD}"] |
|Zabbix_raw_items |MongoDB: Get collections usage stats |<p>Returns usage statistics for each collection.</p> |ZABBIX_PASSIVE |mongodb.collections.usage["{$MONGODB.CONNSTRING}","{$MONGODB.USER}","{$MONGODB.PASSWORD}"] |
|Zabbix_raw_items |MongoDB {#DBNAME}: Get db stats {#DBNAME} |<p>Returns statistics reflecting the database system’s state.</p> |ZABBIX_PASSIVE |mongodb.db.stats["{$MONGODB.CONNSTRING}","{$MONGODB.USER}","{$MONGODB.PASSWORD}","{#DBNAME}"] |
|Zabbix_raw_items |MongoDB {#DBNAME}.{#COLLECTION}: Get collection stats {#DBNAME}.{#COLLECTION} |<p>Returns a variety of storage statistics for a given collection.</p> |ZABBIX_PASSIVE |mongodb.collection.stats["{$MONGODB.CONNSTRING}","{$MONGODB.USER}","{$MONGODB.PASSWORD}","{#DBNAME}","{#COLLECTION}"] |

## Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----|----|----|
|MongoDB: Connection to MongoDB is unavailable |<p>Connection to MongoDB instance is currently unavailable.</p> |`{TEMPLATE_NAME:mongodb.ping["{$MONGODB.CONNSTRING}","{$MONGODB.USER}","{$MONGODB.PASSWORD}"].last()}=0` |HIGH | |
|MongoDB: Version has changed (new version: {ITEM.VALUE}) |<p>MongoDB version has changed. Ack to close.</p> |`{TEMPLATE_NAME:mongodb.version.diff()}=1 and {TEMPLATE_NAME:mongodb.version.strlen()}>0` |INFO |<p>Manual close: YES</p> |
|MongoDB: has been restarted (uptime < 10m) |<p>Uptime is less than 10 minutes</p> |`{TEMPLATE_NAME:mongodb.uptime.last()}<10m` |INFO |<p>Manual close: YES</p> |
|MongoDB: Failed to fetch info data (or no data for 10m) |<p>Zabbix has not received data for items for the last 10 minutes</p> |`{TEMPLATE_NAME:mongodb.uptime.nodata(10m)}=1` |WARNING |<p>Manual close: YES</p><p>**Depends on**:</p><p>- MongoDB: Connection to MongoDB is unavailable</p> |
|MongoDB: Total number of open connections is too high (over {$MONGODB.CONNS.PCT.USED.MAX.WARN%} in 5m) |<p>Too few available connections. If MongoDB runs low on connections, in may not be able to handle incoming requests in a timely manner.</p> |`{TEMPLATE_NAME:mongodb.connections.current.min(5m)}/({MongoDB node by Zabbix Agent 2:mongodb.connections.available.last()}+{TEMPLATE_NAME:mongodb.connections.current.last()})*100>{$MONGODB.CONNS.PCT.USED.MAX.WARN}` |WARNING | |
|MongoDB: Too many cursors opened by MongoDB for clients (over {$MONGODB.CURSOR.OPEN.MAX.WARN} in 5m) |<p>-</p> |`{TEMPLATE_NAME:mongodb.cursor.open.total.min(5m)}>{$MONGODB.CURSOR.OPEN.MAX.WARN}` |WARNING | |
|MongoDB: Too many cursors are timing out (over {$MONGODB.CURSOR.TIMEOUT.MAX.WARN} per second in 5m) |<p>-</p> |`{TEMPLATE_NAME:mongodb.cursor.timed_out.rate.min(5m)}>{$MONGODB.CURSOR.TIMEOUT.MAX.WARN}` |WARNING | |
|MongoDB: Node in ReplicaSet changed the state (new state: {ITEM.VALUE}) |<p>Node in ReplicaSet  changed the state. Ack to close.</p> |`{TEMPLATE_NAME:mongodb.rs.state[{#RS_NAME}].change()}=1` |WARNING |<p>Manual close: YES</p> |
|MongoDB: Replication lag with primary is too high (over {$MONGODB.REPL.LAG.MAX.WARN} in 5m) |<p>-</p> |`{TEMPLATE_NAME:mongodb.rs.lag[{#RS_NAME}].min(5m)}>{$MONGODB.REPL.LAG.MAX.WARN}` |WARNING | |
|MongoDB: There are unhealthy replicas in ReplicaSet | |`{TEMPLATE_NAME:mongodb.rs.unhealthy_count[{#RS_NAME}].last()}>0  and {MongoDB node by Zabbix Agent 2:mongodb.rs.unhealthy[{#RS_NAME}].strlen()}>0 ` |AVERAGE | |
|MongoDB: Available WiredTiger read tickets less then {$MONGODB.WIRED_TIGER.TICKETS.AVAILABLE.MIN.WARN} |<p>"Too few available read tickets.</p><p>When the number of available read tickets remaining reaches zero, new read requests will be queued until a new read ticket is available."</p> |`{TEMPLATE_NAME:mongodb.wired_tiger.concurrent_transactions.read.available[{#SINGLETON}].max(5m)}<{$MONGODB.WIRED_TIGER.TICKETS.AVAILABLE.MIN.WARN}` |WARNING | |
|MongoDB: Available WiredTiger write tickets less then {$MONGODB.WIRED_TIGER.TICKETS.AVAILABLE.MIN.WARN} |<p>"Too few available write tickets.</p><p>When the number of available write tickets remaining reaches zero, new write requests will be queued until a new write ticket is available."</p> |`{TEMPLATE_NAME:mongodb.wired_tiger.concurrent_transactions.write.available[{#SINGLETON}].max(5m)}<{$MONGODB.WIRED_TIGER.TICKETS.AVAILABLE.MIN.WARN}` |WARNING | |

## Feedback

Please report any issues with the template at https://support.zabbix.com

You can also provide a feedback, discuss the template or ask for help with it at [ZABBIX forums](https://www.zabbix.com/forum/zabbix-suggestions-and-feedback/420659-discussion-thread-for-official-zabbix-template-db-mongodb).

