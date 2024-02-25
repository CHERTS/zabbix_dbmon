
# Remote Zabbix server health

## Overview

This template is designed to monitor internal Zabbix metrics on the remote Zabbix server.

## Requirements

Zabbix version: 6.0 and higher.

## Tested versions

This template has been tested on:
- Zabbix server 6.0

## Configuration

> Zabbix should be configured according to the instructions in the [Templates out of the box](https://www.zabbix.com/documentation/6.0/manual/config/templates_out_of_the_box) section.

## Setup

Specify the address of the remote Zabbix server by changing `{$ZABBIX.SERVER.ADDRESS}` and `{$ZABBIX.SERVER.PORT}` macros. Don't forget to adjust the `StatsAllowedIP` parameter in the remote server's configuration file to allow the collection of statistics.

### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$ZABBIX.SERVER.ADDRESS}|<p>IP/DNS/network mask list of servers to be remotely queried (default is 127.0.0.1).</p>||
|{$ZABBIX.SERVER.PORT}|<p>Port of server to be remotely queried (default is 10051).</p>||
|{$ZABBIX.SERVER.NODATA_TIMEOUT}|<p>The time threshold after which statistics are considered unavailable. Used in trigger expression.</p>|`5m`|

### Items

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Remote Zabbix server: Zabbix stats|<p>The master item of Zabbix server statistics.</p>|Zabbix internal|zabbix[stats,{$ZABBIX.SERVER.ADDRESS},{$ZABBIX.SERVER.PORT}]|
|Remote Zabbix server: Zabbix stats queue over 10m|<p>The number of monitored items in the queue, which are delayed at least by 10 minutes.</p>|Zabbix internal|zabbix[stats,{$ZABBIX.SERVER.ADDRESS},{$ZABBIX.SERVER.PORT},queue,10m]<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.queue`</p></li></ul>|
|Remote Zabbix server: Zabbix stats queue|<p>The number of monitored items in the queue, which are delayed at least by 6 seconds.</p>|Zabbix internal|zabbix[stats,{$ZABBIX.SERVER.ADDRESS},{$ZABBIX.SERVER.PORT},queue]<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.queue`</p></li></ul>|
|Remote Zabbix server: Utilization of alert manager internal processes, in %|<p>The average percentage of the time during which the alert manager processes have been busy for the last minute.</p>|Dependent item|process.alert_manager.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['alert manager'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "alert manager" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of alert syncer internal processes, in %|<p>The average percentage of the time during which the alert syncer processes have been busy for the last minute.</p>|Dependent item|process.alert_syncer.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['alert syncer'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "alert syncer" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of alerter internal processes, in %|<p>The average percentage of the time during which the alerter processes have been busy for the last minute.</p>|Dependent item|process.alerter.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['alerter'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "alerter" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of availability manager internal processes, in %|<p>The average percentage of the time during which the availability manager processes have been busy for the last minute.</p>|Dependent item|process.availability_manager.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['availability manager'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "availability manager" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of configuration syncer internal processes, in %|<p>The average percentage of the time during which the configuration syncer processes have been busy for the last minute.</p>|Dependent item|process.configuration_syncer.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['configuration syncer'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "configuration syncer" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of discoverer data collector processes, in %|<p>The average percentage of the time during which the discoverer processes have been busy for the last minute.</p>|Dependent item|process.discoverer.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['discoverer'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "discoverer" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of escalator internal processes, in %|<p>The average percentage of the time during which the escalator processes have been busy for the last minute.</p>|Dependent item|process.escalator.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['escalator'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "escalator" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of history poller data collector processes, in %|<p>The average percentage of the time during which the history poller processes have been busy for the last minute.</p>|Dependent item|process.history_poller.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['history poller'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "history poller" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of ODBC poller data collector processes, in %|<p>The average percentage of the time during which the ODBC poller processes have been busy for the last minute.</p>|Dependent item|process.odbc_poller.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['odbc poller'].busy.avg`</p></li></ul>|
|Remote Zabbix server: Utilization of history syncer internal processes, in %|<p>The average percentage of the time during which the history syncer processes have been busy for the last minute.</p>|Dependent item|process.history_syncer.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['history syncer'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "history syncer" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of housekeeper internal processes, in %|<p>The average percentage of the time during which the housekeeper processes have been busy for the last minute.</p>|Dependent item|process.housekeeper.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['housekeeper'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "housekeeper" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of http poller data collector processes, in %|<p>The average percentage of the time during which the http poller processes have been busy for the last minute.</p>|Dependent item|process.http_poller.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['http poller'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "http poller" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of icmp pinger data collector processes, in %|<p>The average percentage of the time during which the icmp pinger processes have been busy for the last minute.</p>|Dependent item|process.icmp_pinger.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['icmp pinger'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "icmp pinger" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of ipmi manager internal processes, in %|<p>The average percentage of the time during which the ipmi manager processes have been busy for the last minute.</p>|Dependent item|process.ipmi_manager.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['ipmi manager'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "ipmi manager" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of ipmi poller data collector processes, in %|<p>The average percentage of the time during which the ipmi poller processes have been busy for the last minute.</p>|Dependent item|process.ipmi_poller.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['ipmi poller'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "ipmi poller" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of java poller data collector processes, in %|<p>The average percentage of the time during which the java poller processes have been busy for the last minute.</p>|Dependent item|process.java_poller.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['java poller'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "java poller" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of LLD manager internal processes, in %|<p>The average percentage of the time during which the lld manager processes have been busy for the last minute.</p>|Dependent item|process.lld_manager.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['lld manager'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "LLD manager" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of LLD worker internal processes, in %|<p>The average percentage of the time during which the lld worker processes have been busy for the last minute.</p>|Dependent item|process.lld_worker.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['lld worker'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "LLD worker" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of poller data collector processes, in %|<p>The average percentage of the time during which the poller processes have been busy for the last minute.</p>|Dependent item|process.poller.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['poller'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "poller" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of preprocessing worker internal processes, in %|<p>The average percentage of the time during which the preprocessing worker processes have been busy for the last minute.</p>|Dependent item|process.preprocessing_worker.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['preprocessing worker'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "preprocessing worker" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of preprocessing manager internal processes, in %|<p>The average percentage of the time during which the preprocessing manager processes have been busy for the last minute.</p>|Dependent item|process.preprocessing_manager.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['preprocessing manager'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "preprocessing manager" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of proxy poller data collector processes, in %|<p>The average percentage of the time during which the proxy poller processes have been busy for the last minute.</p>|Dependent item|process.proxy_poller.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['proxy poller'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "proxy poller" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of report manager internal processes, in %|<p>The average percentage of the time during which the report manager processes have been busy for the last minute.</p>|Dependent item|process.report_manager.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['report manager'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "report manager" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of report writer internal processes, in %|<p>The average percentage of the time during which the report writer processes have been busy for the last minute.</p>|Dependent item|process.report_writer.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['report writer'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "report writer" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of self-monitoring internal processes, in %|<p>The average percentage of the time during which the self-monitoring processes have been busy for the last minute.</p>|Dependent item|process.self-monitoring.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['self-monitoring'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "self-monitoring" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of snmp trapper data collector processes, in %|<p>The average percentage of the time during which the snmp trapper processes have been busy for the last minute.</p>|Dependent item|process.snmp_trapper.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['snmp trapper'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "snmp trapper" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of task manager internal processes, in %|<p>The average percentage of the time during which the task manager processes have been busy for the last minute.</p>|Dependent item|process.task_manager.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['task manager'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "task manager" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of timer internal processes, in %|<p>The average percentage of the time during which the timer processes have been busy for the last minute.</p>|Dependent item|process.timer.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['timer'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "timer" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of service manager internal processes, in %|<p>The average percentage of the time during which the service manager processes have been busy for the last minute.</p>|Dependent item|process.service_manager.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['service manager'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "service manager" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of trigger housekeeper internal processes, in %|<p>The average percentage of the time during which the trigger housekeeper processes have been busy for the last minute.</p>|Dependent item|process.trigger_housekeeper.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['trigger housekeeper'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "trigger housekeeper" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of trapper data collector processes, in %|<p>The average percentage of the time during which the trapper processes have been busy for the last minute.</p>|Dependent item|process.trapper.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['trapper'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "trapper" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of unreachable poller data collector processes, in %|<p>The average percentage of the time during which the unreachable poller processes have been busy for the last minute.</p>|Dependent item|process.unreachable_poller.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['unreachable poller'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "unreachable poller" processes started.`</p></li></ul>|
|Remote Zabbix server: Utilization of vmware data collector processes, in %|<p>The average percentage of the time during which the vmware collector processes have been busy for the last minute.</p>|Dependent item|process.vmware_collector.avg.busy<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.process['vmware collector'].busy.avg`</p><p>⛔️Custom on fail: Set error to: `No "vmware collector" processes started.`</p></li></ul>|
|Remote Zabbix server: Configuration cache, % used|<p>The availability statistics of Zabbix configuration cache. The percentage of used data buffer.</p>|Dependent item|rcache.buffer.pused<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.rcache.pused`</p></li></ul>|
|Remote Zabbix server: Trend function cache, % of unique requests|<p>The effectiveness statistics of Zabbix trend function cache. The percentage of cached items calculated from the sum of cached items plus requests.</p><p>Low percentage most likely means that the cache size can be reduced.</p>|Dependent item|tcache.pitems<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.tcache.pitems`</p><p>⛔️Custom on fail: Set error to: `Not supported in this version.`</p></li></ul>|
|Remote Zabbix server: Trend function cache, % of misses|<p>The effectiveness statistics of Zabbix trend function cache. The percentage of cache misses.</p>|Dependent item|tcache.pmisses<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.tcache.pmisses`</p><p>⛔️Custom on fail: Set error to: `Not supported in this version.`</p></li></ul>|
|Remote Zabbix server: Value cache, % used|<p>The availability statistics of Zabbix value cache. The percentage of used data buffer.</p>|Dependent item|vcache.buffer.pused<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.vcache.buffer.pused`</p></li></ul>|
|Remote Zabbix server: Value cache hits|<p>The effectiveness statistics of Zabbix value cache. The number of cache hits (history values taken from the cache).</p>|Dependent item|vcache.cache.hits<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.vcache.cache.hits`</p></li><li>Change per second</li></ul>|
|Remote Zabbix server: Value cache misses|<p>The effectiveness statistics of Zabbix value cache. The number of cache misses (history values taken from the database).</p>|Dependent item|vcache.cache.misses<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.vcache.cache.misses`</p></li><li>Change per second</li></ul>|
|Remote Zabbix server: Value cache operating mode|<p>The operating mode of the value cache.</p>|Dependent item|vcache.cache.mode<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.vcache.cache.mode`</p></li></ul>|
|Remote Zabbix server: Version|<p>A version of Zabbix server.</p>|Dependent item|version<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.version`</p></li><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|Remote Zabbix server: VMware cache, % used|<p>The availability statistics of Zabbix vmware cache. The percentage of used data buffer.</p>|Dependent item|vmware.buffer.pused<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.vmware.pused`</p><p>⛔️Custom on fail: Set error to: `No "vmware collector" processes started.`</p></li></ul>|
|Remote Zabbix server: History write cache, % used|<p>The statistics and availability of Zabbix write cache. The percentage of used history buffer.</p><p>The history cache is used to store item values. A high number indicates performance problems on the database side.</p>|Dependent item|wcache.history.pused<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.wcache.history.pused`</p></li></ul>|
|Remote Zabbix server: History index cache, % used|<p>The statistics and availability of Zabbix write cache. The percentage of used history index buffer.</p><p>The history index cache is used to index values stored in the history cache.</p>|Dependent item|wcache.index.pused<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.wcache.index.pused`</p></li></ul>|
|Remote Zabbix server: Trend write cache, % used|<p>The statistics and availability of Zabbix write cache. The percentage of used trend buffer.</p><p>The trend cache stores the aggregate of all items that have receive data for the current hour.</p>|Dependent item|wcache.trend.pused<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.wcache.trend.pused`</p></li></ul>|
|Remote Zabbix server: Number of processed values per second|<p>The statistics and availability of Zabbix write cache.</p><p>The total number of values processed by Zabbix server or Zabbix proxy, except unsupported items.</p>|Dependent item|wcache.values<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.wcache.values.all`</p></li><li>Change per second</li></ul>|
|Remote Zabbix server: Number of processed numeric (float) values per second|<p>The statistics and availability of Zabbix write cache.</p><p>The number of processed float values.</p>|Dependent item|wcache.values.float<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.wcache.values.float`</p></li><li>Change per second</li></ul>|
|Remote Zabbix server: Number of processed log values per second|<p>The statistics and availability of Zabbix write cache.</p><p>The number of processed log values.</p>|Dependent item|wcache.values.log<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.wcache.values.log`</p></li><li>Change per second</li></ul>|
|Remote Zabbix server: Number of processed not supported values per second|<p>The statistics and availability of Zabbix write cache.</p><p>The number of times the item processing resulted in an item becoming unsupported or keeping that state.</p>|Dependent item|wcache.values.not_supported<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.wcache.values['not supported']`</p></li><li>Change per second</li></ul>|
|Remote Zabbix server: Number of processed character values per second|<p>The statistics and availability of Zabbix write cache.</p><p>The number of processed character/string values.</p>|Dependent item|wcache.values.str<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.wcache.values.str`</p></li><li>Change per second</li></ul>|
|Remote Zabbix server: Number of processed text values per second|<p>The statistics and availability of Zabbix write cache.</p><p>The number of processed text values.</p>|Dependent item|wcache.values.text<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.wcache.values.text`</p></li><li>Change per second</li></ul>|
|Remote Zabbix server: LLD queue|<p>The count of values enqueued in the low-level discovery processing queue.</p>|Dependent item|lld_queue<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.lld_queue`</p></li></ul>|
|Remote Zabbix server: Preprocessing queue|<p>The count of values enqueued in the preprocessing queue.</p>|Dependent item|preprocessing_queue<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.preprocessing_queue`</p></li></ul>|
|Remote Zabbix server: Number of processed numeric (unsigned) values per second|<p>The statistics and availability of Zabbix write cache.</p><p>The number of processed numeric (unsigned) values.</p>|Dependent item|wcache.values.uint<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.wcache.values.uint`</p></li><li>Change per second</li></ul>|

### Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Remote Zabbix server: More than 100 items having missing data for more than 10 minutes|<p>The `zabbix[stats,{$ZABBIX.SERVER.ADDRESS},{$ZABBIX.SERVER.PORT},queue,10m]` item collects data about the number of items that have been missing the data for more than 10 minutes.</p>|`min(/Remote Zabbix server health/zabbix[stats,{$ZABBIX.SERVER.ADDRESS},{$ZABBIX.SERVER.PORT},queue,10m],10m)>100`|Warning|**Manual close**: Yes|
|Remote Zabbix server: Utilization of alert manager processes is high||`avg(/Remote Zabbix server health/process.alert_manager.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of alert syncer processes is high||`avg(/Remote Zabbix server health/process.alert_syncer.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of alerter processes is high||`avg(/Remote Zabbix server health/process.alerter.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of availability manager processes is high||`avg(/Remote Zabbix server health/process.availability_manager.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of configuration syncer processes is high||`avg(/Remote Zabbix server health/process.configuration_syncer.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of discoverer processes is high||`avg(/Remote Zabbix server health/process.discoverer.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of escalator processes is high||`avg(/Remote Zabbix server health/process.escalator.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of history poller processes is high||`avg(/Remote Zabbix server health/process.history_poller.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of ODBC poller processes is high||`avg(/Remote Zabbix server health/process.odbc_poller.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of history syncer processes is high||`avg(/Remote Zabbix server health/process.history_syncer.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of housekeeper processes is high||`avg(/Remote Zabbix server health/process.housekeeper.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of http poller processes is high||`avg(/Remote Zabbix server health/process.http_poller.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of icmp pinger processes is high||`avg(/Remote Zabbix server health/process.icmp_pinger.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of ipmi manager processes is high||`avg(/Remote Zabbix server health/process.ipmi_manager.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of ipmi poller processes is high||`avg(/Remote Zabbix server health/process.ipmi_poller.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of java poller processes is high||`avg(/Remote Zabbix server health/process.java_poller.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of lld manager processes is high||`avg(/Remote Zabbix server health/process.lld_manager.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of lld worker processes is high||`avg(/Remote Zabbix server health/process.lld_worker.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of poller processes is high||`avg(/Remote Zabbix server health/process.poller.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of preprocessing worker processes is high||`avg(/Remote Zabbix server health/process.preprocessing_worker.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of preprocessing manager processes is high||`avg(/Remote Zabbix server health/process.preprocessing_manager.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of proxy poller processes is high||`avg(/Remote Zabbix server health/process.proxy_poller.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of report manager processes is high||`avg(/Remote Zabbix server health/process.report_manager.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of report writer processes is high||`avg(/Remote Zabbix server health/process.report_writer.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of self-monitoring processes is high||`avg(/Remote Zabbix server health/process.self-monitoring.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of snmp trapper processes is high||`avg(/Remote Zabbix server health/process.snmp_trapper.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of task manager processes is high||`avg(/Remote Zabbix server health/process.task_manager.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of timer processes is high||`avg(/Remote Zabbix server health/process.timer.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of service manager processes is high||`avg(/Remote Zabbix server health/process.service_manager.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of trigger housekeeper processes is high||`avg(/Remote Zabbix server health/process.trigger_housekeeper.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of trapper processes is high||`avg(/Remote Zabbix server health/process.trapper.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of unreachable poller processes is high||`avg(/Remote Zabbix server health/process.unreachable_poller.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Utilization of vmware collector processes is high||`avg(/Remote Zabbix server health/process.vmware_collector.avg.busy,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: More than 75% used in the configuration cache|<p>Consider increasing `CacheSize` in the `zabbix_server.conf` configuration file.</p>|`max(/Remote Zabbix server health/rcache.buffer.pused,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: Failed to fetch stats data|<p>Zabbix has not received statistics data for {$ZABBIX.SERVER.NODATA_TIMEOUT}.</p>|`nodata(/Remote Zabbix server health/rcache.buffer.pused,{$ZABBIX.SERVER.NODATA_TIMEOUT})=1`|Warning||
|Remote Zabbix server: More than 95% used in the value cache|<p>Consider increasing `ValueCacheSize` in the `zabbix_server.conf` configuration file.</p>|`max(/Remote Zabbix server health/vcache.buffer.pused,10m)>95`|Average|**Manual close**: Yes|
|Remote Zabbix server: Zabbix value cache working in low memory mode|<p>Once the low memory mode has been switched on, the value cache will remain in this state for 24 hours, even if the problem that triggered this mode is resolved sooner.</p>|`last(/Remote Zabbix server health/vcache.cache.mode)=1`|High|**Manual close**: Yes|
|Remote Zabbix server: Version has changed|<p>Zabbix server version has changed. Acknowledge to close the problem manually.</p>|`last(/Remote Zabbix server health/version,#1)<>last(/Remote Zabbix server health/version,#2) and length(last(/Remote Zabbix server health/version))>0`|Info|**Manual close**: Yes|
|Remote Zabbix server: More than 75% used in the vmware cache|<p>Consider increasing `VMwareCacheSize` in the `zabbix_server.conf` configuration file.</p>|`max(/Remote Zabbix server health/vmware.buffer.pused,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: More than 75% used in the history cache|<p>Consider increasing `HistoryCacheSize` in the `zabbix_server.conf` configuration file.</p>|`max(/Remote Zabbix server health/wcache.history.pused,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: More than 75% used in the history index cache|<p>Consider increasing `HistoryIndexCacheSize` in the `zabbix_server.conf` configuration file.</p>|`max(/Remote Zabbix server health/wcache.index.pused,10m)>75`|Average|**Manual close**: Yes|
|Remote Zabbix server: More than 75% used in the trends cache|<p>Consider increasing `TrendCacheSize` in the `zabbix_server.conf` configuration file.</p>|`max(/Remote Zabbix server health/wcache.trend.pused,10m)>75`|Average|**Manual close**: Yes|

### LLD rule High availability cluster node discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|High availability cluster node discovery|<p>LLD rule with item and trigger prototypes for the node discovery.</p>|Dependent item|zabbix.nodes.discovery<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.ha`</p></li></ul>|

### Item prototypes for High availability cluster node discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Cluster node [{#NODE.NAME}]: Stats|<p>Provides the statistics of a node.</p>|Dependent item|zabbix.nodes.stats[{#NODE.ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.data.ha[?(@.id=="{#NODE.ID}")].first()`</p></li></ul>|
|Cluster node [{#NODE.NAME}]: Address|<p>The IPv4 address of a node.</p>|Dependent item|zabbix.nodes.address[{#NODE.ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.address`</p></li><li><p>Discard unchanged with heartbeat: `12h`</p></li></ul>|
|Cluster node [{#NODE.NAME}]: Last access time|<p>Last access time.</p>|Dependent item|zabbix.nodes.lastaccess.time[{#NODE.ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.lastaccess`</p></li></ul>|
|Cluster node [{#NODE.NAME}]: Last access age|<p>The time between the database's `unix_timestamp()` and the last access time.</p>|Dependent item|zabbix.nodes.lastaccess.age[{#NODE.ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.lastaccess_age`</p></li></ul>|
|Cluster node [{#NODE.NAME}]: Status|<p>The status of a node.</p>|Dependent item|zabbix.nodes.status[{#NODE.ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.status`</p></li><li><p>Discard unchanged with heartbeat: `12h`</p></li></ul>|

### Trigger prototypes for High availability cluster node discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Cluster node [{#NODE.NAME}]: Status changed|<p>The state of the node has changed. Acknowledge to close the problem manually.</p>|`last(/Remote Zabbix server health/zabbix.nodes.status[{#NODE.ID}],#1)<>last(/Remote Zabbix server health/zabbix.nodes.status[{#NODE.ID}],#2)`|Info|**Manual close**: Yes|

## Feedback

Please report any issues with the template at [`https://support.zabbix.com`](https://support.zabbix.com)

You can also provide feedback, discuss the template, or ask for help at [`ZABBIX forums`](https://www.zabbix.com/forum/zabbix-suggestions-and-feedback)

