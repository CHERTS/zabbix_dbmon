
# NetApp FAS3220 SNMP

## Overview

For Zabbix version: 5.0 and higher  
The template to monitor SAN NetApp FAS3220 cluster by Zabbix SNMP agent.




This template was tested on:

- NetApp FAS3220, firmware version 5.3.0
- Zabbix, version 5.0

## Setup

1\. Create a host for FAS3220 with cluster management IP as SNMPv2 interface.

2\. Link the template to the host.

3\. Customize macro values if needed.




## Zabbix configuration

No specific Zabbix configuration is required.

### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$CPU.UTIL.CRIT} |<p>The critical threshold of the CPU utilization in %.</p> |`90` |
|{$FAS3220.FS.AVAIL.MIN.CRIT} |<p>Minimum available space on the disk. Can be used with {#FSNAME} as context.</p> |`10G` |
|{$FAS3220.FS.NAME.MATCHES} |<p>This macro is used in filesystems discovery. Can be overridden on the host or linked template level.</p> |`.*` |
|{$FAS3220.FS.NAME.NOT_MATCHES} |<p>This macro is used in filesystems discovery. Can be overridden on the host or linked template level.</p> |`snapshot` |
|{$FAS3220.FS.PUSED.MAX.CRIT} |<p>Maximum percentage of disk used. Can be used with {#FSNAME} as context.</p> |`90` |
|{$FAS3220.FS.TIME} |<p>The time during which disk usage may exceed the threshold. Can be used with {#FSNAME} as context.</p> |`10G` |
|{$FAS3220.FS.TYPE.MATCHES} |<p>This macro is used in filesystems discovery. Can be overridden on the host or linked template level.</p><p>Value should be integer:</p><p>  2 - flexibleVolume,</p><p>  3 - aggregate,</p><p>  4 - stripedAggregate,</p><p>  5 - stripedVolume.</p> |`.*` |
|{$FAS3220.FS.TYPE.NOT_MATCHES} |<p>This macro is used in filesystems discovery. Can be overridden on the host or linked template level.</p><p>Value should be integer:</p><p>  2 - flexibleVolume,</p><p>  3 - aggregate,</p><p>  4 - stripedAggregate,</p><p>  5 - stripedVolume.</p> |`CHANGE_IF_NEEDED` |
|{$FAS3220.FS.USE.PCT} |<p>This macro defines what threshold will be used for disk space trigger:</p><p>  0 - use Bytes ({$FAS3220.FS.AVAIL.MIN.CRIT})</p><p>  1 - use percents ({$FAS3220.FS.PUSED.MAX.CRIT})</p><p>Can be used with {#FSNAME} as context.</p> |`1` |
|{$FAS3220.NET.PORT.NAME.MATCHES} |<p>This macro is used in net ports discovery. Can be overridden on the host or linked template level.</p> |`.*` |
|{$FAS3220.NET.PORT.NAME.NOT_MATCHES} |<p>This macro is used in net ports discovery. Can be overridden on the host or linked template level.</p> |`CHANGE_IF_NEEDED` |
|{$FAS3220.NET.PORT.ROLE.MATCHES} |<p>This macro is used in net ports discovery. Can be overridden on the host or linked template level.</p><p>{#ROLE} is integer. Possible values:</p><p>  0 - undef</p><p>  1 - cluster</p><p>  2 - data</p><p>  3 - node-mgmt</p><p>  4 - intercluster</p><p>  5 - cluster-mgmt</p> |`.*` |
|{$FAS3220.NET.PORT.ROLE.NOT_MATCHES} |<p>This macro is used in net ports discovery. Can be overridden on the host or linked template level.</p><p>{#ROLE} is integer. Possible values:</p><p>  0 - undef</p><p>  1 - cluster</p><p>  2 - data</p><p>  3 - node-mgmt</p><p>  4 - intercluster</p><p>  5 - cluster-mgmt</p> |`CHANGE_IF_NEEDED` |
|{$FAS3220.NET.PORT.TYPE.MATCHES} |<p>This macro is used in net ports discovery. Can be overridden on the host or linked template level.</p><p>{#TYPE} is integer. Possible values: physical, if-group, vlan, undef.</p> |`.*` |
|{$FAS3220.NET.PORT.TYPE.NOT_MATCHES} |<p>This macro is used in net ports discovery. Can be overridden on the host or linked template level.</p><p>{#TYPE} is integer. Possible values: physical, if-group, vlan, undef.</p> |`CHANGE_IF_NEEDED` |
|{$IF.ERRORS.WARN} |<p>-</p> |`` |
|{$IF.UTIL.MAX} |<p>-</p> |`95` |

## Template links

|Name|
|----|
|Generic SNMP |

## Discovery rules

|Name|Description|Type|Key and additional info|
|----|-----------|----|----|
|CPU discovery |<p>Discovery of CPU metrics per node</p> |SNMP |fas3220.cpu.discovery |
|Cluster metrics discovery |<p>Discovery of Cluster metrics per node</p> |SNMP |fas3220.cluster.discovery |
|HA discovery |<p>Discovery of high availability metrics per node</p> |SNMP |fas3220.ha.discovery |
|Filesystems discovery |<p>Filesystems discovery with filter.</p> |SNMP |fas3220.fs.discovery<p>**Filter**:</p>AND <p>- A: {#FSTYPE} MATCHES_REGEX `{$FAS3220.FS.TYPE.MATCHES}`</p><p>- B: {#FSTYPE} NOT_MATCHES_REGEX `{$FAS3220.FS.TYPE.NOT_MATCHES}`</p><p>- C: {#FSNAME} MATCHES_REGEX `{$FAS3220.FS.NAME.MATCHES}`</p><p>- D: {#FSNAME} NOT_MATCHES_REGEX `{$FAS3220.FS.NAME.NOT_MATCHES}`</p> |
|Network ports discovery |<p>Network interfaces discovery with filter.</p> |SNMP |fas3220.net.discovery<p>**Preprocessing**:</p><p>- JAVASCRIPT: `Text is too long. Please see the template.`</p><p>**Filter**:</p>AND <p>- A: {#TYPE} MATCHES_REGEX `{$FAS3220.NET.PORT.TYPE.MATCHES}`</p><p>- B: {#TYPE} NOT_MATCHES_REGEX `{$FAS3220.NET.PORT.TYPE.NOT_MATCHES}`</p><p>- C: {#ROLE} MATCHES_REGEX `{$FAS3220.NET.PORT.ROLE.MATCHES}`</p><p>- D: {#TYPE} NOT_MATCHES_REGEX `{$FAS3220.NET.PORT.ROLE.NOT_MATCHES}`</p><p>- E: {#IFNAME} MATCHES_REGEX `{$FAS3220.NET.PORT.NAME.MATCHES}`</p><p>- F: {#IFNAME} NOT_MATCHES_REGEX `{$FAS3220.NET.PORT.NAME.NOT_MATCHES}`</p> |

## Items collected

|Group|Name|Description|Type|Key and additional info|
|-----|----|-----------|----|---------------------|
|CPU |Node {#NODE.NAME}: CPU utilization |<p>The average, over the last minute, of the percentage of time that this processor was not idle.</p> |SNMP |fas3220.cpu[cDOTCpuBusyTimePerCent, "{#NODE.NAME}"] |
|NetApp FAS3220 |NetApp FAS3220: Product version |<p>Version string for the software running on this platform.</p> |SNMP |fas3220.inventory[productVersion]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |NetApp FAS3220: Product firmware version |<p>Version string for the firmware running on this platform.</p> |SNMP |fas3220.inventory[productFirmwareVersion]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |NetApp FAS3220: Failed disks count |<p>The number of disks that are currently broken.</p> |SNMP |fas3220.disk[diskFailedCount]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|NetApp FAS3220 |NetApp FAS3220: Failed disks message |<p>If diskFailedCount is non-zero, this is a string describing the failed disk or disks. Each failed disk is described.</p> |SNMP |fas3220.disk[diskFailedMessage]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE.NAME}: Location |<p>Node Location. Same as sysLocation for a specific node.</p> |SNMP |fas3220.cluster[nodeLocation, "{#NODE.NAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE.NAME}: Model |<p>Node Model. Same as productModel for a specific node.</p> |SNMP |fas3220.cluster[nodeModel, "{#NODE.NAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE.NAME}: Serial number |<p>Node Serial Number. Same as productSerialNum for a specific node.</p> |SNMP |fas3220.cluster[nodeSerialNumber, "{#NODE.NAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE.NAME}: Uptime |<p>Node uptime. Same as sysUpTime for a specific node.</p> |SNMP |fas3220.cluster[nodeUptime, "{#NODE.NAME}"]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.01`</p> |
|NetApp FAS3220 |Node {#NODE.NAME}: Health |<p>Whether or not the node can communicate with the cluster.</p> |SNMP |fas3220.cluster[nodeHealth, "{#NODE.NAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE.NAME}: NVRAM battery status |<p>An indication of the current status of the NVRAM battery or batteries.</p><p>Batteries which are fully or partially discharged may not fully protect the system during a crash. The end-of-life status values are based on the manufacturer's recommended life for the batteries.</p><p>Possible values:</p><p>ok(1),</p><p>partiallyDischarged(2),</p><p>fullyDischarged(3),</p><p>notPresent(4),</p><p>nearEndOfLife(5),</p><p>atEndOfLife(6),</p><p>unknown(7),</p><p>overCharged(8),</p><p>fullyCharged(9).</p> |SNMP |fas3220.cluster[nodeNvramBatteryStatus, "{#NODE.NAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE.NAME}: Over-temperature |<p>An indication of whether the hardware is currently operating outside of its recommended temperature range. The hardware will shutdown if the temperature exceeds critical thresholds.</p> |SNMP |fas3220.cluster[nodeEnvOverTemperature, "{#NODE.NAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE.NAME}: Failed FAN count |<p>Count of the number of chassis fans that are not operating within the recommended RPM range.</p> |SNMP |fas3220.cluster[nodeEnvFailedFanCount, "{#NODE.NAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE.NAME}: Failed FAN messgae |<p>Text message describing current condition of chassis fans. This is useful only if envFailedFanCount is not zero.</p> |SNMP |fas3220.cluster[nodeEnvFailedFanMessage, "{#NODE.NAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE.NAME}: Degraded power supplies count |<p>Count of the number of power supplies that are in degraded mode.</p> |SNMP |fas3220.cluster[nodeEnvFailedPowerSupplyCount, "{#NODE.NAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE.NAME}: Degraded power supplies message |<p>Text message describing the state of any power supplies that are currently degraded. This is useful only if envFailedPowerSupplyCount is not zero.</p> |SNMP |fas3220.cluster[nodeEnvFailedPowerSupplyMessage, "{#NODE.NAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE.NAME}: Cannot takeover cause |<p>The reason node cannot take over it's HA partner {#PARTNER.NAME}.</p><p>Possible states:</p><p>  ok(1),</p><p>  unknownReason(2),</p><p>  disabledByOperator(3),</p><p>  interconnectOffline(4),</p><p>  disabledByPartner(5),</p><p>  takeoverFailed(6),</p><p>  mailboxIsInDegradedState(7),</p><p>  partnermailboxIsInUninitialisedState(8),</p><p>  mailboxVersionMismatch(9),</p><p>  nvramSizeMismatch(10),</p><p>  kernelVersionMismatch(11),</p><p>  partnerIsInBootingStage(12),</p><p>  diskshelfIsTooHot(13),</p><p>  partnerIsPerformingRevert(14),</p><p>  nodeIsPerformingRevert(15),</p><p>  sametimePartnerIsAlsoTryingToTakeUsOver(16),</p><p>  alreadyInTakenoverMode(17),</p><p>  nvramLogUnsynchronized(18),</p><p>  stateofBackupMailboxIsDoubtful(19).</p> |SNMP |fas3220.ha[haCannotTakeoverCause, "{#NODE.NAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE.NAME}: HA settings |<p>High Availability configuration settings. The value notConfigured(1) indicates that the HA is not licensed. The thisNodeDead(5) setting indicates that this node has been takenover.</p> |SNMP |fas3220.ha[haSettings, "{#NODE.NAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |{#VSERVER}{#FSNAME}: Total space used |<p>The total disk space that is in use on {#FSNAME}.</p> |SNMP |fas3220.fs[df64UsedKBytes, "{#VSERVER}{#FSNAME}"]<p>**Preprocessing**:</p><p>- MULTIPLIER: `1024`</p> |
|NetApp FAS3220 |{#VSERVER}{#FSNAME}: Total space available |<p>The total disk space that is free for use on {#FSNAME}.</p> |SNMP |fas3220.fs[df64AvailKBytes, "{#VSERVER}{#FSNAME}"]<p>**Preprocessing**:</p><p>- MULTIPLIER: `1024`</p> |
|NetApp FAS3220 |{#VSERVER}{#FSNAME}: Total space |<p>The total capacity in Bytes for {#FSNAME}.</p> |SNMP |fas3220.fs[df64TotalKBytes, "{#VSERVER}{#FSNAME}"]<p>**Preprocessing**:</p><p>- MULTIPLIER: `1024`</p> |
|NetApp FAS3220 |{#VSERVER}{#FSNAME}: Used space percents |<p>The percentage of disk space currently in use on {#FSNAME}.</p> |SNMP |fas3220.fs[dfPerCentKBytesCapacity, "{#VSERVER}{#FSNAME}"] |
|NetApp FAS3220 |{#VSERVER}{#FSNAME}: Saved by compression percents |<p>Provides the percentage of compression savings in a volume, which is ((compr_saved/used)) * 10(compr_saved + 0). This is only returned for volumes.</p> |SNMP |fas3220.fs[dfCompressSavedPercent, "{#VSERVER}{#FSNAME}"] |
|NetApp FAS3220 |{#VSERVER}{#FSNAME}: Saved by deduplication percents |<p>Provides the percentage of deduplication savings in a volume, which is ((dedup_saved/(dedup_saved + used)) * 100). This is only returned for volumes.</p> |SNMP |fas3220.fs[dfDedupeSavedPercent, "{#VSERVER}{#FSNAME}"] |
|NetApp FAS3220 |Node {#NODE}: port {#IFNAME} ({#TYPE}): Up by an administrator |<p>Indicates whether the port status is set 'UP' by an administrator.</p> |SNMP |fas3220.net.port[netportUpAdmin, "{#NODE}", "{#IFNAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE}: port {#IFNAME} ({#TYPE}): Role |<p>Role of the port. A port must have one of the following roles: cluster(1), data(2), mgmt(3), intercluster(4), cluster-mgmt(5) or undef(0). The cluster port is used to communicate to other node(s) in the cluster. The data port services clients' requests. It is where all the file requests come in. The management port is used by administrator to manage resources within a node. The intercluster port is used to communicate to other cluster. The cluster-mgmt port is used to manage resources within the cluster. The undef role is for the port that has not yet been assigned a role.</p> |SNMP |fas3220.net.port[netportRole, "{#NODE}", "{#IFNAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE}: port {#IFNAME} ({#TYPE}): Speed |<p>The speed that appears on the port. It can be either undef(0), auto(1), ten Mb/s(2), hundred Mb/s(3), one Gb/s(4), or ten Gb/s(5).</p> |SNMP |fas3220.net.port[netportSpeedOper, "{#NODE}", "{#IFNAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE}: port {#IFNAME} ({#TYPE}): Bits received |<p>The total number of octets received on the interface, including framing characters.</p> |SNMP |fas3220.net.if[if64InOctets, "{#NODE}", "{#IFNAME}"]<p>**Preprocessing**:</p><p>- MULTIPLIER: `8`</p><p>- CHANGE_PER_SECOND |
|NetApp FAS3220 |Node {#NODE}: port {#IFNAME} ({#TYPE}): Bits sent |<p>The total number of octets transmitted out of the interface, including framing characters.</p> |SNMP |fas3220.net.if[if64OutOctets, "{#NODE}", "{#IFNAME}"]<p>**Preprocessing**:</p><p>- MULTIPLIER: `8`</p><p>- CHANGE_PER_SECOND |
|NetApp FAS3220 |Node {#NODE}: port {#IFNAME} ({#TYPE}): State |<p>The link-state of the port. Normally it is either UP(2) or DOWN(3).</p> |SNMP |fas3220.net.port[netportLinkState, "{#NODE}", "{#IFNAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE}: port {#IFNAME} ({#TYPE}): Health |<p>The health status of the port.</p> |SNMP |fas3220.net.port[netportHealthStatus, "{#NODE}", "{#IFNAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|NetApp FAS3220 |Node {#NODE}: port {#IFNAME} ({#TYPE}): Health degraded reason |<p>The list of reasons why the port is marked as degraded.</p> |SNMP |fas3220.net.port[netportDegradedReason, "{#NODE}", "{#IFNAME}"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Network_interfaces |Node {#NODE}: port {#IFNAME} ({#TYPE}): Inbound packets with errors |<p>MIB: IF-MIB</p><p>The number of inbound packets that contained errors preventing them from being deliverable to a higher-layer protocol.</p> |SNMP |fas3220.net.if[if64InErrors, "{#NODE}", "{#IFNAME}"]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Network_interfaces |Node {#NODE}: port {#IFNAME} ({#TYPE}): Outbound packets with errors |<p>MIB: IF-MIB</p><p>The number of outbound packets that could not be transmitted because of errors.</p> |SNMP |fas3220.net.if[if64OutErrors, "{#NODE}", "{#IFNAME}"]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Network_interfaces |Node {#NODE}: port {#IFNAME} ({#TYPE}): Inbound packets discarded |<p>MIB: IF-MIB</p><p>The number of inbound packets that were chosen to be discarded even though no errors had been detected to prevent their being deliverable to a higher-layer protocol. One possible reason for discarding such a packet could be to free up buffer space.</p> |SNMP |fas3220.net.if[if64InDiscards, "{#NODE}", "{#IFNAME}"]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Network_interfaces |Node {#NODE}: port {#IFNAME} ({#TYPE}): Outbound packets discarded |<p>MIB: IF-MIB</p><p>The number of outbound packets that were chosen to be discarded even though no errors had been detected to prevent their being transmitted. One possible reason for discarding such a packet could be to free up buffer space.</p> |SNMP |fas3220.net.if[if64OutDiscards, "{#NODE}", "{#IFNAME}"]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |

## Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----|----|----|
|Node {#NODE.NAME}: High CPU utilization (over {$CPU.UTIL.CRIT}% for 5m) |<p>CPU utilization is too high. The system might be slow to respond.</p> |`{TEMPLATE_NAME:fas3220.cpu[cDOTCpuBusyTimePerCent, "{#NODE.NAME}"].min(5m)}>{$CPU.UTIL.CRIT}` |WARNING | |
|NetApp FAS3220: Number of failed disks has changed |<p>{{ITEM.LASTVALUE2}.regsub("(.*)", \1)}</p> |`{TEMPLATE_NAME:fas3220.disk[diskFailedCount].last()}>0 and {NetApp FAS3220 SNMP:fas3220.disk[diskFailedMessage].diff()}=1`<p>Recovery expression:</p>`{TEMPLATE_NAME:fas3220.disk[diskFailedCount].last()}=0` |WARNING | |
|Node {#NODE.NAME}: has been restarted (uptime < 10m) |<p>Uptime is less than 10 minutes</p> |`{TEMPLATE_NAME:fas3220.cluster[nodeUptime, "{#NODE.NAME}"].last()}<10m` |INFO |<p>Manual close: YES</p> |
|Node {#NODE.NAME}: Node can not communicate with the cluster |<p>-</p> |`{TEMPLATE_NAME:fas3220.cluster[nodeHealth, "{#NODE.NAME}"].last()}=0` |HIGH |<p>Manual close: YES</p> |
|Node {#NODE.NAME}: NVRAM battery status is not OK |<p>-</p> |`{TEMPLATE_NAME:fas3220.cluster[nodeNvramBatteryStatus, "{#NODE.NAME}"].last()}<>1` |AVERAGE |<p>Manual close: YES</p> |
|Node {#NODE.NAME}: Temperature is over than recommended |<p>The hardware will shutdown if the temperature exceeds critical thresholds.</p> |`{TEMPLATE_NAME:fas3220.cluster[nodeEnvOverTemperature, "{#NODE.NAME}"].last()}=2` |HIGH | |
|Node {#NODE.NAME}: Failed FAN count is over than zero |<p>{{ITEM.VALUE2}.regsub("(.*)", \1)}</p> |`{TEMPLATE_NAME:fas3220.cluster[nodeEnvFailedFanCount, "{#NODE.NAME}"].last()}>0 and {NetApp FAS3220 SNMP:fas3220.cluster[nodeEnvFailedFanMessage, "{#NODE.NAME}"].last()}={NetApp FAS3220 SNMP:fas3220.cluster[nodeEnvFailedFanMessage, "{#NODE.NAME}"].last()}` |HIGH | |
|Node {#NODE.NAME}: Degraded power supplies count is more than zero |<p>{{ITEM.VALUE2}.regsub("(.*)", \1)}</p> |`{TEMPLATE_NAME:fas3220.cluster[nodeEnvFailedPowerSupplyCount, "{#NODE.NAME}"].last()}>0 and {NetApp FAS3220 SNMP:fas3220.cluster[nodeEnvFailedPowerSupplyMessage, "{#NODE.NAME}"].last()}={NetApp FAS3220 SNMP:fas3220.cluster[nodeEnvFailedPowerSupplyMessage, "{#NODE.NAME}"].last()}` |AVERAGE | |
|Node {#NODE.NAME}: Node cannot takeover it's HA partner {#PARTNER.NAME}. Reason: {ITEM.VALUE} |<p>Possible reasons:</p><p>  unknownReason(2),</p><p>  disabledByOperator(3),</p><p>  interconnectOffline(4),</p><p>  disabledByPartner(5),</p><p>  takeoverFailed(6),</p><p>  mailboxIsInDegradedState(7),</p><p>  partnermailboxIsInUninitialisedState(8),</p><p>  mailboxVersionMismatch(9),</p><p>  nvramSizeMismatch(10),</p><p>  kernelVersionMismatch(11),</p><p>  partnerIsInBootingStage(12),</p><p>  diskshelfIsTooHot(13),</p><p>  partnerIsPerformingRevert(14),</p><p>  nodeIsPerformingRevert(15),</p><p>  sametimePartnerIsAlsoTryingToTakeUsOver(16),</p><p>  alreadyInTakenoverMode(17),</p><p>  nvramLogUnsynchronized(18),</p><p>  stateofBackupMailboxIsDoubtful(19).</p> |`{TEMPLATE_NAME:fas3220.ha[haCannotTakeoverCause, "{#NODE.NAME}"].last()}<>1` |HIGH | |
|Node {#NODE.NAME}: Node has been taken over |<p>The thisNodeDead(5) setting indicates that this node has been takenover.</p> |`{TEMPLATE_NAME:fas3220.ha[haSettings, "{#NODE.NAME}"].last()}=5` |HIGH | |
|Node {#NODE.NAME}: HA is not licensed |<p>The value notConfigured(1) indicates that the HA is not licensed.</p> |`{TEMPLATE_NAME:fas3220.ha[haSettings, "{#NODE.NAME}"].last()}=1` |AVERAGE | |
|{#VSERVER}{#FSNAME}: Disk space is too low (below {$FAS3220.FS.AVAIL.MIN.CRIT:"{#FSNAME}"} for {$FAS3220.FS.TIME:"{#FSNAME}"}) |<p>-</p> |`{TEMPLATE_NAME:fas3220.fs[df64AvailKBytes, "{#VSERVER}{#FSNAME}"].min({$FAS3220.FS.TIME:"{#FSNAME}"})}<{$FAS3220.FS.AVAIL.MIN.CRIT:"{#FSNAME}"} and {$FAS3220.FS.USE.PCT:"{#FSNAME}"}=0` |HIGH | |
|{#VSERVER}{#FSNAME}: Disk space is too low (used over {$FAS3220.FS.PUSED.MAX.CRIT:"{#FSNAME}"}% for {$FAS3220.FS.TIME:"{#FSNAME}"}) |<p>-</p> |`{TEMPLATE_NAME:fas3220.fs[dfPerCentKBytesCapacity, "{#VSERVER}{#FSNAME}"].max({$FAS3220.FS.TIME:"{#FSNAME}"})}>{$FAS3220.FS.PUSED.MAX.CRIT:"{#FSNAME}"} and {$FAS3220.FS.USE.PCT:"{#FSNAME}"}=1` |HIGH | |
|Node {#NODE}: port {#IFNAME} ({#TYPE}): Link down |<p>Link state is not UP and the port status is set 'UP' by an administrator.</p> |`{TEMPLATE_NAME:fas3220.net.port[netportLinkState, "{#NODE}", "{#IFNAME}"].last()}<>2 and {NetApp FAS3220 SNMP:fas3220.net.port[netportUpAdmin, "{#NODE}", "{#IFNAME}"].last()}=1` |AVERAGE |<p>Manual close: YES</p> |
|Node {#NODE}: port {#IFNAME} ({#TYPE}): Port is not healthy |<p>{{ITEM.LASTVALUE2}.regsub("(.*)", \1)}</p> |`{TEMPLATE_NAME:fas3220.net.port[netportHealthStatus, "{#NODE}", "{#IFNAME}"].last()}<>0 and {NetApp FAS3220 SNMP:fas3220.net.port[netportDegradedReason, "{#NODE}", "{#IFNAME}"].strlen()}>0` |INFO | |
|Node {#NODE}: port {#IFNAME} ({#TYPE}): High error rate (> {$IF.ERRORS.WARN:"{#IFNAME}"} for 5m) |<p>Recovers when below 80% of {$IF.ERRORS.WARN:"{#IFNAME}"} threshold</p> |`{TEMPLATE_NAME:fas3220.net.if[if64InErrors, "{#NODE}", "{#IFNAME}"].min(5m)}>{$IF.ERRORS.WARN:"{#IFNAME}"} or {NetApp FAS3220 SNMP:fas3220.net.if[if64OutErrors, "{#NODE}", "{#IFNAME}"].min(5m)}>{$IF.ERRORS.WARN:"{#IFNAME}"}`<p>Recovery expression:</p>`{TEMPLATE_NAME:fas3220.net.if[if64InErrors, "{#NODE}", "{#IFNAME}"].max(5m)}<{$IF.ERRORS.WARN:"{#IFNAME}"}*0.8 and {NetApp FAS3220 SNMP:fas3220.net.if[if64OutErrors, "{#NODE}", "{#IFNAME}"].max(5m)}<{$IF.ERRORS.WARN:"{#IFNAME}"}*0.8` |WARNING |<p>Manual close: YES</p> |

## Feedback

Please report any issues with the template at https://support.zabbix.com

You can also provide a feedback, discuss the template or ask for help with it at [ZABBIX forums](https://www.zabbix.com/forum/zabbix-suggestions-and-feedback/416694-discussion-thread-for-official-zabbix-template-netapp-fas3220).

