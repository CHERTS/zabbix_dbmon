
# Apache ActiveMQ by JMX

## Overview

For Zabbix version: 5.0 and higher  
Official JMX Template for Apache ActiveMQ.


This template was tested on:

- Apache ActiveMQ, version 5.15.5
- Zabbix, version 5.0

## Setup

> See [Zabbix template operation](https://www.zabbix.com/documentation/5.0/manual/config/templates_out_of_the_box/jmx) for basic instructions.

Metrics are collected by JMX.

1. Enable and configure JMX access to Apache ActiveMQ.
 See documentation for [instructions](https://activemq.apache.org/jmx.html).
2. Set values in host macros {$ACTIVEMQ.USERNAME}, {$ACTIVEMQ.PASSWORD} and {$ACTIVEMQ.PORT}.


## Zabbix configuration

No specific Zabbix configuration is required.

### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$ACTIVEMQ.BROKER.CONSUMERS.MIN.HIGH} |<p>Minimum amount of consumers for broker. Can be used with broker name as context.</p> |`1` |
|{$ACTIVEMQ.BROKER.CONSUMERS.MIN.TIME} |<p>Time during which there may be no consumers on destination. Can be used with broker name as context.</p> |`5m` |
|{$ACTIVEMQ.BROKER.PRODUCERS.MIN.HIGH} |<p>Minimum amount of producers for broker. Can be used with broker name as context.</p> |`1` |
|{$ACTIVEMQ.BROKER.PRODUCERS.MIN.TIME} |<p>Time during which there may be no producers on broker. Can be used with broker name as context.</p> |`5m` |
|{$ACTIVEMQ.DESTINATION.CONSUMERS.MIN.HIGH} |<p>Minimum amount of consumers for destination. Can be used with destination name as context.</p> |`1` |
|{$ACTIVEMQ.DESTINATION.CONSUMERS.MIN.TIME} |<p>Time during which there may be no consumers in destination. Can be used with destination name as context.</p> |`10m` |
|{$ACTIVEMQ.DESTINATION.PRODUCERS.MIN.HIGH} |<p>Minimum amount of producers for destination. Can be used with destination name as context.</p> |`1` |
|{$ACTIVEMQ.DESTINATION.PRODUCERS.MIN.TIME} |<p>Time during which there may be no producers on destination. Can be used with destination name as context.</p> |`10m` |
|{$ACTIVEMQ.EXPIRIED.WARN} |<p>Threshold for expiried messages count. Can be used with destination name as context.</p> |`0` |
|{$ACTIVEMQ.LLD.FILTER.BROKER.MATCHES} |<p>Filter of discoverable discovered brokers</p> |`.*` |
|{$ACTIVEMQ.LLD.FILTER.BROKER.NOT_MATCHES} |<p>Filter to exclude discovered brokers</p> |`CHANGE IF NEEDED` |
|{$ACTIVEMQ.LLD.FILTER.DESTINATION.MATCHES} |<p>Filter of discoverable discovered destinations</p> |`.*` |
|{$ACTIVEMQ.LLD.FILTER.DESTINATION.NOT_MATCHES} |<p>Filter to exclude discovered destinations</p> |`CHANGE IF NEEDED` |
|{$ACTIVEMQ.MEM.MAX.HIGH} |<p>Memory threshold for HIGH trigger. Can be used with destination or broker name as context.</p> |`90` |
|{$ACTIVEMQ.MEM.MAX.WARN} |<p>Memory threshold for AVERAGE trigger. Can be used with destination or broker name as context.</p> |`75` |
|{$ACTIVEMQ.MEM.TIME} |<p>Time during which the metric can be above the threshold. Can be used with destination or broker name as context.</p> |`5m` |
|{$ACTIVEMQ.MSG.RATE.WARN.TIME} |<p>The time for message enqueue/dequeue rate. Can be used with destination or broker name as context.</p> |`15m` |
|{$ACTIVEMQ.PASSWORD} |<p>Password for JMX</p> |`activemq` |
|{$ACTIVEMQ.PORT} |<p>Port for JMX</p> |`1099` |
|{$ACTIVEMQ.QUEUE.ENABLED} |<p>Use this to disable alerting for specific destination. 1 = enabled, 0 = disabled. Can be used with destination name as context.</p> |`1` |
|{$ACTIVEMQ.QUEUE.TIME} |<p>Time during which the QueueSize can be higher than threshold. Can be used with destination name as context.</p> |`10m` |
|{$ACTIVEMQ.QUEUE.WARN} |<p>Threshold for QueueSize. Can be used with destination name as context.</p> |`100` |
|{$ACTIVEMQ.STORE.MAX.HIGH} |<p>Storage threshold for HIGH trigger. Can be used with broker name as context.</p> |`90` |
|{$ACTIVEMQ.STORE.MAX.WARN} |<p>Storage threshold for AVERAGE trigger. Can be used with broker name as context.</p> |`75` |
|{$ACTIVEMQ.STORE.TIME} |<p>Time during which the metric can be above the threshold. Can be used with destination or broker name as context.</p> |`5m` |
|{$ACTIVEMQ.TEMP.MAX.HIGH} |<p>Temp threshold for HIGH trigger. Can be used with broker name as context.</p> |`90` |
|{$ACTIVEMQ.TEMP.MAX.WARN} |<p>Temp threshold for AVERAGE trigger. Can be used with broker name as context.</p> |`75` |
|{$ACTIVEMQ.TEMP.TIME} |<p>Time during which the metric can be above the threshold. Can be used with destination or broker name as context.</p> |`5m` |
|{$ACTIVEMQ.TOTAL.CONSUMERS.COUNT} |<p>Attribute for TotalConsumerCount per destination. Used to suppress destination's triggers when the count of consumers on the broker is lower than threshold.</p> |`TotalConsumerCount` |
|{$ACTIVEMQ.TOTAL.PRODUCERS.COUNT} |<p>Attribute for TotalProducerCount per destination. Used to suppress destination's triggers when the count of consumers on the broker is lower than threshold.</p> |`TotalProducerCount` |
|{$ACTIVEMQ.USER} |<p>User for JMX</p> |`admin` |

## Template links

There are no template links in this template.

## Discovery rules

|Name|Description|Type|Key and additional info|
|----|-----------|----|----|
|Brokers discovery |<p>Discovery of brokers</p> |JMX |jmx.discovery[beans,"org.apache.activemq:type=Broker,brokerName=*"]<p>**Filter**:</p>FORMULA A and B<p>- A: {#JMXBROKERNAME} MATCHES_REGEX `{$ACTIVEMQ.LLD.FILTER.BROKER.MATCHES}`</p><p>- B: {#JMXBROKERNAME} NOT_MATCHES_REGEX `{$ACTIVEMQ.LLD.FILTER.BROKER.NOT_MATCHES}`</p> |
|Destinations discovery |<p>Discovery of destinations</p> |JMX |jmx.discovery[beans,"org.apache.activemq:type=Broker,brokerName=*,destinationType=*,destinationName=*"]<p>**Filter**:</p>FORMULA A and B<p>- A: {#JMXDESTINATIONNAME} MATCHES_REGEX `{$ACTIVEMQ.LLD.FILTER.DESTINATION.MATCHES}`</p><p>- B: {#JMXDESTINATIONNAME} NOT_MATCHES_REGEX `{$ACTIVEMQ.LLD.FILTER.DESTINATION.NOT_MATCHES}`</p> |

## Items collected

|Group|Name|Description|Type|Key and additional info|
|-----|----|-----------|----|---------------------|
|ActiveMQ |Broker {#JMXBROKERNAME}: Version |<p>The version of the broker.</p> |JMX |jmx[{#JMXOBJ},BrokerVersion]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `3h`</p> |
|ActiveMQ |Broker {#JMXBROKERNAME}: Uptime |<p>The uptime of the broker.</p> |JMX |jmx[{#JMXOBJ},UptimeMillis]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.001`</p> |
|ActiveMQ |Broker {#JMXBROKERNAME}: Memory limit |<p>Memory limit, in bytes, used for holding undelivered messages before paging to temporary storage.</p> |JMX |jmx[{#JMXOBJ},MemoryLimit]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|ActiveMQ |Broker {#JMXBROKERNAME}: Memory usage in percents |<p>Percent of memory limit used.</p> |JMX |jmx[{#JMXOBJ}, MemoryPercentUsage] |
|ActiveMQ |Broker {#JMXBROKERNAME}: Storage limit |<p>Disk limit, in bytes, used for persistent messages before producers are blocked.</p> |JMX |jmx[{#JMXOBJ},StoreLimit]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|ActiveMQ |Broker {#JMXBROKERNAME}: Storage usage in percents |<p>Percent of store limit used.</p> |JMX |jmx[{#JMXOBJ},StorePercentUsage] |
|ActiveMQ |Broker {#JMXBROKERNAME}: Temp limit |<p>Disk limit, in bytes, used for non-persistent messages and temporary data before producers are blocked.</p> |JMX |jmx[{#JMXOBJ},TempLimit]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|ActiveMQ |Broker {#JMXBROKERNAME}: Temp usage in percents |<p>Percent of temp limit used.</p> |JMX |jmx[{#JMXOBJ},TempPercentUsage] |
|ActiveMQ |Broker {#JMXBROKERNAME}: Messages enqueue rate |<p>Rate of messages that have been sent to the broker.</p> |JMX |jmx[{#JMXOBJ},TotalEnqueueCount]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|ActiveMQ |Broker {#JMXBROKERNAME}: Messages dequeue rate |<p>Rate of messages that have been delivered by the broker and acknowledged by consumers.</p> |JMX |jmx[{#JMXOBJ},TotalDequeueCount]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|ActiveMQ |Broker {#JMXBROKERNAME}: Consumers count total |<p>Number of consumers attached to this broker.</p> |JMX |jmx[{#JMXOBJ},TotalConsumerCount] |
|ActiveMQ |Broker {#JMXBROKERNAME}: Producers count total |<p>Number of producers attached to this broker.</p> |JMX |jmx[{#JMXOBJ},TotalProducerCount] |
|ActiveMQ |{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Consumers count |<p>Number of consumers attached to this destination.</p> |JMX |jmx[{#JMXOBJ},ConsumerCount] |
|ActiveMQ |{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Consumers count total on {#JMXBROKERNAME} |<p>Number of consumers attached to the broker of this destination. Used to suppress destination's triggers when the count of consumers on the broker is lower than threshold.</p> |JMX |jmx["org.apache.activemq:type=Broker,brokerName={#JMXBROKERNAME}",{$ACTIVEMQ.TOTAL.CONSUMERS.COUNT: "{#JMXDESTINATIONNAME}"}]<p>**Preprocessing**:</p><p>- IN_RANGE: `0 {$ACTIVEMQ.BROKER.CONSUMERS.MIN.HIGH}`</p><p>⛔️ON_FAIL: `CUSTOM_VALUE -> {$ACTIVEMQ.BROKER.CONSUMERS.MIN.HIGH}`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `3h`</p> |
|ActiveMQ |{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Producers count |<p>Number of producers attached to this destination.</p> |JMX |jmx[{#JMXOBJ},ProducerCount] |
|ActiveMQ |{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Producers count total on {#JMXBROKERNAME} |<p>Number of producers attached to the broker of this destination. Used to suppress destination's triggers when the count of producers on the broker is lower than threshold.</p> |JMX |jmx["org.apache.activemq:type=Broker,brokerName={#JMXBROKERNAME}",{$ACTIVEMQ.TOTAL.PRODUCERS.COUNT: "{#JMXDESTINATIONNAME}"}]<p>**Preprocessing**:</p><p>- IN_RANGE: `0 {$ACTIVEMQ.BROKER.PRODUCERS.MIN.HIGH}`</p><p>⛔️ON_FAIL: `CUSTOM_VALUE -> {$ACTIVEMQ.BROKER.PRODUCERS.MIN.HIGH}`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `3h`</p> |
|ActiveMQ |{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Memory usage in percents |<p>The percentage of the memory limit used.</p> |JMX |jmx[{#JMXOBJ},MemoryPercentUsage] |
|ActiveMQ |{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Messages enqueue rate |<p>Rate of messages that have been sent to the destination.</p> |JMX |jmx[{#JMXOBJ},EnqueueCount]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|ActiveMQ |{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Messages dequeue rate |<p>Rate of messages that has been acknowledged (and removed) from the destination.</p> |JMX |jmx[{#JMXOBJ},DequeueCount]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|ActiveMQ |{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Queue size |<p>Number of messages on this destination, including any that have been dispatched but not acknowledged.</p> |JMX |jmx[{#JMXOBJ},QueueSize] |
|ActiveMQ |{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Expired messages count |<p>Number of messages that have been expired.</p> |JMX |jmx[{#JMXOBJ},ExpiredCount]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `3h`</p> |

## Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----|----|----|
|Broker {#JMXBROKERNAME}: Version has been changed |<p>Broker {#JMXBROKERNAME} version has changed. Ack to close.</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},BrokerVersion].diff()}=1 and {TEMPLATE_NAME:jmx[{#JMXOBJ},BrokerVersion].strlen()}>0` |INFO |<p>Manual close: YES</p> |
|Broker {#JMXBROKERNAME}: Broker has been restarted (uptime < 10m) |<p>Uptime is less than 10 minutes</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},UptimeMillis].last()}<10m` |INFO |<p>Manual close: YES</p> |
|Broker {#JMXBROKERNAME}: Memory usage is too high (over {$ACTIVEMQ.MEM.MAX.WARN:"{#JMXBROKERNAME}"}%) |<p>-</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ}, MemoryPercentUsage].min({$ACTIVEMQ.MEM.TIME:"{#JMXBROKERNAME}"})}>{$ACTIVEMQ.MEM.MAX.WARN:"{#JMXBROKERNAME}"}` |AVERAGE |<p>**Depends on**:</p><p>- Broker {#JMXBROKERNAME}: Memory usage is too high (over {$ACTIVEMQ.MEM.MAX.HIGH:"{#JMXBROKERNAME}"}%)</p> |
|Broker {#JMXBROKERNAME}: Memory usage is too high (over {$ACTIVEMQ.MEM.MAX.HIGH:"{#JMXBROKERNAME}"}%) |<p>-</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ}, MemoryPercentUsage].min({$ACTIVEMQ.MEM.TIME:"{#JMXBROKERNAME}"})}>{$ACTIVEMQ.MEM.MAX.HIGH:"{#JMXBROKERNAME}"}` |HIGH | |
|Broker {#JMXBROKERNAME}: Storage usage is too high (over {$ACTIVEMQ.STORE.MAX.WARN:"{#JMXBROKERNAME}"}%) |<p>-</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},StorePercentUsage].min({$ACTIVEMQ.STORE.TIME:"{#JMXBROKERNAME}"})}>{$ACTIVEMQ.STORE.MAX.WARN:"{#JMXBROKERNAME}"}` |AVERAGE |<p>**Depends on**:</p><p>- Broker {#JMXBROKERNAME}: Storage usage is too high (over {$ACTIVEMQ.STORE.MAX.HIGH:"{#JMXBROKERNAME}"}%)</p> |
|Broker {#JMXBROKERNAME}: Storage usage is too high (over {$ACTIVEMQ.STORE.MAX.HIGH:"{#JMXBROKERNAME}"}%) |<p>-</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},StorePercentUsage].min({$ACTIVEMQ.STORE.TIME:"{#JMXBROKERNAME}"})}>{$ACTIVEMQ.STORE.MAX.HIGH:"{#JMXBROKERNAME}"}` |HIGH | |
|Broker {#JMXBROKERNAME}: Temp usage is too high (over {$ACTIVEMQ.TEMP.MAX.WARN:"{#JMXBROKERNAME}"}%) |<p>-</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},TempPercentUsage].min({$ACTIVEMQ.TEMP.TIME:"{#JMXBROKERNAME}"})}>{$ACTIVEMQ.TEMP.MAX.WARN}` |AVERAGE |<p>**Depends on**:</p><p>- Broker {#JMXBROKERNAME}: Temp usage is too high (over {$ACTIVEMQ.TEMP.MAX.WARN:"{#JMXBROKERNAME}"}%)</p> |
|Broker {#JMXBROKERNAME}: Temp usage is too high (over {$ACTIVEMQ.TEMP.MAX.WARN:"{#JMXBROKERNAME}"}%) |<p>-</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},TempPercentUsage].min({$ACTIVEMQ.TEMP.TIME:"{#JMXBROKERNAME}"})}>{$ACTIVEMQ.TEMP.MAX.HIGH}` |HIGH | |
|Broker {#JMXBROKERNAME}: Message enqueue rate is higher than dequeue rate for {$ACTIVEMQ.MSG.RATE.WARN.TIME:"{#JMXBROKERNAME}"} |<p>Enqueue rate is higher than dequeue rate. It may indicate performance problems.</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},TotalEnqueueCount].avg({$ACTIVEMQ.MSG.RATE.WARN.TIME:"{#JMXBROKERNAME}"})}>{Apache ActiveMQ by JMX:jmx[{#JMXOBJ},TotalEnqueueCount].avg({$ACTIVEMQ.MSG.RATE.WARN.TIME:"{#JMXBROKERNAME}"})}` |AVERAGE | |
|Broker {#JMXBROKERNAME}: Consumers count is too low (below {$ACTIVEMQ.BROKER.CONSUMERS.MIN.HIGH:"{#JMXBROKERNAME}"} for {$ACTIVEMQ.BROKER.CONSUMERS.MIN.TIME:"{#JMXBROKERNAME}"}) |<p>-</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},TotalConsumerCount].max({$ACTIVEMQ.BROKER.CONSUMERS.MIN.TIME:"{#JMXBROKERNAME}"})}<{$ACTIVEMQ.BROKER.CONSUMERS.MIN.HIGH:"{#JMXBROKERNAME}"}` |HIGH | |
|Broker {#JMXBROKERNAME}: Producers count is too low (below {$ACTIVEMQ.BROKER.PRODUCERS.MIN.HIGH:"{#JMXBROKERNAME}"} for {$ACTIVEMQ.BROKER.PRODUCERS.MIN.TIME:"{#JMXBROKERNAME}"}) |<p>-</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},TotalProducerCount].max({$ACTIVEMQ.BROKER.PRODUCERS.MIN.TIME:"{#JMXBROKERNAME}"})}<{$ACTIVEMQ.BROKER.PRODUCERS.MIN.HIGH:"{#JMXBROKERNAME}"}` |HIGH | |
|{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Consumers count is too low (below {$ACTIVEMQ.DESTINATION.CONSUMERS.MIN.HIGH:"{#JMXDESTINATIONNAME}"} for {$ACTIVEMQ.DESTINATION.CONSUMERS.MIN.TIME:"{#JMXDESTINATIONNAME}"}) |<p>-</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},ConsumerCount].max({$ACTIVEMQ.DESTINATION.CONSUMERS.MIN.TIME:"{#JMXDESTINATIONNAME}"})}<{$ACTIVEMQ.DESTINATION.CONSUMERS.MIN.HIGH:"{#JMXDESTINATIONNAME}"} and {Apache ActiveMQ by JMX:jmx["org.apache.activemq:type=Broker,brokerName={#JMXBROKERNAME}",{$ACTIVEMQ.TOTAL.CONSUMERS.COUNT: "{#JMXDESTINATIONNAME}"}].last()}>{$ACTIVEMQ.BROKER.CONSUMERS.MIN.HIGH:"{#JMXBROKERNAME}"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:jmx[{#JMXOBJ},ConsumerCount].min({$ACTIVEMQ.DESTINATION.CONSUMERS.MIN.TIME:"{#JMXDESTINATIONNAME}"})}>={$ACTIVEMQ.DESTINATION.CONSUMERS.MIN.HIGH:"{#JMXDESTINATIONNAME}"}` |AVERAGE |<p>Manual close: YES</p> |
|{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Producers count is too low (below {$ACTIVEMQ.DESTINATION.PRODUCERS.MIN.HIGH:"{#JMXDESTINATIONNAME}"} for {$ACTIVEMQ.DESTINATION.PRODUCERS.MIN.TIME:"{#JMXDESTINATIONNAME}"}) |<p>-</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},ProducerCount].max({$ACTIVEMQ.DESTINATION.PRODUCERS.MIN.TIME:"{#JMXDESTINATIONNAME}"})}<{$ACTIVEMQ.DESTINATION.PRODUCERS.MIN.HIGH:"{#JMXDESTINATIONNAME}"} and {Apache ActiveMQ by JMX:jmx["org.apache.activemq:type=Broker,brokerName={#JMXBROKERNAME}",{$ACTIVEMQ.TOTAL.PRODUCERS.COUNT: "{#JMXDESTINATIONNAME}"}].last()}>{$ACTIVEMQ.BROKER.PRODUCERS.MIN.HIGH:"{#JMXBROKERNAME}"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:jmx[{#JMXOBJ},ProducerCount].min({$ACTIVEMQ.DESTINATION.PRODUCERS.MIN.TIME:"{#JMXDESTINATIONNAME}"})}>={$ACTIVEMQ.DESTINATION.PRODUCERS.MIN.HIGH:"{#JMXDESTINATIONNAME}"}` |AVERAGE |<p>Manual close: YES</p> |
|{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Memory usage is too high (over {$ACTIVEMQ.MEM.MAX.WARN:"{#JMXDESTINATIONNAME}"}%) |<p>-</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},MemoryPercentUsage].last()}>{$ACTIVEMQ.MEM.MAX.WARN:"{#JMXDESTINATIONNAME}"}` |AVERAGE | |
|{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Memory usage is too high (over {$ACTIVEMQ.MEM.MAX.HIGH:"{#JMXDESTINATIONNAME}"}%) |<p>-</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},MemoryPercentUsage].last()}>{$ACTIVEMQ.MEM.MAX.HIGH:"{#JMXDESTINATIONNAME}"}` |HIGH | |
|{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Message enqueue rate is higher than dequeue rate for {$ACTIVEMQ.MSG.RATE.WARN.TIME:"{#JMXDESTINATIONNAME}"} |<p>Enqueue rate is higher than dequeue rate. It may indicate performance problems.</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},EnqueueCount].avg({$ACTIVEMQ.MSG.RATE.WARN.TIME:"{#JMXDESTINATIONNAME}"})}>{Apache ActiveMQ by JMX:jmx[{#JMXOBJ},DequeueCount].avg({$ACTIVEMQ.MSG.RATE.WARN.TIME:"{#JMXDESTINATIONNAME}"})}` |AVERAGE | |
|{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Queue size higher than {$ACTIVEMQ.QUEUE.WARN:"{#JMXDESTINATIONNAME}"} for {$ACTIVEMQ.QUEUE.TIME:"{#JMXDESTINATIONNAME}"} |<p>Queue size is higher than threshold. It may indicate performance problems.</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},QueueSize].min({$ACTIVEMQ.QUEUE.TIME:"{#JMXDESTINATIONNAME}"})}>{$ACTIVEMQ.QUEUE.WARN:"{#JMXDESTINATIONNAME}"} and {$ACTIVEMQ.QUEUE.ENABLED:"{#JMXDESTINATIONNAME}"}=1` |AVERAGE | |
|{#JMXBROKERNAME}: {#JMXDESTINATIONTYPE} {#JMXDESTINATIONNAME}: Expired messages count higher than {$ACTIVEMQ.EXPIRIED.WARN:"{#JMXDESTINATIONNAME}"} |<p>This metric represents the number of messages that expired before they could be delivered. If you expect all messages to be delivered and acknowledged within a certain amount of time, you can set an expiration for each message, and investigate if your ExpiredCount metric rises above zero.</p> |`{TEMPLATE_NAME:jmx[{#JMXOBJ},ExpiredCount].last()}>{$ACTIVEMQ.EXPIRIED.WARN:"{#JMXDESTINATIONNAME}"}` |AVERAGE | |

## Feedback

Please report any issues with the template at https://support.zabbix.com

You can also provide a feedback, discuss the template or ask for help with it at [ZABBIX forums](https://www.zabbix.com/forum/zabbix-suggestions-and-feedback/411049-discussion-thread-for-official-zabbix-template-amq).

