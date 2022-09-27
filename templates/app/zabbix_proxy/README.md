
# Template App Zabbix Proxy

## Overview

For Zabbix version: 5.0 and higher  

## Setup

Refer to the vendor documentation.

## Zabbix configuration

No specific Zabbix configuration is required.

### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$ZABBIX.PROXY.UTIL.MAX} |<p>Maximum average percentage of time processes busy in the last minute (default is 75).</p> |`75` |
|{$ZABBIX.PROXY.UTIL.MIN} |<p>Minimum average percentage of time processes busy in the last minute (default is 65).</p> |`65` |

## Template links

There are no template links in this template.

## Discovery rules


## Items collected

|Group|Name|Description|Type|Key and additional info|
|-----|----|-----------|----|---------------------|
|Zabbix proxy |Zabbix proxy: Queue over 10 minutes |<p>Number of monitored items in the queue which are delayed at least by 10 minutes.</p> |INTERNAL |zabbix[queue,10m] |
|Zabbix proxy |Zabbix proxy: Queue |<p>Number of monitored items in the queue which are delayed at least by 6 seconds.</p> |INTERNAL |zabbix[queue] |
|Zabbix proxy |Zabbix proxy: Utilization of data sender internal processes, in % |<p>Average percentage of time data sender processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,data sender,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of configuration syncer internal processes, in % |<p>Average percentage of time configuration syncer processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,configuration syncer,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of discoverer data collector processes, in % |<p>Average percentage of time discoverer processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,discoverer,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of heartbeat sender internal processes, in % |<p>Average percentage of time heartbeat sender processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,heartbeat sender,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of history syncer internal processes, in % |<p>Average percentage of time history syncer processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,history syncer,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of housekeeper internal processes, in % |<p>Average percentage of time housekeeper processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,housekeeper,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of http poller data collector processes, in % |<p>Average percentage of time http poller processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,http poller,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of icmp pinger data collector processes, in % |<p>Average percentage of time icmp pinger processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,icmp pinger,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of ipmi manager internal processes, in % |<p>Average percentage of time ipmi manager processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,ipmi manager,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of ipmi poller data collector processes, in % |<p>Average percentage of time ipmi poller processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,ipmi poller,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of java poller data collector processes, in % |<p>Average percentage of time java poller processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,java poller,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of poller data collector processes, in % |<p>Average percentage of time poller processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,poller,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of preprocessing worker internal processes, in % |<p>Average percentage of time preprocessing worker processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,preprocessing worker,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of preprocessing manager internal processes, in % |<p>Average percentage of time preprocessing manager processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,preprocessing manager,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of self-monitoring internal processes, in % |<p>Average percentage of time self-monitoring processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,self-monitoring,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of snmp trapper data collector processes, in % |<p>Average percentage of time snmp trapper processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,snmp trapper,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of task manager internal processes, in % |<p>Average percentage of time task manager processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,task manager,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of trapper data collector processes, in % |<p>Average percentage of time trapper processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,trapper,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of unreachable poller data collector processes, in % |<p>Average percentage of time unreachable poller processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,unreachable poller,avg,busy] |
|Zabbix proxy |Zabbix proxy: Utilization of vmware data collector processes, in % |<p>Average percentage of time vmware collector processes have been busy in the last minute.</p> |INTERNAL |zabbix[process,vmware collector,avg,busy] |
|Zabbix proxy |Zabbix proxy: Configuration cache, % used |<p>Availability statistics of Zabbix configuration cache. Percentage of used buffer.</p> |INTERNAL |zabbix[rcache,buffer,pused] |
|Zabbix proxy |Zabbix proxy: Version |<p>Version of Zabbix proxy.</p> |INTERNAL |zabbix[version]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1d`</p> |
|Zabbix proxy |Zabbix proxy: VMware cache, % used |<p>Availability statistics of Zabbix vmware cache. Percentage of used buffer.</p> |INTERNAL |zabbix[vmware,buffer,pused] |
|Zabbix proxy |Zabbix proxy: History write cache, % used |<p>Statistics and availability of Zabbix write cache. Percentage of used history buffer.</p><p>History cache is used to store item values. A high number indicates performance problems on the database side.</p> |INTERNAL |zabbix[wcache,history,pused] |
|Zabbix proxy |Zabbix proxy: History index cache, % used |<p>Statistics and availability of Zabbix write cache. Percentage of used history index buffer.</p><p>History index cache is used to index values stored in history cache.</p> |INTERNAL |zabbix[wcache,index,pused] |
|Zabbix proxy |Zabbix proxy: Number of processed values per second |<p>Statistics and availability of Zabbix write cache.</p><p>Total number of values processed by Zabbix server or Zabbix proxy, except unsupported items.</p> |INTERNAL |zabbix[wcache,values]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Zabbix proxy |Zabbix proxy: Number of processed numeric (float) values per second |<p>Statistics and availability of Zabbix write cache.</p><p>Number of processed float values.</p> |INTERNAL |zabbix[wcache,values,float]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Zabbix proxy |Zabbix proxy: Number of processed log values per second |<p>Statistics and availability of Zabbix write cache.</p><p>Number of processed log values.</p> |INTERNAL |zabbix[wcache,values,log]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Zabbix proxy |Zabbix proxy: Number of processed not supported values per second |<p>Statistics and availability of Zabbix write cache.</p><p>Number of times item processing resulted in item becoming unsupported or keeping that state.</p> |INTERNAL |zabbix[wcache,values,not supported]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Zabbix proxy |Zabbix proxy: Number of processed character values per second |<p>Statistics and availability of Zabbix write cache.</p><p>Number of processed character/string values.</p> |INTERNAL |zabbix[wcache,values,str]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Zabbix proxy |Zabbix proxy: Number of processed text values per second |<p>Statistics and availability of Zabbix write cache.</p><p>Number of processed text values.</p> |INTERNAL |zabbix[wcache,values,text]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Zabbix proxy |Zabbix proxy: Preprocessing queue |<p>Count of values enqueued in the preprocessing queue.</p> |INTERNAL |zabbix[preprocessing_queue] |
|Zabbix proxy |Zabbix proxy: Number of processed numeric (unsigned) values per second |<p>Statistics and availability of Zabbix write cache.</p><p>Number of processed numeric (unsigned) values.</p> |INTERNAL |zabbix[wcache,values,uint]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Zabbix proxy |Zabbix proxy: Values waiting to be sent |<p>Number of values in the proxy history table waiting to be sent to the server.</p> |INTERNAL |zabbix[proxy_history] |
|Zabbix proxy |Zabbix proxy: Required performance |<p>Required performance of Zabbix proxy, in new values per second expected.</p> |INTERNAL |zabbix[requiredperformance] |
|Zabbix proxy |Zabbix proxy: Uptime |<p>Uptime of Zabbix proxy process in seconds.</p> |INTERNAL |zabbix[uptime] |

## Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----|----|----|
|Zabbix proxy: More than 100 items having missing data for more than 10 minutes |<p>zabbix[stats,{$IP},{$PORT},queue,10m] item is collecting data about how many items are missing data for more than 10 minutes.</p> |`{TEMPLATE_NAME:zabbix[queue,10m].min(10m)}>100` |WARNING | |
|Zabbix proxy: Utilization of data sender processes over {$ZABBIX.PROXY.UTIL.MAX:"data sender"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,data sender,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"data sender"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,data sender,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"data sender"}` |AVERAGE | |
|Zabbix proxy: Utilization of configuration syncer processes over {$ZABBIX.PROXY.UTIL.MAX:"configuration syncer"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,configuration syncer,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"configuration syncer"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,configuration syncer,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"configuration syncer"}` |AVERAGE | |
|Zabbix proxy: Utilization of discoverer processes over {$ZABBIX.PROXY.UTIL.MAX:"discoverer"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,discoverer,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"discoverer"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,discoverer,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"discoverer"}` |AVERAGE | |
|Zabbix proxy: Utilization of heartbeat sender processes over {$ZABBIX.PROXY.UTIL.MAX:"heartbeat sender"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,heartbeat sender,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"heartbeat sender"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,heartbeat sender,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"heartbeat sender"}` |AVERAGE | |
|Zabbix proxy: Utilization of history syncer processes over {$ZABBIX.PROXY.UTIL.MAX:"history syncer"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,history syncer,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"history syncer"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,history syncer,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"history syncer"}` |AVERAGE | |
|Zabbix proxy: Utilization of housekeeper processes over {$ZABBIX.PROXY.UTIL.MAX:"housekeeper"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,housekeeper,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"housekeeper"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,housekeeper,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"housekeeper"}` |AVERAGE | |
|Zabbix proxy: Utilization of http poller processes over {$ZABBIX.PROXY.UTIL.MAX:"http poller"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,http poller,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"http poller"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,http poller,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"http poller"}` |AVERAGE | |
|Zabbix proxy: Utilization of icmp pinger processes over {$ZABBIX.PROXY.UTIL.MAX:"icmp pinger"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,icmp pinger,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"icmp pinger"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,icmp pinger,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"icmp pinger"}` |AVERAGE | |
|Zabbix proxy: Utilization of ipmi manager processes over {$ZABBIX.PROXY.UTIL.MAX:"ipmi manager"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,ipmi manager,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"ipmi manager"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,ipmi manager,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"ipmi manager"}` |AVERAGE | |
|Zabbix proxy: Utilization of ipmi poller processes over {$ZABBIX.PROXY.UTIL.MAX:"ipmi poller"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,ipmi poller,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"ipmi poller"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,ipmi poller,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"ipmi poller"}` |AVERAGE | |
|Zabbix proxy: Utilization of java poller processes over {$ZABBIX.PROXY.UTIL.MAX:"java poller"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,java poller,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"java poller"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,java poller,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"java poller"}` |AVERAGE | |
|Zabbix proxy: Utilization of poller processes over {$ZABBIX.PROXY.UTIL.MAX:"poller"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,poller,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"poller"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,poller,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"poller"}` |AVERAGE | |
|Zabbix proxy: Utilization of preprocessing worker processes over {$ZABBIX.PROXY.UTIL.MAX:"preprocessing worker"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,preprocessing worker,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"preprocessing worker"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,preprocessing worker,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"preprocessing worker"}` |AVERAGE | |
|Zabbix proxy: Utilization of preprocessing manager processes over {$ZABBIX.PROXY.UTIL.MAX:"preprocessing manager"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,preprocessing manager,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"preprocessing manager"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,preprocessing manager,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"preprocessing manager"}` |AVERAGE | |
|Zabbix proxy: Utilization of self-monitoring processes over {$ZABBIX.PROXY.UTIL.MAX:"self-monitoring"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,self-monitoring,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"self-monitoring"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,self-monitoring,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"self-monitoring"}` |AVERAGE | |
|Zabbix proxy: Utilization of snmp trapper processes over {$ZABBIX.PROXY.UTIL.MAX:"snmp trapper"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,snmp trapper,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"snmp trapper"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,snmp trapper,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"snmp trapper"}` |AVERAGE | |
|Zabbix proxy: Utilization of task manager processes over {$ZABBIX.PROXY.UTIL.MAX:"task manager"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,task manager,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"task manager"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,task manager,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"task manager"}` |AVERAGE | |
|Zabbix proxy: Utilization of trapper processes over {$ZABBIX.PROXY.UTIL.MAX:"trapper"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,trapper,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"trapper"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,trapper,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"trapper"}` |AVERAGE | |
|Zabbix proxy: Utilization of unreachable poller processes over {$ZABBIX.PROXY.UTIL.MAX:"unreachable poller"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,unreachable poller,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"unreachable poller"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,unreachable poller,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"unreachable poller"}` |AVERAGE | |
|Zabbix proxy: Utilization of vmware collector processes over {$ZABBIX.PROXY.UTIL.MAX:"vmware collector"}% |<p>-</p> |`{TEMPLATE_NAME:zabbix[process,vmware collector,avg,busy].avg(10m)}>{$ZABBIX.PROXY.UTIL.MAX:"vmware collector"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:zabbix[process,vmware collector,avg,busy].avg(10m)}<{$ZABBIX.PROXY.UTIL.MIN:"vmware collector"}` |AVERAGE | |
|Zabbix proxy: More than {$ZABBIX.PROXY.UTIL.MAX}% used in the configuration cache |<p>Consider increasing CacheSize in the zabbix_proxy.conf configuration file.</p> |`{TEMPLATE_NAME:zabbix[rcache,buffer,pused].max(10m)}>{$ZABBIX.PROXY.UTIL.MAX}` |AVERAGE | |
|Zabbix proxy: Version has changed (new version: {ITEM.VALUE}) |<p>Zabbix proxy version has changed. Ack to close.</p> |`{TEMPLATE_NAME:zabbix[version].diff()}=1 and {TEMPLATE_NAME:zabbix[version].strlen()}>0` |INFO |<p>Manual close: YES</p> |
|Zabbix proxy: More than {$ZABBIX.PROXY.UTIL.MAX}% used in the vmware cache |<p>Consider increasing VMwareCacheSize in the zabbix_proxy.conf configuration file.</p> |`{TEMPLATE_NAME:zabbix[vmware,buffer,pused].max(10m)}>{$ZABBIX.PROXY.UTIL.MAX}` |AVERAGE | |
|Zabbix proxy: More than {$ZABBIX.PROXY.UTIL.MAX}% used in the history cache |<p>Consider increasing HistoryCacheSize in the zabbix_proxy.conf configuration file.</p> |`{TEMPLATE_NAME:zabbix[wcache,history,pused].max(10m)}>{$ZABBIX.PROXY.UTIL.MAX}` |AVERAGE | |
|Zabbix proxy: More than {$ZABBIX.PROXY.UTIL.MAX}% used in the history index cache |<p>Consider increasing HistoryIndexCacheSize in the zabbix_proxy.conf configuration file.</p> |`{TEMPLATE_NAME:zabbix[wcache,index,pused].max(10m)}>{$ZABBIX.PROXY.UTIL.MAX}` |AVERAGE | |
|Zabbix proxy: has been restarted (uptime < 10m) |<p>Uptime is less than 10 minutes</p> |`{TEMPLATE_NAME:zabbix[uptime].last()}<10m` |INFO |<p>Manual close: YES</p> |

## Feedback

Please report any issues with the template at https://support.zabbix.com

