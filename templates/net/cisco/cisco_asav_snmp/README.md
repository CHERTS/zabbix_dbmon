
# Cisco ASAv by SNMP

## Overview

Secure Firewall ASA Virtual is the virtualized option of popular Secure Firewall ASA solution and offers security in traditional physical data centers and private and public clouds.
Learn more about Cisco ASAv: https://www.cisco.com/c/en/us/products/collateral/security/adaptive-security-virtual-appliance-asav/adapt-security-virtual-appliance-ds.html

## Requirements

Zabbix version: 6.0 and higher.

## Tested versions

This template has been tested on:
- Cisco Adaptive Security Appliance Software Version 9.9(2), Device Manager Version 7.3(3)

## Configuration

> Zabbix should be configured according to the instructions in the [Templates out of the box](https://www.zabbix.com/documentation/6.0/manual/config/templates_out_of_the_box) section.

## Setup

Refer to the vendor documentation.

### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$SNMP.TIMEOUT}|<p>The time interval for SNMP agent availability trigger expression.</p>|`5m`|
|{$CISCO.LLD.FILTER.IF.NAME.MATCHES}|<p>Filter by discoverable interface names.</p>|`.*`|
|{$CISCO.LLD.FILTER.IF.NAME.NOT_MATCHES}|<p>Filter to exclude discovered interfaces by name.</p>|`CHANGE_IF_NEEDED`|
|{$CISCO.LLD.FILTER.IF.DESC.MATCHES}|<p>Filter by discoverable interface description.</p>|`.*`|
|{$CISCO.LLD.FILTER.IF.DESC.NOT_MATCHES}|<p>Filter to exclude discovered interfaces by description.</p>|`CHANGE_IF_NEEDED`|
|{$CISCO.LLD.FILTER.IF.ADMIN.MATCHES}|<p>Filter of discoverable interfaces by admin status.</p><p>1 - Up</p><p>2 - Down</p><p>3 - Testing</p>|`1`|
|{$CISCO.LLD.FILTER.IF.ADMIN.NOT_MATCHES}|<p>Filter to exclude discovered interfaces by admin status.</p>|`CHANGE_IF_NEEDED`|
|{$CISCO.LLD.FILTER.IF.CONTROL.MATCHES}|<p>Filter triggers by discoverable interface names.</p><p>Used in overrides. Triggers will only be created for interfaces whose names contain the value of the macro.</p>|`.*`|
|{$CPU.UTIL.CRIT}||`90`|
|{$MEMORY.UTIL.MAX}||`90`|

### Items

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Cisco ASAv: SNMP agent availability||Zabbix internal|zabbix[host,snmp,available]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Cisco ASAv: System description|<p>MIB: RFC1213-MIB</p><p>A textual description of the entity.  This value</p><p>should include the full name and version</p><p>identification of the system's hardware type,</p><p>software operating-system, and networking</p><p>software.  It is mandatory that this only contain</p><p>printable ASCII characters.</p>|SNMP agent|cisco.asav.model<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: Contact|<p>MIB: RFC1213-MIB</p><p>The textual identification of the contact person</p><p>for this managed node, together with information</p><p>on how to contact this person.</p>|SNMP agent|cisco.asav.contact<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: Host name|<p>MIB: RFC1213-MIB</p><p>An administratively-assigned name for this</p><p>managed node.  By convention, this is the node's</p><p>fully-qualified domain name.</p>|SNMP agent|cisco.asav.name<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: Location|<p>MIB: RFC1213-MIB</p><p>The physical location of this node (e.g.,</p><p>`telephone closet, 3rd floor').</p>|SNMP agent|cisco.asav.location<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: Uptime|<p>MIB: RFC1213-MIB</p><p>The time (in hundredths of a second) since the</p><p>network management portion of the system was last</p><p>re-initialized.</p>|SNMP agent|cisco.asav.uptime<p>**Preprocessing**</p><ul><li><p>Custom multiplier: `0.01`</p></li></ul>|

### Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Cisco ASAv: No SNMP data collection|<p>SNMP is not available for polling. Please check device connectivity and SNMP settings.</p>|`max(/Cisco ASAv by SNMP/zabbix[host,snmp,available],{$SNMP.TIMEOUT})=0`|Warning||
|Cisco ASAv: Host has been restarted|<p>Uptime is less than 10 minutes.</p>|`last(/Cisco ASAv by SNMP/cisco.asav.uptime)<10m`|Info|**Manual close**: Yes|

### LLD rule Physical entry discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Physical entry discovery|<p>Information about a particular physical entity.</p>|SNMP agent|cisco.asav.physical.entry.discovery|

### Item prototypes for Physical entry discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Cisco ASAv: {#CISCO.ASAV.PHYS.NAME} Physical description|<p>MIB: ENTITY-MIB</p><p>A textual description of physical entity.  This object</p><p>should contain a string that identifies the manufacturer's</p><p>name for the physical entity, and should be set to a</p><p>distinct value for each version or model of the physical</p><p>entity.</p>|SNMP agent|cisco.asav.phys.description[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.ASAV.PHYS.NAME} Physical class|<p>MIB: ENTITY-MIB</p><p>An indication of the general hardware type of the physical</p><p>entity.</p><p>An agent should set this object to the standard enumeration</p><p>value that most accurately indicates the general class of</p><p>the physical entity, or the primary class if there is more</p><p>than one entity.</p><p>If no appropriate standard registration identifier exists</p><p>for this physical entity, then the value 'other(1)' is</p><p>returned.  If the value is unknown by this agent, then the</p><p>value 'unknown(2)' is returned.</p>|SNMP agent|cisco.asav.phys.class[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.ASAV.PHYS.NAME} Physical name|<p>MIB: ENTITY-MIB</p><p>The textual name of the physical entity.  The value of this</p><p>object should be the name of the component as assigned by</p><p>the local device and should be suitable for use in commands</p><p>entered at the device's `console'.  This might be a text</p><p>name (e.g., `console') or a simple component number (e.g.,</p><p>port or module number, such as `1'), depending on the</p><p>physical component naming syntax of the device.</p><p>If there is no local name, or if this object is otherwise</p><p>not applicable, then this object contains a zero-length</p><p>string.</p><p>Note that the value of entPhysicalName for two physical</p><p>entities will be the same in the event that the console</p><p>interface does not distinguish between them, e.g., slot-1</p><p>and the card in slot-1.</p>|SNMP agent|cisco.asav.phys.name[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.ASAV.PHYS.NAME} Hardware revision|<p>MIB: ENTITY-MIB</p><p>The vendor-specific hardware revision string for the</p><p>physical entity.  The preferred value is the hardware</p><p>revision identifier actually printed on the component itself</p><p>(if present).</p><p>Note that if revision information is stored internally in a</p><p>non-printable (e.g., binary) format, then the agent must</p><p>convert such information to a printable format, in an</p><p>implementation-specific manner.</p><p>If no specific hardware revision string is associated with</p><p>the physical component, or if this information is unknown to</p><p>the agent, then this object will contain a zero-length</p><p>string.</p>|SNMP agent|cisco.asav.phys.hw[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.ASAV.PHYS.NAME} Software revision|<p>MIB: ENTITY-MIB</p><p>The vendor-specific software revision string for the</p><p>physical entity.</p><p>Note that if revision information is stored internally in a</p><p>non-printable (e.g., binary) format, then the agent must</p><p>convert such information to a printable format, in an</p><p>implementation-specific manner.</p><p>If no specific software programs are associated with the</p><p>physical component, or if this information is unknown to the</p><p>agent, then this object will contain a zero-length string.</p>|SNMP agent|cisco.asav.phys.sw[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.ASAV.PHYS.NAME} Serial number|<p>MIB: ENTITY-MIB</p><p>The vendor-specific serial number string for the physical</p><p>entity.  The preferred value is the serial number string</p><p>actually printed on the component itself (if present).</p><p>On the first instantiation of an physical entity, the value</p><p>of entPhysicalSerialNum associated with that entity is set</p><p>to the correct vendor-assigned serial number, if this</p><p>information is available to the agent.  If a serial number</p><p>is unknown or non-existent, the entPhysicalSerialNum will be</p><p>set to a zero-length string instead.</p>|SNMP agent|cisco.asav.phys.sn[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.ASAV.PHYS.NAME} Manufacturer name|<p>MIB: ENTITY-MIB</p><p>The name of the manufacturer of this physical component.</p><p>The preferred value is the manufacturer name string actually</p><p>printed on the component itself (if present).</p><p>Note that comparisons between instances of the</p><p>entPhysicalModelName, entPhysicalFirmwareRev,</p><p>entPhysicalSoftwareRev, and the entPhysicalSerialNum</p><p>objects, are only meaningful amongst entPhysicalEntries with</p><p>the same value of entPhysicalMfgName.</p><p>If the manufacturer name string associated with the physical</p><p>component is unknown to the agent, then this object will</p><p>contain a zero-length string.</p>|SNMP agent|cisco.asav.phys.mfgname[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.ASAV.PHYS.NAME} Model name|<p>MIB: ENTITY-MIB</p><p>The vendor-specific model name identifier string associated</p><p>with this physical component.  The preferred value is the</p><p>customer-visible part number, which may be printed on the</p><p>component itself.</p><p>If the model name string associated with the physical</p><p>component is unknown to the agent, then this object will</p><p>contain a zero-length string.</p>|SNMP agent|cisco.asav.phys.model[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|

### Trigger prototypes for Physical entry discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Cisco ASAv: {#CISCO.ASAV.PHYS.NAME} has been replaced|<p>{#CISCO.ASAV.PHYS.NAME} serial number has changed. Acknowledge to close the problem manually.</p>|`last(/Cisco ASAv by SNMP/cisco.asav.phys.sn[{#SNMPINDEX}],#1)<>last(/Cisco ASAv by SNMP/cisco.asav.phys.sn[{#SNMPINDEX}],#2) and length(last(/Cisco ASAv by SNMP/cisco.asav.phys.sn[{#SNMPINDEX}]))>0`|Info|**Manual close**: Yes|

### LLD rule Interface discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Interface discovery|<p>Network interfaces discovery</p>|SNMP agent|cisco.asav.net.if.discovery|

### Item prototypes for Interface discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Cisco ASAv: {#CISCO.IF.NAME} Interface name|<p>MIB: CISCO-PORT-MIB</p><p>Descriptive name that identifies this port.</p>|SNMP agent|cisco.asav.net.if.name[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.IF.NAME} Interface description|<p>MIB: IF-MIB</p><p>A textual string containing information about the interface</p>|SNMP agent|cisco.asav.net.if.descr[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.IF.NAME} Operational status|<p>MIB: IF-MIB</p><p>The current operational state of the interface.</p><p>The testing(3) state indicates that no operational</p><p>packets can be passed.</p>|SNMP agent|cisco.asav.net.if.operstatus[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.IF.NAME} Administrative status|<p>MIB: IF-MIB</p><p>The desired state of the interface.  The</p><p>testing(3) state indicates that no operational</p><p>packets can be passed.</p>|SNMP agent|cisco.asav.net.if.adminstatus[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.IF.NAME} Incoming traffic|<p>MIB: IF-MIB</p><p>The total number of octets received on the interface,</p><p>including framing characters.</p>|SNMP agent|cisco.asav.net.if.in.traffic[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Custom multiplier: `8`</p></li><li>Change per second</li></ul>|
|Cisco ASAv: {#CISCO.IF.NAME} Incoming unicast packets|<p>MIB: IF-MIB</p><p>The number of packets, delivered by this sub-layer to a</p><p>higher (sub-)layer, which were not addressed to a multicast</p><p>or broadcast address at this sub-layer</p>|SNMP agent|cisco.asav.net.if.in.ucastpkts[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li></ul>|
|Cisco ASAv: {#CISCO.IF.NAME} Incoming multicast packets|<p>MIB: IF-MIB</p><p>The number of packets, delivered by this sub-layer to a</p><p>higher (sub-)layer, which were addressed to a multicast</p><p>address at this sub-layer.  For a MAC layer protocol, this</p><p>includes both Group and Functional addresses.</p>|SNMP agent|cisco.asav.net.if.in.multicastpkts[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li></ul>|
|Cisco ASAv: {#CISCO.IF.NAME} Incoming broadcast packets|<p>MIB: IF-MIB</p><p>The number of packets, delivered by this sub-layer to a</p><p>higher (sub-)layer, which were addressed to a broadcast</p><p>address at this sub-layer.</p>|SNMP agent|cisco.asav.net.if.in.broadcastpkts[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li></ul>|
|Cisco ASAv: {#CISCO.IF.NAME} Outgoing traffic|<p>MIB: IF-MIB</p><p>The total number of octets transmitted out of the</p><p>interface, including framing characters.  This object is a</p><p>64-bit version of ifOutOctets.</p>|SNMP agent|cisco.asav.net.if.out.traffic[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Custom multiplier: `8`</p></li><li>Change per second</li></ul>|
|Cisco ASAv: {#CISCO.IF.NAME} Outgoing unicast packets|<p>MIB: IF-MIB</p><p>The total number of packets that higher-level protocols</p><p>requested be transmitted, and which were not addressed to a</p><p>multicast or broadcast address at this sub-layer, including</p><p>those that were discarded or not sent.</p>|SNMP agent|cisco.asav.net.if.out.ucastpkts[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li></ul>|
|Cisco ASAv: {#CISCO.IF.NAME} Outgoing multicast packets|<p>MIB: IF-MIB</p><p>The total number of packets that higher-level protocols</p><p>requested be transmitted, and which were addressed to a</p><p>multicast address at this sub-layer, including those that</p><p>were discarded or not sent.  For a MAC layer protocol, this</p><p>includes both Group and Functional addresses.</p>|SNMP agent|cisco.asav.net.if.out.multicastpkts[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li></ul>|
|Cisco ASAv: {#CISCO.IF.NAME} Outgoing broadcast packets|<p>MIB: IF-MIB</p><p>The total number of packets that higher-level protocols</p><p>requested be transmitted, and which were addressed to a</p><p>broadcast address at this sub-layer, including those that</p><p>were discarded or not sent.</p>|SNMP agent|cisco.asav.net.if.out.broadcastpkts[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li></ul>|
|Cisco ASAv: {#CISCO.IF.NAME} Link speed|<p>MIB: IF-MIB</p><p>An estimate of the interface's current bandwidth in bits per second</p>|SNMP agent|cisco.asav.net.if.highspeed[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Custom multiplier: `1000000`</p></li><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.IF.NAME} Incoming utilization|<p>Interface utilization percentage</p>|Calculated|cisco.asav.net.if.in.util[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>In range: `0 -> 100`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JavaScript: `return +parseFloat(value).toFixed(0);`</p></li></ul>|
|Cisco ASAv: {#CISCO.IF.NAME} Outgoing utilization|<p>Interface utilization percentage</p>|Calculated|cisco.asav.net.if.out.util[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>In range: `0 -> 100`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JavaScript: `return +parseFloat(value).toFixed(0);`</p></li></ul>|

### Trigger prototypes for Interface discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Cisco ASAv: {#CISCO.IF.NAME} Link down|<p>This trigger expression works as follows:<br>1. It can be triggered if the operations status is down.<br>2. {TEMPLATE_NAME:METRIC.diff()}=1) - trigger fires only if operational status was up(1) sometime before. (So, do not fire 'eternal off' interfaces.)<br><br>WARNING: if closed manually - won't fire again on next poll, because of .diff.</p>|`last(/Cisco ASAv by SNMP/cisco.asav.net.if.operstatus[{#SNMPINDEX}])=2 and last(/Cisco ASAv by SNMP/cisco.asav.net.if.operstatus[{#SNMPINDEX}],#1)<>last(/Cisco ASAv by SNMP/cisco.asav.net.if.operstatus[{#SNMPINDEX}],#2)`|Average|**Manual close**: Yes|

### LLD rule Memory discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Memory discovery|<p>Discovery of ciscoMemoryPoolTable, a table of memory pool monitoring entries.</p>|SNMP agent|cisco.asav.memory.discovery|

### Item prototypes for Memory discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Cisco ASAv: {#SNMPVALUE} Used memory|<p>MIB: CISCO-MEMORY-POOL-MIB</p><p>Indicates the number of bytes from the memory pool that are currently in use by applications on the managed device.</p><p>Reference: http://www.cisco.com/c/en/us/support/docs/ip/simple-network-management-protocol-snmp/15216-contiguous-memory.html</p>|SNMP agent|cisco.asav.memory.used[{#SNMPINDEX}]|
|Cisco ASAv: {#SNMPVALUE} Free memory|<p>MIB: CISCO-MEMORY-POOL-MIB</p><p>Indicates the number of bytes from the memory pool that are currently unused on the managed device. Note that the sum of ciscoMemoryPoolUsed and ciscoMemoryPoolFree is the total amount of memory in the pool</p><p>Reference: http://www.cisco.com/c/en/us/support/docs/ip/simple-network-management-protocol-snmp/15216-contiguous-memory.html</p>|SNMP agent|cisco.asav.memory.free[{#SNMPINDEX}]|
|Cisco ASAv: {#SNMPVALUE} Memory utilization|<p>Memory utilization in %.</p>|Calculated|cisco.asav.memory.util[{#SNMPINDEX}]|

### Trigger prototypes for Memory discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Cisco ASAv: High memory utilization|<p>The system is running out of free memory.</p>|`min(/Cisco ASAv by SNMP/cisco.asav.memory.util[{#SNMPINDEX}],5m)>{$MEMORY.UTIL.MAX}`|Average||

### LLD rule CPU discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|CPU discovery|<p>Discovery of cpmCPUTotalTable, a table of CPU monitoring entries.</p>|SNMP agent|cisco.asav.cpu.discovery|

### Item prototypes for CPU discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Cisco ASAv: CPU [{#SNMPINDEX}] Utilization|<p>MIB: CISCO-PROCESS-MIB</p><p>The overall CPU busy percentage in the last 5 minute</p><p>period. This object deprecates the object cpmCPUTotal5min</p><p>and increases the value range to (0..100).</p>|SNMP agent|cisco.asav.cpu.util[{#SNMPINDEX}]|

### Trigger prototypes for CPU discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Cisco ASAv: High CPU utilization|<p>The CPU utilization is too high. The system might be slow to respond.</p>|`min(/Cisco ASAv by SNMP/cisco.asav.cpu.util[{#SNMPINDEX}],5m)>{$CPU.UTIL.CRIT}`|Warning||

### LLD rule Session discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Session discovery|<p>Remote access session discovery</p>|SNMP agent|cisco.asav.session.discovery|

### Item prototypes for Session discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Cisco ASAv: {#CISCO.CRAS.USER} [{#CISCO.CRAS.INDEX}] Authenticate method|<p>MIB:  CISCO-REMOTE-ACCESS-MONITOR-MIB</p><p>The method used to authenticate the user prior to</p><p>establishing the session.</p>|SNMP agent|cisco.asav.session.authen.method[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.CRAS.USER} [{#CISCO.CRAS.INDEX}] Authorize method|<p>MIB:  CISCO-REMOTE-ACCESS-MONITOR-MIB</p><p>The method used to authorize the user prior to</p><p>establishing the session.</p>|SNMP agent|cisco.asav.session.author.method[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.CRAS.USER} [{#CISCO.CRAS.INDEX}] Session duration|<p>MIB:  CISCO-REMOTE-ACCESS-MONITOR-MIB</p><p>The number of seconds elapsed since this session</p><p>was established.</p>|SNMP agent|cisco.asav.session.duration[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Cisco ASAv: {#CISCO.CRAS.USER} [{#CISCO.CRAS.INDEX}] Local address|<p>MIB:  CISCO-REMOTE-ACCESS-MONITOR-MIB</p><p>The IP address assigned to the client of this session</p><p>in the private network assigned by the managed entity.</p>|SNMP agent|cisco.asav.session.addr.local[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.CRAS.USER} [{#CISCO.CRAS.INDEX}] ISP address|<p>MIB:  CISCO-REMOTE-ACCESS-MONITOR-MIB</p><p>The IP address of the peer (client) assigned by the ISP.</p><p>This is the address of the client device in the public</p><p>network.</p>|SNMP agent|cisco.asav.session.addr.isp[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.CRAS.USER} [{#CISCO.CRAS.INDEX}] Session protocol|<p>MIB:  CISCO-REMOTE-ACCESS-MONITOR-MIB</p><p>The protocol underlying this remote access session.</p>|SNMP agent|cisco.asav.session.protocol[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.CRAS.USER} [{#CISCO.CRAS.INDEX}] Encryption algorithm|<p>MIB:  CISCO-REMOTE-ACCESS-MONITOR-MIB</p><p>The algorithm used by this remote access session to</p><p>encrypt its payload.</p>|SNMP agent|cisco.asav.session.encryption[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.CRAS.USER} [{#CISCO.CRAS.INDEX}] Algorithm validate packets|<p>MIB:  CISCO-REMOTE-ACCESS-MONITOR-MIB</p><p>The algorithm used by this remote access session to</p><p>to validate packets.</p>|SNMP agent|cisco.asav.session.authen.algorithm[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `6h`</p></li></ul>|
|Cisco ASAv: {#CISCO.CRAS.USER} [{#CISCO.CRAS.INDEX}] Incoming traffic|<p>MIB: CISCO-REMOTE-ACCESS-MONITOR-MIB</p><p>The rate of octets received by this Remote</p><p>Access Session.</p><p>This value is accumulated BEFORE determining whether</p><p>or not the packet should be decompressed.</p>|SNMP agent|cisco.asav.session.in.traffic[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Custom multiplier: `8`</p></li><li>Change per second</li></ul>|
|Cisco ASAv: {#CISCO.CRAS.USER} [{#CISCO.CRAS.INDEX}] Outgoing traffic|<p>MIB: CISCO-REMOTE-ACCESS-MONITOR-MIB</p><p>The rate of octets transmitted by this Remote</p><p>Access Session.</p><p>This value is accumulated AFTER determining whether</p><p>or not the packet should be compressed.</p>|SNMP agent|cisco.asav.session.out.traffic[{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Custom multiplier: `8`</p></li><li>Change per second</li></ul>|

## Feedback

Please report any issues with the template at [`https://support.zabbix.com`](https://support.zabbix.com)

You can also provide feedback, discuss the template, or ask for help at [`ZABBIX forums`](https://www.zabbix.com/forum/zabbix-suggestions-and-feedback)

