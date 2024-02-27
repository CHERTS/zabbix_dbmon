
# TP-LINK by SNMP

## Overview

Link to MIBs: https://www.tp-link.com/en/support/download/t2600g-28ts/#MIBs_Files
Sample device overview page: https://www.tp-link.com/en/business-networking/managed-switch/t2600g-28ts/#overview
Emulation page (web): https://emulator.tp-link.com/T2600G-28TS(UN)_1.0/Index.htm

### Know issues

- Description: 'Default sysLocation, sysName and sysContact is not filled with proper data. Real hostname and location can be found only in private branch (TPLINK-SYSINFO-MIB). Please check whether this problem exists in the latest firmware: https://www.tp-link.com/en/support/download/t2600g-28ts/#Firmware'
  version: 2.0.0 Build 20170628 Rel.55184 (Beta)
  device: T2600G-28TS 2.0

- Description: The Serial number of the product (tpSysInfoSerialNum) is missing in HW versions prior to V2_170323
  version: Prior to version V2_170323
  device: T2600G-28TS 2.0


## Requirements

Zabbix version: 6.0 and higher.

## Tested versions

This template has been tested on:
- T2600G-28TS revision 2.0 2.0.0 Build 20170628 Rel.55184 (Beta)

## Configuration

> Zabbix should be configured according to the instructions in the [Templates out of the box](https://www.zabbix.com/documentation/6.0/manual/config/templates_out_of_the_box) section.

## Setup

Refer to the vendor documentation.

### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$MEMORY.UTIL.MAX}||`90`|
|{$CPU.UTIL.CRIT}||`90`|
|{$SNMP.TIMEOUT}||`5m`|
|{$ICMP_LOSS_WARN}||`20`|
|{$ICMP_RESPONSE_TIME_WARN}||`0.15`|
|{$IFCONTROL}||`1`|
|{$IF.UTIL.MAX}||`95`|
|{$NET.IF.IFNAME.MATCHES}||`^.*$`|
|{$NET.IF.IFNAME.NOT_MATCHES}|<p>Filter out loopbacks, nulls, docker veth links and docker0 bridge by default</p>|`Macro too long. Please see the template.`|
|{$NET.IF.IFOPERSTATUS.MATCHES}||`^.*$`|
|{$NET.IF.IFOPERSTATUS.NOT_MATCHES}|<p>Ignore notPresent(6)</p>|`^6$`|
|{$NET.IF.IFADMINSTATUS.MATCHES}||`^.*`|
|{$NET.IF.IFADMINSTATUS.NOT_MATCHES}|<p>Ignore down(2) administrative status</p>|`^2$`|
|{$NET.IF.IFDESCR.MATCHES}||`.*`|
|{$NET.IF.IFDESCR.NOT_MATCHES}||`CHANGE_IF_NEEDED`|
|{$NET.IF.IFTYPE.MATCHES}||`.*`|
|{$NET.IF.IFTYPE.NOT_MATCHES}||`CHANGE_IF_NEEDED`|
|{$IF.ERRORS.WARN}||`2`|

### Items

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|TP-LINK: Hardware model name|<p>MIB: TPLINK-SYSINFO-MIB</p><p>The hardware version of the product.</p>|SNMP agent|system.hw.model<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|TP-LINK: Hardware serial number|<p>MIB: TPLINK-SYSINFO-MIB</p><p>The Serial number of the product.</p>|SNMP agent|system.hw.serialnumber<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|TP-LINK: Firmware version|<p>MIB: TPLINK-SYSINFO-MIB</p><p>The software version of the product.</p>|SNMP agent|system.hw.firmware<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|TP-LINK: Hardware version(revision)|<p>MIB: TPLINK-SYSINFO-MIB</p><p>The hardware version of the product.</p>|SNMP agent|system.hw.version<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|TP-LINK: Uptime (network)|<p>MIB: SNMPv2-MIB</p><p>The time (in hundredths of a second) since the network management portion of the system was last re-initialized.</p>|SNMP agent|system.net.uptime[sysUpTime.0]<p>**Preprocessing**</p><ul><li><p>Custom multiplier: `0.01`</p></li></ul>|
|TP-LINK: Uptime (hardware)|<p>MIB: HOST-RESOURCES-MIB</p><p>The amount of time since this host was last initialized. Note that this is different from sysUpTime in the SNMPv2-MIB [RFC1907] because sysUpTime is the uptime of the network management portion of the system.</p>|SNMP agent|system.hw.uptime[hrSystemUptime.0]<p>**Preprocessing**</p><ul><li><p>Check for not supported value</p><p>⛔️Custom on fail: Set value to: `0`</p></li><li><p>Custom multiplier: `0.01`</p></li></ul>|
|TP-LINK: SNMP traps (fallback)|<p>The item is used to collect all SNMP traps unmatched by other snmptrap items</p>|SNMP trap|snmptrap.fallback|
|TP-LINK: System location|<p>MIB: SNMPv2-MIB</p><p>The physical location of this node (e.g., `telephone closet, 3rd floor').  If the location is unknown, the value is the zero-length string.</p>|SNMP agent|system.location[sysLocation.0]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `12h`</p></li></ul>|
|TP-LINK: System contact details|<p>MIB: SNMPv2-MIB</p><p>The textual identification of the contact person for this managed node, together with information on how to contact this person.  If no contact information is known, the value is the zero-length string.</p>|SNMP agent|system.contact[sysContact.0]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `12h`</p></li></ul>|
|TP-LINK: System object ID|<p>MIB: SNMPv2-MIB</p><p>The vendor's authoritative identification of the network management subsystem contained in the entity.  This value is allocated within the SMI enterprises subtree (1.3.6.1.4.1) and provides an easy and unambiguous means for determining`what kind of box' is being managed.  For example, if vendor`Flintstones, Inc.' was assigned the subtree1.3.6.1.4.1.4242, it could assign the identifier 1.3.6.1.4.1.4242.1.1 to its `Fred Router'.</p>|SNMP agent|system.objectid[sysObjectID.0]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `12h`</p></li></ul>|
|TP-LINK: System name|<p>MIB: SNMPv2-MIB</p><p>An administratively-assigned name for this managed node.By convention, this is the node's fully-qualified domain name.  If the name is unknown, the value is the zero-length string.</p>|SNMP agent|system.name<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `12h`</p></li></ul>|
|TP-LINK: System description|<p>MIB: SNMPv2-MIB</p><p>A textual description of the entity. This value should</p><p>include the full name and version identification of the system's hardware type, software operating-system, and</p><p>networking software.</p>|SNMP agent|system.descr[sysDescr.0]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `12h`</p></li></ul>|
|TP-LINK: SNMP agent availability|<p>Availability of SNMP checks on the host. The value of this item corresponds to availability icons in the host list.</p><p>Possible value:</p><p>0 - not available</p><p>1 - available</p><p>2 - unknown</p>|Zabbix internal|zabbix[host,snmp,available]|
|TP-LINK: ICMP ping||Simple check|icmpping|
|TP-LINK: ICMP loss||Simple check|icmppingloss|
|TP-LINK: ICMP response time||Simple check|icmppingsec|

### Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|TP-LINK: Device has been replaced|<p>Device serial number has changed. Acknowledge to close the problem manually.</p>|`last(/TP-LINK by SNMP/system.hw.serialnumber,#1)<>last(/TP-LINK by SNMP/system.hw.serialnumber,#2) and length(last(/TP-LINK by SNMP/system.hw.serialnumber))>0`|Info|**Manual close**: Yes|
|TP-LINK: Firmware has changed|<p>Firmware version has changed. Acknowledge to close the problem manually.</p>|`last(/TP-LINK by SNMP/system.hw.firmware,#1)<>last(/TP-LINK by SNMP/system.hw.firmware,#2) and length(last(/TP-LINK by SNMP/system.hw.firmware))>0`|Info|**Manual close**: Yes|
|TP-LINK: Host has been restarted|<p>Uptime is less than 10 minutes.</p>|`(last(/TP-LINK by SNMP/system.hw.uptime[hrSystemUptime.0])>0 and last(/TP-LINK by SNMP/system.hw.uptime[hrSystemUptime.0])<10m) or (last(/TP-LINK by SNMP/system.hw.uptime[hrSystemUptime.0])=0 and last(/TP-LINK by SNMP/system.net.uptime[sysUpTime.0])<10m)`|Warning|**Manual close**: Yes<br>**Depends on**:<br><ul><li>TP-LINK: No SNMP data collection</li></ul>|
|TP-LINK: System name has changed|<p>The name of the system has changed. Acknowledge to close the problem manually.</p>|`last(/TP-LINK by SNMP/system.name,#1)<>last(/TP-LINK by SNMP/system.name,#2) and length(last(/TP-LINK by SNMP/system.name))>0`|Info|**Manual close**: Yes|
|TP-LINK: No SNMP data collection|<p>SNMP is not available for polling. Please check device connectivity and SNMP settings.</p>|`max(/TP-LINK by SNMP/zabbix[host,snmp,available],{$SNMP.TIMEOUT})=0`|Warning|**Depends on**:<br><ul><li>TP-LINK: Unavailable by ICMP ping</li></ul>|
|TP-LINK: Unavailable by ICMP ping|<p>Last three attempts returned timeout.  Please check device connectivity.</p>|`max(/TP-LINK by SNMP/icmpping,#3)=0`|High||
|TP-LINK: High ICMP ping loss||`min(/TP-LINK by SNMP/icmppingloss,5m)>{$ICMP_LOSS_WARN} and min(/TP-LINK by SNMP/icmppingloss,5m)<100`|Warning|**Depends on**:<br><ul><li>TP-LINK: Unavailable by ICMP ping</li></ul>|
|TP-LINK: High ICMP ping response time||`avg(/TP-LINK by SNMP/icmppingsec,5m)>{$ICMP_RESPONSE_TIME_WARN}`|Warning|**Depends on**:<br><ul><li>TP-LINK: High ICMP ping loss</li><li>TP-LINK: Unavailable by ICMP ping</li></ul>|

### LLD rule CPU Discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|CPU Discovery|<p>Discovering TPLINK-SYSMONITOR-MIB::tpSysMonitorCpuTable, displays the CPU utilization of all UNITs.</p>|SNMP agent|cpu.discovery|

### Item prototypes for CPU Discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|#{#SNMPVALUE}: CPU utilization|<p>MIB: TPLINK-SYSMONITOR-MIB</p><p>Displays the CPU utilization in 1 minute.</p><p>Reference: http://www.tp-link.com/faq-1330.html</p>|SNMP agent|system.cpu.util[tpSysMonitorCpu1Minute.{#SNMPINDEX}]|

### Trigger prototypes for CPU Discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|#{#SNMPVALUE}: High CPU utilization|<p>The CPU utilization is too high. The system might be slow to respond.</p>|`min(/TP-LINK by SNMP/system.cpu.util[tpSysMonitorCpu1Minute.{#SNMPINDEX}],5m)>{$CPU.UTIL.CRIT}`|Warning||

### LLD rule Memory Discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Memory Discovery|<p>Discovering TPLINK-SYSMONITOR-MIB::tpSysMonitorMemoryTable, displays the memory utilization of all UNITs.</p>|SNMP agent|memory.discovery|

### Item prototypes for Memory Discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|#{#SNMPVALUE}: Memory utilization|<p>MIB: TPLINK-SYSMONITOR-MIB</p><p>Displays the memory utilization.</p><p>Reference: http://www.tp-link.com/faq-1330.html</p>|SNMP agent|vm.memory.util[tpSysMonitorMemoryUtilization.{#SNMPINDEX}]|

### Trigger prototypes for Memory Discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|#{#SNMPVALUE}: High memory utilization|<p>The system is running out of free memory.</p>|`min(/TP-LINK by SNMP/vm.memory.util[tpSysMonitorMemoryUtilization.{#SNMPINDEX}],5m)>{$MEMORY.UTIL.MAX}`|Average||

### LLD rule Network interfaces discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Network interfaces discovery|<p>Discovering interfaces from IF-MIB.</p>|SNMP agent|net.if.discovery|

### Item prototypes for Network interfaces discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Interface {#IFDESCR}: Operational status|<p>MIB: IF-MIB</p><p>The current operational state of the interface.</p><p>- The testing(3) state indicates that no operational packet scan be passed</p><p>- If ifAdminStatus is down(2) then ifOperStatus should be down(2)</p><p>- If ifAdminStatus is changed to up(1) then ifOperStatus should change to up(1) if the interface is ready to transmit and receive network traffic</p><p>- It should change todormant(5) if the interface is waiting for external actions (such as a serial line waiting for an incoming connection)</p><p>- It should remain in the down(2) state if and only if there is a fault that prevents it from going to the up(1) state</p><p>- It should remain in the notPresent(6) state if the interface has missing(typically, hardware) components.</p>|SNMP agent|net.if.status[ifOperStatus.{#SNMPINDEX}]|
|Interface {#IFDESCR}: Bits received|<p>MIB: IF-MIB</p><p>The total number of octets received on the interface,including framing characters. Discontinuities in the value of this counter can occur at re-initialization of the management system, and another times as indicated by the value of ifCounterDiscontinuityTime.</p>|SNMP agent|net.if.in[ifInOctets.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li><li><p>Custom multiplier: `8`</p></li></ul>|
|Interface {#IFDESCR}: Bits sent|<p>MIB: IF-MIB</p><p>The total number of octets transmitted out of the interface, including framing characters. Discontinuities in the value of this counter can occur at re-initialization of the management system, and at other times as indicated by the value of ifCounterDiscontinuityTime.</p>|SNMP agent|net.if.out[ifOutOctets.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li><li><p>Custom multiplier: `8`</p></li></ul>|
|Interface {#IFDESCR}: Inbound packets with errors|<p>MIB: IF-MIB</p><p>For packet-oriented interfaces, the number of inbound packets that contained errors preventing them from being deliverable to a higher-layer protocol.  For character-oriented or fixed-length interfaces, the number of inbound transmission units that contained errors preventing them from being deliverable to a higher-layer protocol. Discontinuities in the value of this counter can occur at re-initialization of the management system, and at other times as indicated by the value of ifCounterDiscontinuityTime.</p>|SNMP agent|net.if.in.errors[ifInErrors.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li></ul>|
|Interface {#IFDESCR}: Outbound packets with errors|<p>MIB: IF-MIB</p><p>For packet-oriented interfaces, the number of outbound packets that contained errors preventing them from being deliverable to a higher-layer protocol.  For character-oriented or fixed-length interfaces, the number of outbound transmission units that contained errors preventing them from being deliverable to a higher-layer protocol. Discontinuities in the value of this counter can occur at re-initialization of the management system, and at other times as indicated by the value of ifCounterDiscontinuityTime.</p>|SNMP agent|net.if.out.errors[ifOutErrors.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li></ul>|
|Interface {#IFDESCR}: Outbound packets discarded|<p>MIB: IF-MIB</p><p>The number of outbound packets which were chosen to be discarded</p><p>even though no errors had been detected to prevent their being deliverable to a higher-layer protocol.</p><p>One possible reason for discarding such a packet could be to free up buffer space.</p><p>Discontinuities in the value of this counter can occur at re-initialization of the management system,</p><p>and at other times as indicated by the value of ifCounterDiscontinuityTime.</p>|SNMP agent|net.if.out.discards[ifOutDiscards.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li></ul>|
|Interface {#IFDESCR}: Inbound packets discarded|<p>MIB: IF-MIB</p><p>The number of inbound packets which were chosen to be discarded</p><p>even though no errors had been detected to prevent their being deliverable to a higher-layer protocol.</p><p>One possible reason for discarding such a packet could be to free up buffer space.</p><p>Discontinuities in the value of this counter can occur at re-initialization of the management system,</p><p>and at other times as indicated by the value of ifCounterDiscontinuityTime.</p>|SNMP agent|net.if.in.discards[ifInDiscards.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li></ul>|
|Interface {#IFDESCR}: Interface type|<p>MIB: IF-MIB</p><p>The type of interface.</p><p>Additional values for ifType are assigned by the Internet Assigned Numbers Authority (IANA),</p><p>through updating the syntax of the IANAifType textual convention.</p>|SNMP agent|net.if.type[ifType.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|Interface {#IFDESCR}: Speed|<p>MIB: IF-MIB</p><p>An estimate of the interface's current bandwidth in bits per second.</p><p>For interfaces which do not vary in bandwidth or for those where no accurate estimation can be made,</p><p>this object should contain the nominal bandwidth.</p><p>If the bandwidth of the interface is greater than the maximum value reportable by this object then</p><p>this object should report its maximum value (4,294,967,295) and ifHighSpeed must be used to report the interface's speed.</p><p>For a sub-layer which has no concept of bandwidth, this object should be zero.</p>|SNMP agent|net.if.speed[ifSpeed.{#SNMPINDEX}]|

### Trigger prototypes for Network interfaces discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Interface {#IFDESCR}: Link down|<p>This trigger expression works as follows:<br>1. It can be triggered if the operations status is down.<br>2. `{$IFCONTROL:"{#IFNAME}"}=1` - a user can redefine context macro to value - 0. That marks this interface as not important. No new trigger will be fired if this interface is down.<br>3. `{TEMPLATE_NAME:METRIC.diff()}=1` - the trigger fires only if the operational status was up to (1) sometime before (so, do not fire for the 'eternal off' interfaces.)<br><br>WARNING: if closed manually - it will not fire again on the next poll, because of .diff.</p>|`{$IFCONTROL:"{#IFNAME}"}=1 and last(/TP-LINK by SNMP/net.if.status[ifOperStatus.{#SNMPINDEX}])=2 and (last(/TP-LINK by SNMP/net.if.status[ifOperStatus.{#SNMPINDEX}],#1)<>last(/TP-LINK by SNMP/net.if.status[ifOperStatus.{#SNMPINDEX}],#2))`|Average|**Manual close**: Yes|
|Interface {#IFDESCR}: High bandwidth usage|<p>The utilization of the network interface is close to its estimated maximum bandwidth.</p>|`(avg(/TP-LINK by SNMP/net.if.in[ifInOctets.{#SNMPINDEX}],15m)>({$IF.UTIL.MAX:"{#IFNAME}"}/100)*last(/TP-LINK by SNMP/net.if.speed[ifSpeed.{#SNMPINDEX}]) or avg(/TP-LINK by SNMP/net.if.out[ifOutOctets.{#SNMPINDEX}],15m)>({$IF.UTIL.MAX:"{#IFNAME}"}/100)*last(/TP-LINK by SNMP/net.if.speed[ifSpeed.{#SNMPINDEX}])) and last(/TP-LINK by SNMP/net.if.speed[ifSpeed.{#SNMPINDEX}])>0`|Warning|**Manual close**: Yes<br>**Depends on**:<br><ul><li>Interface {#IFDESCR}: Link down</li></ul>|
|Interface {#IFDESCR}: High error rate|<p>It recovers when it is below 80% of the `{$IF.ERRORS.WARN:"{#IFNAME}"}` threshold.</p>|`min(/TP-LINK by SNMP/net.if.in.errors[ifInErrors.{#SNMPINDEX}],5m)>{$IF.ERRORS.WARN:"{#IFNAME}"} or min(/TP-LINK by SNMP/net.if.out.errors[ifOutErrors.{#SNMPINDEX}],5m)>{$IF.ERRORS.WARN:"{#IFNAME}"}`|Warning|**Manual close**: Yes<br>**Depends on**:<br><ul><li>Interface {#IFDESCR}: Link down</li></ul>|
|Interface {#IFDESCR}: Ethernet has changed to lower speed than it was before|<p>This Ethernet connection has transitioned down from its known maximum speed. This might be a sign of autonegotiation issues. Acknowledge to close the problem manually.</p>|`change(/TP-LINK by SNMP/net.if.speed[ifSpeed.{#SNMPINDEX}])<0 and last(/TP-LINK by SNMP/net.if.speed[ifSpeed.{#SNMPINDEX}])>0 and ( last(/TP-LINK by SNMP/net.if.type[ifType.{#SNMPINDEX}])=6 or last(/TP-LINK by SNMP/net.if.type[ifType.{#SNMPINDEX}])=7 or last(/TP-LINK by SNMP/net.if.type[ifType.{#SNMPINDEX}])=11 or last(/TP-LINK by SNMP/net.if.type[ifType.{#SNMPINDEX}])=62 or last(/TP-LINK by SNMP/net.if.type[ifType.{#SNMPINDEX}])=69 or last(/TP-LINK by SNMP/net.if.type[ifType.{#SNMPINDEX}])=117 ) and (last(/TP-LINK by SNMP/net.if.status[ifOperStatus.{#SNMPINDEX}])<>2)`|Info|**Manual close**: Yes<br>**Depends on**:<br><ul><li>Interface {#IFDESCR}: Link down</li></ul>|

## Feedback

Please report any issues with the template at [`https://support.zabbix.com`](https://support.zabbix.com)

You can also provide feedback, discuss the template, or ask for help at [`ZABBIX forums`](https://www.zabbix.com/forum/zabbix-suggestions-and-feedback)

