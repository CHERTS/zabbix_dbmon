
# ZYXEL MES3500-24S SNMP

## Overview

For Zabbix version: 5.0 and higher  
hhttps://service-provider.zyxel.com/emea/en/products/carrier-and-access-switches/access-switches/mes3500-series

This template was tested on:

- ZYXEL MES3500-24S, version V4.10(ABBR.1)C0_20200309 | 03/09/2020

## Setup

> See [Zabbix template operation](https://www.zabbix.com/documentation/5.0/manual/config/templates_out_of_the_box/network_devices) for basic instructions.

Refer to the vendor documentation.

## Zabbix configuration

No specific Zabbix configuration is required.

### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$CPU.UTIL.CRIT} |<p>-</p> |`90` |
|{$MEMORY.UTIL.MAX} |<p>-</p> |`90` |
|{$SNMP.TIMEOUT} |<p>The time interval for SNMP agent availability trigger expression.</p> |`5m` |
|{$ZYXEL.LLD.FILTER.IF.CONTROL.MATCHES} |<p>Triggers will be created only for interfaces whose description contains the value of this macro</p> |`CHANGE_IF_NEEDED` |
|{$ZYXEL.LLD.FILTER.IF.LINKUPTYPE.MATCHES} |<p>Filter of discoverable link types.</p><p>0 - Down link</p><p>1 - Cooper link</p><p>2 - Fiber link</p><p>3 - XFP</p><p>4 - CX4</p> |`1|2|3|4` |
|{$ZYXEL.LLD.FILTER.IF.LINKUPTYPE.NOT_MATCHES} |<p>Filter to exclude discovered by link types.</p> |`CHANGE_IF_NEEDED` |
|{$ZYXEL.LLD.FILTER.IF.NAME.MATCHES} |<p>Filter by discoverable interface names.</p> |`.*` |
|{$ZYXEL.LLD.FILTER.IF.NAME.NOT_MATCHES} |<p>Filter to exclude discovered interfaces by name.</p> |`CHANGE_IF_NEEDED` |
|{$ZYXEL.LLD.FILTER.SFP.STATUS.MATCHES} |<p>Filter of discoverable status.</p><p>0 - OK with DDM</p><p>1 - OK without DDM</p><p>2 - nonoperational</p> |`1|2` |
|{$ZYXEL.LLD.FILTER.SFP.STATUS.NOT_MATCHES} |<p>Filter to exclude discovered by status.</p> |`CHANGE_IF_NEEDED` |
|{$ZYXEL.LLD.FILTER.SFPDDM.DESC.MATCHES} |<p>Filter by discoverable SFP modules name.</p> |`.*` |
|{$ZYXEL.LLD.FILTER.SFPDDM.DESC.NOT_MATCHES} |<p>Filter to exclude discovered SFP modules by name.</p> |`N/A` |
|{$ZYXEL.LLD.SFP.UPDATE} |<p>Receiving data from the SFP module is slow, we do not recommend setting the interval less than 10 minutes.</p> |`10m` |

## Template links

There are no template links in this template.

## Discovery rules

|Name|Description|Type|Key and additional info|
|----|-----------|----|----|
|Fan discovery |<p>An entry in fanRpmTable.</p> |SNMP |zyxel.3500_24s.fan.discovery |
|Temperature discovery |<p>An entry in tempTable.</p> |SNMP |zyxel.3500_24s.temp.discovery |
|Voltage discovery |<p>An entry in voltageTable.</p> |SNMP |zyxel.3500_24s.volt.discovery<p>**Preprocessing**:</p><p>- JAVASCRIPT: `The text is too long. Please see the template.`</p> |
|Interface discovery |<p>-</p> |SNMP |zyxel.3500_24s.net.if.discovery<p>**Filter**:</p>AND <p>- A: {#ZYXEL.IF.NAME} MATCHES_REGEX `{$ZYXEL.LLD.FILTER.IF.NAME.MATCHES}`</p><p>- B: {#ZYXEL.IF.NAME} NOT_MATCHES_REGEX `{$ZYXEL.LLD.FILTER.IF.NAME.NOT_MATCHES}`</p><p>- C: {#ZYXEL.IF.LINKUPTYPE} MATCHES_REGEX `{$ZYXEL.LLD.FILTER.IF.LINKUPTYPE.MATCHES}`</p><p>- D: {#ZYXEL.IF.LINKUPTYPE} NOT_MATCHES_REGEX `{$ZYXEL.LLD.FILTER.IF.LINKUPTYPE.NOT_MATCHES}`</p> |
|SFP without DDM discovery |<p>SFP module discovery.</p> |SNMP |zyxel.3500_24s.sfp.discovery<p>**Filter**:</p>AND <p>- A: {#ZYXEL.SFP.STATUS} MATCHES_REGEX `{$ZYXEL.LLD.FILTER.SFP.STATUS.MATCHES}`</p><p>- B: {#ZYXEL.SFP.STATUS} NOT_MATCHES_REGEX `{$ZYXEL.LLD.FILTER.SFP.STATUS.NOT_MATCHES}`</p> |
|SFP with DDM discovery |<p>SFP DDM module discovery.</p> |SNMP |zyxel.3500_24s.sfp.ddm.discovery<p>**Preprocessing**:</p><p>- JAVASCRIPT: `The text is too long. Please see the template.`</p><p>**Filter**:</p>AND <p>- A: {#ZYXEL.SFP.DESCRIPTION} MATCHES_REGEX `{$ZYXEL.LLD.FILTER.SFPDDM.DESC.MATCHES}`</p><p>- B: {#ZYXEL.SFP.DESCRIPTION} NOT_MATCHES_REGEX `{$ZYXEL.LLD.FILTER.SFPDDM.DESC.NOT_MATCHES}`</p> |

## Items collected

|Group|Name|Description|Type|Key and additional info|
|-----|----|-----------|----|---------------------|
|CPU |ZYXEL MES3500-24S: CPU utilization |<p>MIB: ZYXEL-ES-COMMON</p><p>Show device CPU load in %, it's the snapshot of CPU load when </p><p>getting the values.</p> |SNMP |zyxel.3500_24s.cpuusage |
|Fans |ZYXEL MES3500-24S: {#ZYXEL.FAN.DESCRIPTION} |<p>MIB: ZYXEL-HW-MONITOR-MIB</p><p>Current speed in Revolutions Per Minute (RPM) on the fan.</p> |SNMP |zyxel.3500_24s.fan[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Inventory |ZYXEL MES3500-24S: Hardware model name |<p>MIB: RFC1213-MIB</p><p>A textual description of the entity.  This value</p><p>should include the full name and version</p><p>identification of the system's hardware type,</p><p>software operating-system, and networking</p><p>software.  It is mandatory that this only contain</p><p>printable ASCII characters.</p> |SNMP |zyxel.3500_24s.model<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Inventory |ZYXEL MES3500-24S: Contact |<p>MIB: RFC1213-MIB</p><p>The textual identification of the contact person</p><p>for this managed node, together with information</p><p>on how to contact this person.</p> |SNMP |zyxel.3500_24s.contact<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Inventory |ZYXEL MES3500-24S: Host name |<p>MIB: RFC1213-MIB</p><p>An administratively-assigned name for this</p><p>managed node.  By convention, this is the node's</p><p>fully-qualified domain name.</p> |SNMP |zyxel.3500_24s.name<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Inventory |ZYXEL MES3500-24S: Location |<p>MIB: RFC1213-MIB</p><p>The physical location of this node (e.g.,</p><p>`telephone closet, 3rd floor').</p> |SNMP |zyxel.3500_24s.location<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Inventory |ZYXEL MES3500-24S: MAC address |<p>MIB: IF-MIB</p><p>The interface's address at the protocol layer</p><p>immediately `below' the network layer in the</p><p>protocol stack.  For interfaces which do not have</p><p>such an address (e.g., a serial line), this object</p><p>should contain an octet string of zero length.</p> |SNMP |zyxel.3500_24s.mac<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Inventory |ZYXEL MES3500-24S: ZyNOS F/W Version |<p>MIB: ZYXEL-ES-COMMON</p> |SNMP |zyxel.3500_24s.fwversion<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Inventory |ZYXEL MES3500-24S: Hardware serial number |<p>MIB: ZYXEL-ES-COMMON</p><p>Serial number</p> |SNMP |zyxel.3500_24s.serialnumber<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Memory |ZYXEL MES3500-24S: Memory utilization |<p>MIB: ZYXEL-ES-COMMON</p><p>Show device memory usage in %.</p> |SNMP |zyxel.3500_24s.memusage |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Speed Duplex |<p>MIB: ZYXEL-PORT-MIB</p><p>Select The speed and the duplex mode of the Ethernet connection on this port. Selecting Auto </p><p>(auto-negotiation) allows one port to negotiate with a peer port automatically to obtain the </p><p>connection speed and duplex mode that both ends support. When auto-negotiation is turned on, </p><p>a port on the Switch negotiates with the peer automatically to determine the connection speed </p><p>and duplex mode. If the peer port does not support auto-negotiation or turns off this feature, </p><p>the Switch determines the connection speed by detecting the signal on the cable and using half </p><p>duplex mode. Thus requiring you to make sure that the settings of the peer port are the same in </p><p>order to connect.</p> |SNMP |zyxel.3500_24s.net.if.speed_duplex[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Interface description |<p>MIB: ZYXEL-PORT-MIB</p><p>Descriptive name that identifies this port.</p> |SNMP |zyxel.3500_24s.net.if.name[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Link type |<p>MIB: ZYXEL-PORT-MIB</p><p>The entry shows the linkUp cable type (copper, fiber, xfp or cx4) for the combo ports.</p> |SNMP |zyxel.3500_24s.net.if.link_type[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Interface name |<p>MIB: IF-MIB</p><p>A textual string containing information about the interface</p> |SNMP |zyxel.3500_24s.net.if.descr[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Operational status |<p>MIB: IF-MIB</p><p>The current operational state of the interface.</p><p>The testing(3) state indicates that no operational</p><p>packets can be passed.</p> |SNMP |zyxel.3500_24s.net.if.operstatus[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Administrative status |<p>MIB: IF-MIB</p><p>The desired state of the interface.  The</p><p>testing(3) state indicates that no operational</p><p>packets can be passed.</p> |SNMP |zyxel.3500_24s.net.if.adminstatus[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Incoming traffic |<p>MIB: IF-MIB</p><p>The total number of octets received on the interface,</p><p>including framing characters.</p> |SNMP |zyxel.3500_24s.net.if.in.traffic[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `8`</p><p>- CHANGE_PER_SECOND |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Incoming unicast packages |<p>MIB: IF-MIB</p><p>The number of packets, delivered by this sub-layer to a</p><p>higher (sub-)layer, which were not addressed to a multicast</p><p>or broadcast address at this sub-layer</p> |SNMP |zyxel.3500_24s.net.if.in.ucastpkts[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Incoming multicast packages |<p>MIB: IF-MIB</p><p>The number of packets, delivered by this sub-layer to a</p><p>higher (sub-)layer, which were addressed to a multicast</p><p>address at this sub-layer.  For a MAC layer protocol, this</p><p>includes both Group and Functional addresses.</p> |SNMP |zyxel.3500_24s.net.if.in.multicastpkts[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Incoming broadcast packages |<p>MIB: IF-MIB</p><p>The number of packets, delivered by this sub-layer to a</p><p>higher (sub-)layer, which were addressed to a broadcast</p><p>address at this sub-layer.</p> |SNMP |zyxel.3500_24s.net.if.in.broadcastpkts[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Outgoing traffic |<p>MIB: IF-MIB</p><p>The total number of octets transmitted out of the</p><p>interface, including framing characters.  This object is a</p><p>64-bit version of ifOutOctets.</p> |SNMP |zyxel.3500_24s.net.if.out.traffic[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `8`</p><p>- CHANGE_PER_SECOND |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Outgoing unicast packages |<p>MIB: IF-MIB</p><p>The total number of packets that higher-level protocols</p><p>requested be transmitted, and which were not addressed to a</p><p>multicast or broadcast address at this sub-layer, including</p><p>those that were discarded or not sent.</p> |SNMP |zyxel.3500_24s.net.if.out.ucastpkts[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Outgoing multicast packages |<p>MIB: IF-MIB</p><p>The total number of packets that higher-level protocols</p><p>requested be transmitted, and which were addressed to a</p><p>multicast address at this sub-layer, including those that</p><p>were discarded or not sent.  For a MAC layer protocol, this</p><p>includes both Group and Functional addresses.</p> |SNMP |zyxel.3500_24s.net.if.out.multicastpkts[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Outgoing broadcast packages |<p>MIB: IF-MIB</p><p>The total number of packets that higher-level protocols</p><p>requested be transmitted, and which were addressed to a</p><p>broadcast address at this sub-layer, including those that</p><p>were discarded or not sent.</p> |SNMP |zyxel.3500_24s.net.if.out.broadcastpkts[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Link speed |<p>MIB: IF-MIB</p><p>An estimate of the interface's current bandwidth in bits per second</p> |SNMP |zyxel.3500_24s.net.if.highspeed[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `1000000`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Incoming utilization |<p>Interface utilization percentage</p> |CALCULATED |zyxel.3500_24s.net.if.in.util[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- IN_RANGE: `0 100`</p><p>⛔️ON_FAIL: `DISCARD_VALUE -> `</p><p>- JAVASCRIPT: `return +parseFloat(value).toFixed(0); `</p><p>**Expression**:</p>`last(zyxel.3500_24s.net.if.in.traffic[{#SNMPINDEX}]) * (last(zyxel.3500_24s.net.if.highspeed[{#SNMPINDEX}]) <> 0) /  ( last(zyxel.3500_24s.net.if.highspeed[{#SNMPINDEX}]) + (last(zyxel.3500_24s.net.if.highspeed[{#SNMPINDEX}]) = 0) ) * 100 ` |
|Network_interfaces |ZYXEL MES3500-24S: Port {#SNMPINDEX}: Outgoing utilization |<p>Interface utilization percentage</p> |CALCULATED |zyxel.3500_24s.net.if.out.util[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- IN_RANGE: `0 100`</p><p>⛔️ON_FAIL: `DISCARD_VALUE -> `</p><p>- JAVASCRIPT: `return +parseFloat(value).toFixed(0); `</p><p>**Expression**:</p>`last(zyxel.3500_24s.net.if.out.traffic[{#SNMPINDEX}]) * (last(zyxel.3500_24s.net.if.highspeed[{#SNMPINDEX}]) <> 0) /  ( last(zyxel.3500_24s.net.if.highspeed[{#SNMPINDEX}]) + (last(zyxel.3500_24s.net.if.highspeed[{#SNMPINDEX}]) = 0) ) * 100 ` |
|Network_interfaces |ZYXEL MES3500-24S: SFP {#SNMPINDEX}: Status |<p>MIB: ZYXEL-TRANSCEIVER-MIB</p><p>Transceiver module type.</p> |SNMP |zyxel.3500_24s.sfp.status[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Network_interfaces |ZYXEL MES3500-24S: SFP {#SNMPINDEX}: Vendor |<p>MIB: ZYXEL-TRANSCEIVER-MIB</p><p>Transceiver module vendor name.</p> |SNMP |zyxel.3500_24s.sfp.vendor[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Network_interfaces |ZYXEL MES3500-24S: SFP {#SNMPINDEX}: Part number |<p>MIB: ZYXEL-TRANSCEIVER-MIB</p><p>Part number provided by transceiver module vendor.</p> |SNMP |zyxel.3500_24s.sfp.part[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Network_interfaces |ZYXEL MES3500-24S: SFP {#SNMPINDEX}: Serial number |<p>MIB: ZYXEL-TRANSCEIVER-MIB</p><p>Serial number provided by transceiver module vendor.</p> |SNMP |zyxel.3500_24s.sfp.serialnumber[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Network_interfaces |ZYXEL MES3500-24S: SFP {#SNMPINDEX}: Revision |<p>MIB: ZYXEL-TRANSCEIVER-MIB</p><p>Revision level for part number provided by transceiver module vendor.</p> |SNMP |zyxel.3500_24s.sfp.revision[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Network_interfaces |ZYXEL MES3500-24S: SFP {#SNMPINDEX}: Date code |<p>MIB: ZYXEL-TRANSCEIVER-MIB</p><p>Transceiver module vendor's manufacturing date code.</p> |SNMP |zyxel.3500_24s.sfp.datecode[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Network_interfaces |ZYXEL MES3500-24S: SFP {#SNMPINDEX}: Transceiver |<p>MIB: ZYXEL-TRANSCEIVER-MIB</p><p>Transceiver module type names.</p> |SNMP |zyxel.3500_24s.sfp.transceiver[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Network_interfaces |ZYXEL MES3500-24S: SFP {#ZYXEL.SFP.PORT}: {#ZYXEL.SFP.DESCRIPTION} |<p>MIB: ZYXEL-TRANSCEIVER-MIB</p><p>Transceiver module DDM data ({#ZYXEL.SFP.DESCRIPTION}).</p> |SNMP |zyxel.3500_24s.sfp.ddm[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.01`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Power_supply |ZYXEL MES3500-24S: Nominal "{#ZYXEL.DESCRIPTION}" |<p>MIB: ZYXEL-HW-MONITOR-MIB</p><p>The current voltage reading.</p> |SNMP |zyxel.3500_24s.volt[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.001`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Status |ZYXEL MES3500-24S: SNMP agent availability |<p>-</p> |INTERNAL |zabbix[host,snmp,available]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Status |ZYXEL MES3500-24S: Uptime |<p>MIB: RFC1213-MIB</p><p>The time (in hundredths of a second) since the</p><p>network management portion of the system was last</p><p>re-initialized.</p> |SNMP |zyxel.3500_24s.uptime<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.01`</p> |
|Temperature |ZYXEL MES3500-24S: Temperature "{#ZYXEL.TEMPDESCRIPTION}" |<p>MIB: ZYXEL-HW-MONITOR-MIB</p><p>The current temperature measured at this sensor</p> |SNMP |zyxel.3500_24s.temp[{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |

## Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----|----|----|
|ZYXEL MES3500-24S: High CPU utilization (over {$CPU.UTIL.CRIT}% for 5m) |<p>CPU utilization is too high. The system might be slow to respond.</p> |`{TEMPLATE_NAME:zyxel.3500_24s.cpuusage.min(5m)}>{$CPU.UTIL.CRIT}` |WARNING | |
|ZYXEL MES3500-24S: {#ZYXEL.FAN.DESCRIPTION} is in critical state |<p>Please check the fan unit</p> |`{TEMPLATE_NAME:zyxel.3500_24s.fan[{#SNMPINDEX}].last()}<{#ZYXEL.FANRPM.THRESH.LOW}` |AVERAGE | |
|ZYXEL MES3500-24S: Template does not match hardware |<p>This template is for Zyxel MES3500-24S series, but connected to {ITEM.VALUE}</p> |`{TEMPLATE_NAME:zyxel.3500_24s.model.last()}<>"MES3500-24S"` |INFO | |
|ZYXEL MES3500-24S: Firmware has changed |<p>Firmware version has changed. Ack to close</p> |`{TEMPLATE_NAME:zyxel.3500_24s.fwversion.diff()}=1 and {TEMPLATE_NAME:zyxel.3500_24s.fwversion.strlen()}>0` |INFO |<p>Manual close: YES</p> |
|ZYXEL MES3500-24S: Device has been replaced (new serial number received) |<p>Device serial number has changed. Ack to close</p> |`{TEMPLATE_NAME:zyxel.3500_24s.serialnumber.diff()}=1 and {TEMPLATE_NAME:zyxel.3500_24s.serialnumber.strlen()}>0` |INFO |<p>Manual close: YES</p> |
|ZYXEL MES3500-24S: High memory utilization (>{$MEMORY.UTIL.MAX}% for 5m) |<p>The system is running out of free memory.</p> |`{TEMPLATE_NAME:zyxel.3500_24s.memusage.min(5m)}>{$MEMORY.UTIL.MAX}` |AVERAGE | |
|ZYXEL MES3500-24S: Port {#SNMPINDEX}: Link down |<p>This trigger expression works as follows:</p><p>1. Can be triggered if operations status is down.</p><p>2. {TEMPLATE_NAME:METRIC.diff()}=1) - trigger fires only if operational status was up(1) sometime before. (So, do not fire 'ethernal off' interfaces.)</p><p>WARNING: if closed manually - won't fire again on next poll, because of .diff.</p> |`{TEMPLATE_NAME:zyxel.3500_24s.net.if.operstatus[{#SNMPINDEX}].last()}=2 and {TEMPLATE_NAME:zyxel.3500_24s.net.if.operstatus[{#SNMPINDEX}].diff()}=1`<p>Recovery expression:</p>`{TEMPLATE_NAME:zyxel.3500_24s.net.if.operstatus[{#SNMPINDEX}].last()}<>2` |AVERAGE |<p>Manual close: YES</p> |
|ZYXEL MES3500-24S: SFP {#SNMPINDEX} has been replaced (new serial number received) |<p>SFP {#SNMPINDEX} serial number has changed. Ack to close</p> |`{TEMPLATE_NAME:zyxel.3500_24s.sfp.serialnumber[{#SNMPINDEX}].diff()}=1 and {TEMPLATE_NAME:zyxel.3500_24s.sfp.serialnumber[{#SNMPINDEX}].strlen()}>0` |INFO |<p>Manual close: YES</p> |
|ZYXEL MES3500-24S: SFP {#ZYXEL.SFP.PORT}: High {#ZYXEL.SFP.DESCRIPTION} (over {#ZYXEL.SFP.WARN.MAX}{#ZYXEL.SFP.UNIT}) |<p>The upper threshold value of the parameter is exceeded</p> |`{TEMPLATE_NAME:zyxel.3500_24s.sfp.ddm[{#SNMPINDEX}].last()} > {#ZYXEL.SFP.WARN.MAX}` |WARNING | |
|ZYXEL MES3500-24S: SFP {#ZYXEL.SFP.PORT}: Low {#ZYXEL.SFP.DESCRIPTION} (less than {#ZYXEL.SFP.WARN.MIN}{#ZYXEL.SFP.UNIT}) |<p>The parameter values are less than the lower threshold</p> |`{TEMPLATE_NAME:zyxel.3500_24s.sfp.ddm[{#SNMPINDEX}].last()} < {#ZYXEL.SFP.WARN.MIN}` |WARNING | |
|ZYXEL MES3500-24S: Voltage {#ZYXEL.DESCRIPTION} is in critical state |<p>Please check the power supply</p> |`{TEMPLATE_NAME:zyxel.3500_24s.volt[{#SNMPINDEX}].last()}<{#ZYXEL.VOLT.THRESH.LOW}` |AVERAGE | |
|ZYXEL MES3500-24S: No SNMP data collection |<p>SNMP is not available for polling. Please check device connectivity and SNMP settings.</p> |`{TEMPLATE_NAME:zabbix[host,snmp,available].max({$SNMP.TIMEOUT})}=0` |WARNING | |
|ZYXEL MES3500-24S: has been restarted (uptime < 10m) |<p>Uptime is less than 10 minutes</p> |`{TEMPLATE_NAME:zyxel.3500_24s.uptime.last()}<10m` |INFO |<p>Manual close: YES</p> |
|ZYXEL MES3500-24S: Temperature {#ZYXEL.TEMPDESCRIPTION} is in critical state |<p>Please check the temperature</p> |`{TEMPLATE_NAME:zyxel.3500_24s.temp[{#SNMPINDEX}].last()}>{#ZYXEL.TEMP.THRESH.HIGH}` |AVERAGE | |

## Feedback

Please report any issues with the template at https://support.zabbix.com

You can also provide a feedback, discuss the template or ask for help with it at [ZABBIX forums](https://www.zabbix.com/forum/zabbix-suggestions-and-feedback/422668-discussion-thread-for-official-zabbix-templates-for-zyxel).

## Known Issues

- Description: Incorrect handling of SNMP bulk requests. Disable the use of bulk requests in the SNMP interface settings.
  - Version: all versions firmware
  - Device: ZYXEL MGS3520-28

