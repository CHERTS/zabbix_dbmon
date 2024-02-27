
# MikroTik CRS305-1G-4SIN by SNMP

## Overview

The template for monitoring Switch MikroTik CRS305-1G-4S+IN.

Five-port desktop switch with one Gigabit Ethernet port and four SFP+ 10Gbps ports

## Requirements

Zabbix version: 6.0 and higher.

## Tested versions

This template has been tested on:
- MikroTik CRS305-1G-4S+IN

## Configuration

> Zabbix should be configured according to the instructions in the [Templates out of the box](https://www.zabbix.com/documentation/6.0/manual/config/templates_out_of_the_box) section.

## Setup

Refer to the vendor documentation.

### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$VFS.FS.PUSED.MAX.CRIT}||`90`|
|{$VFS.FS.PUSED.MAX.WARN}||`80`|
|{$CPU.UTIL.CRIT}||`90`|
|{$TEMP_CRIT}||`60`|
|{$TEMP_CRIT_LOW}||`5`|
|{$TEMP_WARN}||`50`|
|{$TEMP_CRIT:"CPU"}||`75`|
|{$TEMP_WARN:"CPU"}||`70`|
|{$MEMORY.UTIL.MAX}||`90`|
|{$IFNAME.WIFI.MATCHES}|<p>This macro is used in CAPsMAN AP channel discovery. It can be overridden on the host level.</p>|`WIFI`|
|{$IFNAME.LTEMODEM.MATCHES}|<p>This macro is used in LTE modem discovery. It can be overridden on the host.</p>|`^lte`|
|{$LTEMODEM.RSSI.MIN.WARN}|<p>The LTE modem RSSI minimum value for warning trigger expression.</p>|`-100`|
|{$LTEMODEM.RSRP.MIN.WARN}|<p>The LTE modem RSRP minimum value for warning trigger expression.</p>|`-100`|
|{$LTEMODEM.RSRQ.MIN.WARN}|<p>The LTE modem RSRQ minimum value for warning trigger expression.</p>|`-20`|
|{$LTEMODEM.SINR.MIN.WARN}|<p>The LTE modem SINR minimum value for warning trigger expression.</p>|`0`|
|{$SNMP.TIMEOUT}||`5m`|
|{$ICMP_LOSS_WARN}||`20`|
|{$ICMP_RESPONSE_TIME_WARN}||`0.15`|
|{$IF.ERRORS.WARN}||`2`|
|{$IF.UTIL.MAX}||`90`|
|{$IFCONTROL}||`1`|
|{$NET.IF.IFNAME.MATCHES}||`^.*$`|
|{$NET.IF.IFNAME.NOT_MATCHES}|<p>Filter out loopbacks, nulls, docker veth links and docker0 bridge by default</p>|`Macro too long. Please see the template.`|
|{$NET.IF.IFOPERSTATUS.MATCHES}||`^.*$`|
|{$NET.IF.IFOPERSTATUS.NOT_MATCHES}|<p>Ignore notPresent(6)</p>|`^6$`|
|{$NET.IF.IFADMINSTATUS.MATCHES}||`^.*`|
|{$NET.IF.IFADMINSTATUS.NOT_MATCHES}|<p>Ignore down(2) administrative status</p>|`^2$`|
|{$NET.IF.IFDESCR.MATCHES}||`.*`|
|{$NET.IF.IFDESCR.NOT_MATCHES}||`CHANGE_IF_NEEDED`|
|{$NET.IF.IFALIAS.MATCHES}||`.*`|
|{$NET.IF.IFALIAS.NOT_MATCHES}||`CHANGE_IF_NEEDED`|
|{$NET.IF.IFTYPE.MATCHES}||`.*`|
|{$NET.IF.IFTYPE.NOT_MATCHES}||`CHANGE_IF_NEEDED`|

### Items

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Mikrotik: Used memory|<p>MIB: HOST-RESOURCES-MIB</p><p>The amount of the storage represented by this entry that is allocated, in units of hrStorageAllocationUnits.</p>|SNMP agent|vm.memory.used[hrStorageUsed.Memory]<p>**Preprocessing**</p><ul><li><p>Custom multiplier: `1024`</p></li></ul>|
|Mikrotik: Total memory|<p>MIB: HOST-RESOURCES-MIB</p><p>The size of the storage represented by this entry, in</p><p>units of hrStorageAllocationUnits. This object is</p><p>writable to allow remote configuration of the size of</p><p>the storage area in those cases where such an</p><p>operation makes sense and is possible on the</p><p>underlying system. For example, the amount of main</p><p>memory allocated to a buffer pool might be modified or</p><p>the amount of disk space allocated to virtual memory</p><p>might be modified.</p>|SNMP agent|vm.memory.total[hrStorageSize.Memory]<p>**Preprocessing**</p><ul><li><p>Custom multiplier: `1024`</p></li></ul>|
|Mikrotik: Memory utilization|<p>Memory utilization in %.</p>|Calculated|vm.memory.util[memoryUsedPercentage.Memory]|
|Mikrotik: Operating system|<p>MIB: MIKROTIK-MIB</p><p>Software version.</p>|SNMP agent|system.sw.os[mtxrLicVersion.0]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|Mikrotik: Hardware model name||SNMP agent|system.hw.model<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|Mikrotik: Hardware serial number|<p>MIB: MIKROTIK-MIB</p><p>RouterBOARD serial number.</p>|SNMP agent|system.hw.serialnumber<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|Mikrotik: Firmware version|<p>MIB: MIKROTIK-MIB</p><p>Current firmware version.</p>|SNMP agent|system.hw.firmware<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|Mikrotik: Uptime (network)|<p>MIB: SNMPv2-MIB</p><p>The time (in hundredths of a second) since the network management portion of the system was last re-initialized.</p>|SNMP agent|system.net.uptime[sysUpTime.0]<p>**Preprocessing**</p><ul><li><p>Custom multiplier: `0.01`</p></li></ul>|
|Mikrotik: Uptime (hardware)|<p>MIB: HOST-RESOURCES-MIB</p><p>The amount of time since this host was last initialized. Note that this is different from sysUpTime in the SNMPv2-MIB [RFC1907] because sysUpTime is the uptime of the network management portion of the system.</p>|SNMP agent|system.hw.uptime[hrSystemUptime.0]<p>**Preprocessing**</p><ul><li><p>Check for not supported value</p><p>⛔️Custom on fail: Set value to: `0`</p></li><li><p>Custom multiplier: `0.01`</p></li></ul>|
|Mikrotik: SNMP traps (fallback)|<p>The item is used to collect all SNMP traps unmatched by other snmptrap items</p>|SNMP trap|snmptrap.fallback|
|Mikrotik: System location|<p>MIB: SNMPv2-MIB</p><p>The physical location of this node (e.g., `telephone closet, 3rd floor').  If the location is unknown, the value is the zero-length string.</p>|SNMP agent|system.location[sysLocation.0]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `12h`</p></li></ul>|
|Mikrotik: System contact details|<p>MIB: SNMPv2-MIB</p><p>The textual identification of the contact person for this managed node, together with information on how to contact this person.  If no contact information is known, the value is the zero-length string.</p>|SNMP agent|system.contact[sysContact.0]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `12h`</p></li></ul>|
|Mikrotik: System object ID|<p>MIB: SNMPv2-MIB</p><p>The vendor's authoritative identification of the network management subsystem contained in the entity.  This value is allocated within the SMI enterprises subtree (1.3.6.1.4.1) and provides an easy and unambiguous means for determining`what kind of box' is being managed.  For example, if vendor`Flintstones, Inc.' was assigned the subtree1.3.6.1.4.1.4242, it could assign the identifier 1.3.6.1.4.1.4242.1.1 to its `Fred Router'.</p>|SNMP agent|system.objectid[sysObjectID.0]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `12h`</p></li></ul>|
|Mikrotik: System name|<p>MIB: SNMPv2-MIB</p><p>An administratively-assigned name for this managed node.By convention, this is the node's fully-qualified domain name.  If the name is unknown, the value is the zero-length string.</p>|SNMP agent|system.name<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `12h`</p></li></ul>|
|Mikrotik: System description|<p>MIB: SNMPv2-MIB</p><p>A textual description of the entity. This value should</p><p>include the full name and version identification of the system's hardware type, software operating-system, and</p><p>networking software.</p>|SNMP agent|system.descr[sysDescr.0]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `12h`</p></li></ul>|
|Mikrotik: SNMP agent availability|<p>Availability of SNMP checks on the host. The value of this item corresponds to availability icons in the host list.</p><p>Possible value:</p><p>0 - not available</p><p>1 - available</p><p>2 - unknown</p>|Zabbix internal|zabbix[host,snmp,available]|
|Mikrotik: ICMP ping||Simple check|icmpping|
|Mikrotik: ICMP loss||Simple check|icmppingloss|
|Mikrotik: ICMP response time||Simple check|icmppingsec|

### Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Mikrotik: High memory utilization|<p>The system is running out of free memory.</p>|`min(/MikroTik CRS305-1G-4SIN by SNMP/vm.memory.util[memoryUsedPercentage.Memory],5m)>{$MEMORY.UTIL.MAX}`|Average||
|Mikrotik: Operating system description has changed|<p>The description of the operating system has changed. Possible reasons are that the system has been updated or replaced. Acknowledge to close the problem manually.</p>|`last(/MikroTik CRS305-1G-4SIN by SNMP/system.sw.os[mtxrLicVersion.0],#1)<>last(/MikroTik CRS305-1G-4SIN by SNMP/system.sw.os[mtxrLicVersion.0],#2) and length(last(/MikroTik CRS305-1G-4SIN by SNMP/system.sw.os[mtxrLicVersion.0]))>0`|Info|**Manual close**: Yes<br>**Depends on**:<br><ul><li>Mikrotik: System name has changed</li></ul>|
|Mikrotik: Device has been replaced|<p>Device serial number has changed. Acknowledge to close the problem manually.</p>|`last(/MikroTik CRS305-1G-4SIN by SNMP/system.hw.serialnumber,#1)<>last(/MikroTik CRS305-1G-4SIN by SNMP/system.hw.serialnumber,#2) and length(last(/MikroTik CRS305-1G-4SIN by SNMP/system.hw.serialnumber))>0`|Info|**Manual close**: Yes|
|Mikrotik: Firmware has changed|<p>Firmware version has changed. Acknowledge to close the problem manually.</p>|`last(/MikroTik CRS305-1G-4SIN by SNMP/system.hw.firmware,#1)<>last(/MikroTik CRS305-1G-4SIN by SNMP/system.hw.firmware,#2) and length(last(/MikroTik CRS305-1G-4SIN by SNMP/system.hw.firmware))>0`|Info|**Manual close**: Yes|
|Mikrotik: Host has been restarted|<p>Uptime is less than 10 minutes.</p>|`(last(/MikroTik CRS305-1G-4SIN by SNMP/system.hw.uptime[hrSystemUptime.0])>0 and last(/MikroTik CRS305-1G-4SIN by SNMP/system.hw.uptime[hrSystemUptime.0])<10m) or (last(/MikroTik CRS305-1G-4SIN by SNMP/system.hw.uptime[hrSystemUptime.0])=0 and last(/MikroTik CRS305-1G-4SIN by SNMP/system.net.uptime[sysUpTime.0])<10m)`|Warning|**Manual close**: Yes<br>**Depends on**:<br><ul><li>Mikrotik: No SNMP data collection</li></ul>|
|Mikrotik: System name has changed|<p>The name of the system has changed. Acknowledge to close the problem manually.</p>|`last(/MikroTik CRS305-1G-4SIN by SNMP/system.name,#1)<>last(/MikroTik CRS305-1G-4SIN by SNMP/system.name,#2) and length(last(/MikroTik CRS305-1G-4SIN by SNMP/system.name))>0`|Info|**Manual close**: Yes|
|Mikrotik: No SNMP data collection|<p>SNMP is not available for polling. Please check device connectivity and SNMP settings.</p>|`max(/MikroTik CRS305-1G-4SIN by SNMP/zabbix[host,snmp,available],{$SNMP.TIMEOUT})=0`|Warning|**Depends on**:<br><ul><li>Mikrotik: Unavailable by ICMP ping</li></ul>|
|Mikrotik: Unavailable by ICMP ping|<p>Last three attempts returned timeout.  Please check device connectivity.</p>|`max(/MikroTik CRS305-1G-4SIN by SNMP/icmpping,#3)=0`|High||
|Mikrotik: High ICMP ping loss||`min(/MikroTik CRS305-1G-4SIN by SNMP/icmppingloss,5m)>{$ICMP_LOSS_WARN} and min(/MikroTik CRS305-1G-4SIN by SNMP/icmppingloss,5m)<100`|Warning|**Depends on**:<br><ul><li>Mikrotik: Unavailable by ICMP ping</li></ul>|
|Mikrotik: High ICMP ping response time||`avg(/MikroTik CRS305-1G-4SIN by SNMP/icmppingsec,5m)>{$ICMP_RESPONSE_TIME_WARN}`|Warning|**Depends on**:<br><ul><li>Mikrotik: High ICMP ping loss</li><li>Mikrotik: Unavailable by ICMP ping</li></ul>|

### LLD rule CPU discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|CPU discovery|<p>HOST-RESOURCES-MIB::hrProcessorTable discovery</p>|SNMP agent|hrProcessorLoad.discovery|

### Item prototypes for CPU discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|#{#SNMPINDEX}: CPU utilization|<p>MIB: HOST-RESOURCES-MIB</p><p>The average, over the last minute, of the percentage of time that this processor was not idle. Implementations may approximate this one minute smoothing period if necessary.</p>|SNMP agent|system.cpu.util[hrProcessorLoad.{#SNMPINDEX}]|

### Trigger prototypes for CPU discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|#{#SNMPINDEX}: High CPU utilization|<p>CPU utilization is too high. The system might be slow to respond.</p>|`min(/MikroTik CRS305-1G-4SIN by SNMP/system.cpu.util[hrProcessorLoad.{#SNMPINDEX}],5m)>{$CPU.UTIL.CRIT}`|Warning||

### LLD rule Temperature CPU discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Temperature CPU discovery|<p>MIKROTIK-MIB::mtxrHlProcessorTemperature</p><p>Since temperature of CPU is not available on all Mikrotik hardware, this is done to avoid unsupported items.</p>|SNMP agent|mtxrHlProcessorTemperature.discovery|

### Item prototypes for Temperature CPU discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|CPU: Temperature|<p>MIB: MIKROTIK-MIB</p><p>mtxrHlProcessorTemperature Processor temperature in Celsius (degrees C).</p><p>Might be missing in entry models (RB750, RB450G..).</p>|SNMP agent|sensor.temp.value[mtxrHlProcessorTemperature.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Custom multiplier: `0.1`</p></li></ul>|

### Trigger prototypes for Temperature CPU discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|CPU: Temperature is above warning threshold|<p>This trigger uses temperature sensor values as well as temperature sensor status if available.</p>|`avg(/MikroTik CRS305-1G-4SIN by SNMP/sensor.temp.value[mtxrHlProcessorTemperature.{#SNMPINDEX}],5m)>{$TEMP_WARN:"CPU"}`|Warning|**Depends on**:<br><ul><li>CPU: Temperature is above critical threshold</li></ul>|
|CPU: Temperature is above critical threshold|<p>This trigger uses temperature sensor values as well as temperature sensor status if available.</p>|`avg(/MikroTik CRS305-1G-4SIN by SNMP/sensor.temp.value[mtxrHlProcessorTemperature.{#SNMPINDEX}],5m)>{$TEMP_CRIT:"CPU"}`|High||
|CPU: Temperature is too low||`avg(/MikroTik CRS305-1G-4SIN by SNMP/sensor.temp.value[mtxrHlProcessorTemperature.{#SNMPINDEX}],5m)<{$TEMP_CRIT_LOW:"CPU"}`|Average||

### LLD rule Temperature sensor discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Temperature sensor discovery|<p>MIKROTIK-MIB::mtxrHlTemperature</p><p>Since temperature sensor is not available on all Mikrotik hardware, this is done to avoid unsupported items.</p>|SNMP agent|mtxrHlTemperature.discovery|

### Item prototypes for Temperature sensor discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Device: Temperature|<p>MIB: MIKROTIK-MIB</p><p>mtxrHlTemperature Device temperature in Celsius (degrees C).</p><p>Might be missing in entry models (RB750, RB450G..).</p><p></p><p>Reference: http://wiki.mikrotik.com/wiki/Manual:SNMP</p>|SNMP agent|sensor.temp.value[mtxrHlTemperature.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Custom multiplier: `0.1`</p></li></ul>|

### Trigger prototypes for Temperature sensor discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Device: Temperature is above warning threshold|<p>This trigger uses temperature sensor values as well as temperature sensor status if available.</p>|`avg(/MikroTik CRS305-1G-4SIN by SNMP/sensor.temp.value[mtxrHlTemperature.{#SNMPINDEX}],5m)>{$TEMP_WARN:"Device"}`|Warning|**Depends on**:<br><ul><li>Device: Temperature is above critical threshold</li></ul>|
|Device: Temperature is above critical threshold|<p>This trigger uses temperature sensor values as well as temperature sensor status if available.</p>|`avg(/MikroTik CRS305-1G-4SIN by SNMP/sensor.temp.value[mtxrHlTemperature.{#SNMPINDEX}],5m)>{$TEMP_CRIT:"Device"}`|High||
|Device: Temperature is too low||`avg(/MikroTik CRS305-1G-4SIN by SNMP/sensor.temp.value[mtxrHlTemperature.{#SNMPINDEX}],5m)<{$TEMP_CRIT_LOW:"Device"}`|Average||

### LLD rule LTE modem discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|LTE modem discovery|<p>MIKROTIK-MIB::mtxrLTEModemInterfaceIndex</p>|SNMP agent|mtxrLTEModem.discovery|

### Item prototypes for LTE modem discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Interface {#IFNAME}({#IFALIAS}): LTE modem RSSI|<p>MIB: MIKROTIK-MIB</p><p>mtxrLTEModemSignalRSSI Received Signal Strength Indicator.</p>|SNMP agent|lte.modem.rssi[mtxrLTEModemSignalRSSI.{#SNMPINDEX}]|
|Interface {#IFNAME}({#IFALIAS}): LTE modem RSRP|<p>MIB: MIKROTIK-MIB</p><p>mtxrLTEModemSignalRSRP Reference Signal Received Power.</p>|SNMP agent|lte.modem.rsrp[mtxrLTEModemSignalRSRP.{#SNMPINDEX}]|
|Interface {#IFNAME}({#IFALIAS}): LTE modem RSRQ|<p>MIB: MIKROTIK-MIB</p><p>mtxrLTEModemSignalRSRQ Reference Signal Received Quality.</p>|SNMP agent|lte.modem.rsrq[mtxrLTEModemSignalRSRQ.{#SNMPINDEX}]|
|Interface {#IFNAME}({#IFALIAS}): LTE modem SINR|<p>MIB: MIKROTIK-MIB</p><p>mtxrLTEModemSignalSINR Signal to Interference & Noise Ratio.</p>|SNMP agent|lte.modem.sinr[mtxrLTEModemSignalSINR.{#SNMPINDEX}]|

### Trigger prototypes for LTE modem discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Interface {#IFNAME}({#IFALIAS}): LTE modem RSSI is low||`max(/MikroTik CRS305-1G-4SIN by SNMP/lte.modem.rssi[mtxrLTEModemSignalRSSI.{#SNMPINDEX}],5m) < {$LTEMODEM.RSSI.MIN.WARN}`|Warning||
|Interface {#IFNAME}({#IFALIAS}): LTE modem RSRP is low||`max(/MikroTik CRS305-1G-4SIN by SNMP/lte.modem.rsrp[mtxrLTEModemSignalRSRP.{#SNMPINDEX}],5m) < {$LTEMODEM.RSRP.MIN.WARN}`|Warning||
|Interface {#IFNAME}({#IFALIAS}): LTE modem RSRQ is low||`max(/MikroTik CRS305-1G-4SIN by SNMP/lte.modem.rsrq[mtxrLTEModemSignalRSRQ.{#SNMPINDEX}],5m) < {$LTEMODEM.RSRQ.MIN.WARN}`|Warning||
|Interface {#IFNAME}({#IFALIAS}): LTE modem SINR is low||`max(/MikroTik CRS305-1G-4SIN by SNMP/lte.modem.sinr[mtxrLTEModemSignalSINR.{#SNMPINDEX}],5m) < {$LTEMODEM.SINR.MIN.WARN}`|Warning||

### LLD rule AP channel discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|AP channel discovery|<p>MIKROTIK-MIB::mtxrWlAp</p>|SNMP agent|mtxrWlAp.discovery|

### Item prototypes for AP channel discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Interface {#IFNAME}({#IFALIAS}): SSID|<p>MIB: MIKROTIK-MIB</p><p>mtxrWlApSsid Service Set Identifier.</p>|SNMP agent|ssid.name[mtxrWlApSsid.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Interface {#IFNAME}({#IFALIAS}): AP band|<p>MIB: MIKROTIK-MIB</p><p>mtxrWlApBand</p>|SNMP agent|ssid.band[mtxrWlApBand.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Interface {#IFNAME}({#IFALIAS}): AP noise floor|<p>MIB: MIKROTIK-MIB</p><p>mtxrWlApNoiseFloor</p>|SNMP agent|ssid.noise[mtxrWlApNoiseFloor.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `15m`</p></li></ul>|
|Interface {#IFNAME}({#IFALIAS}): AP registered clients|<p>MIB: MIKROTIK-MIB</p><p>mtxrWlApClientCount Client established connection to AP, but didn't finish all authentication procedures for full connection.</p>|SNMP agent|ssid.regclient[mtxrWlApClientCount.{#SNMPINDEX}]|
|Interface {#IFNAME}({#IFALIAS}): AP authenticated clients|<p>MIB: MIKROTIK-MIB</p><p>mtxrWlApAuthClientCount Number of authentication clients.</p>|SNMP agent|ssid.authclient[mtxrWlApAuthClientCount.{#SNMPINDEX}]|

### LLD rule CAPsMAN AP channel discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|CAPsMAN AP channel discovery|<p>MIKROTIK-MIB::mtxrWlCMChannel</p>|SNMP agent|mtxrWlCMChannel.discovery|

### Item prototypes for CAPsMAN AP channel discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Interface {#IFNAME}({#IFALIAS}): AP channel|<p>MIB: MIKROTIK-MIB</p><p>mtxrWlCMChannel</p>|SNMP agent|ssid.channel[mtxrWlCMChannel.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Interface {#IFNAME}({#IFALIAS}): AP state|<p>MIB: MIKROTIK-MIB</p><p>mtxrWlCMState Wireless interface state.</p>|SNMP agent|ssid.state[mtxrWlCMState.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Interface {#IFNAME}({#IFALIAS}): AP registered clients|<p>MIB: MIKROTIK-MIB</p><p>mtxrWlCMRegClientCount Client established connection to AP, but didn't finish all authentication procedures for full connection.</p>|SNMP agent|ssid.regclient[mtxrWlCMRegClientCount.{#SNMPINDEX}]|
|Interface {#IFNAME}({#IFALIAS}): AP authenticated clients|<p>MIB: MIKROTIK-MIB</p><p>mtxrWlCMAuthClientCount Number of authentication clients.</p>|SNMP agent|ssid.authclient[mtxrWlCMAuthClientCount.{#SNMPINDEX}]|

### Trigger prototypes for CAPsMAN AP channel discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Interface {#IFNAME}({#IFALIAS}): AP interface {#IFNAME}({#IFALIAS}) is not running|<p>Access point interface can be not running by different reasons - disabled interface, power off, network link down.</p>|`last(/MikroTik CRS305-1G-4SIN by SNMP/ssid.state[mtxrWlCMState.{#SNMPINDEX}])<>"running-ap"`|Warning||

### LLD rule Storage discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Storage discovery|<p>HOST-RESOURCES-MIB::hrStorage discovery with storage filter</p>|SNMP agent|storage.discovery|

### Item prototypes for Storage discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Disk-{#SNMPINDEX}: Used space|<p>MIB: HOST-RESOURCES-MIB</p><p>The amount of the storage represented by this entry that is allocated, in units of hrStorageAllocationUnits.</p>|SNMP agent|vfs.fs.used[hrStorageSize.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Custom multiplier: `1024`</p></li></ul>|
|Disk-{#SNMPINDEX}: Total space|<p>MIB: HOST-RESOURCES-MIB</p><p>The size of the storage represented by this entry, in</p><p>units of hrStorageAllocationUnits. This object is</p><p>writable to allow remote configuration of the size of</p><p>the storage area in those cases where such an</p><p>operation makes sense and is possible on the</p><p>underlying system. For example, the amount of main</p><p>memory allocated to a buffer pool might be modified or</p><p>the amount of disk space allocated to virtual memory</p><p>might be modified.</p>|SNMP agent|vfs.fs.total[hrStorageSize.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Custom multiplier: `1024`</p></li></ul>|
|Disk-{#SNMPINDEX}: Space utilization|<p>The space utilization expressed in % for Disk-{#SNMPINDEX}.</p>|Calculated|vfs.fs.pused[hrStorageSize.{#SNMPINDEX}]|

### Trigger prototypes for Storage discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Disk-{#SNMPINDEX}: Disk space is critically low|<p>Two conditions should match: First, space utilization should be above {$VFS.FS.PUSED.MAX.CRIT:"Disk-{#SNMPINDEX}"}.<br> Second condition should be one of the following:<br> - The disk free space is less than {$VFS.FS.FREE.MIN.CRIT:"Disk-{#SNMPINDEX}"}.<br> - The disk will be full in less than 24 hours.</p>|`last(/MikroTik CRS305-1G-4SIN by SNMP/vfs.fs.pused[hrStorageSize.{#SNMPINDEX}])>{$VFS.FS.PUSED.MAX.CRIT:"Disk-{#SNMPINDEX}"} and ((last(/MikroTik CRS305-1G-4SIN by SNMP/vfs.fs.total[hrStorageSize.{#SNMPINDEX}])-last(/MikroTik CRS305-1G-4SIN by SNMP/vfs.fs.used[hrStorageSize.{#SNMPINDEX}]))<{$VFS.FS.FREE.MIN.CRIT:"Disk-{#SNMPINDEX}"} or timeleft(/MikroTik CRS305-1G-4SIN by SNMP/vfs.fs.pused[hrStorageSize.{#SNMPINDEX}],1h,100)<1d)`|Average|**Manual close**: Yes|
|Disk-{#SNMPINDEX}: Disk space is low|<p>Two conditions should match: First, space utilization should be above {$VFS.FS.PUSED.MAX.WARN:"Disk-{#SNMPINDEX}"}.<br> Second condition should be one of the following:<br> - The disk free space is less than {$VFS.FS.FREE.MIN.WARN:"Disk-{#SNMPINDEX}"}.<br> - The disk will be full in less than 24 hours.</p>|`last(/MikroTik CRS305-1G-4SIN by SNMP/vfs.fs.pused[hrStorageSize.{#SNMPINDEX}])>{$VFS.FS.PUSED.MAX.WARN:"Disk-{#SNMPINDEX}"} and ((last(/MikroTik CRS305-1G-4SIN by SNMP/vfs.fs.total[hrStorageSize.{#SNMPINDEX}])-last(/MikroTik CRS305-1G-4SIN by SNMP/vfs.fs.used[hrStorageSize.{#SNMPINDEX}]))<{$VFS.FS.FREE.MIN.WARN:"Disk-{#SNMPINDEX}"} or timeleft(/MikroTik CRS305-1G-4SIN by SNMP/vfs.fs.pused[hrStorageSize.{#SNMPINDEX}],1h,100)<1d)`|Warning|**Manual close**: Yes<br>**Depends on**:<br><ul><li>Disk-{#SNMPINDEX}: Disk space is critically low</li></ul>|

### LLD rule Network interfaces discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Network interfaces discovery|<p>Discovering interfaces from IF-MIB.</p>|SNMP agent|net.if.discovery|

### Item prototypes for Network interfaces discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Interface {#IFNAME}({#IFALIAS}): Operational status|<p>MIB: IF-MIB</p><p>The current operational state of the interface.</p><p>- The testing(3) state indicates that no operational packet scan be passed</p><p>- If ifAdminStatus is down(2) then ifOperStatus should be down(2)</p><p>- If ifAdminStatus is changed to up(1) then ifOperStatus should change to up(1) if the interface is ready to transmit and receive network traffic</p><p>- It should change todormant(5) if the interface is waiting for external actions (such as a serial line waiting for an incoming connection)</p><p>- It should remain in the down(2) state if and only if there is a fault that prevents it from going to the up(1) state</p><p>- It should remain in the notPresent(6) state if the interface has missing(typically, hardware) components.</p>|SNMP agent|net.if.status[ifOperStatus.{#SNMPINDEX}]|
|Interface {#IFNAME}({#IFALIAS}): Bits received|<p>MIB: IF-MIB</p><p>The total number of octets received on the interface, including framing characters. This object is a 64-bit version of ifInOctets. Discontinuities in the value of this counter can occur at re-initialization of the management system, and at other times as indicated by the value of ifCounterDiscontinuityTime.</p>|SNMP agent|net.if.in[ifHCInOctets.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li><li><p>Custom multiplier: `8`</p></li></ul>|
|Interface {#IFNAME}({#IFALIAS}): Bits sent|<p>MIB: IF-MIB</p><p>The total number of octets transmitted out of the interface, including framing characters. This object is a 64-bit version of ifOutOctets.Discontinuities in the value of this counter can occur at re-initialization of the management system, and at other times as indicated by the value of ifCounterDiscontinuityTime.</p>|SNMP agent|net.if.out[ifHCOutOctets.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li><li><p>Custom multiplier: `8`</p></li></ul>|
|Interface {#IFNAME}({#IFALIAS}): Inbound packets with errors|<p>MIB: IF-MIB</p><p>For packet-oriented interfaces, the number of inbound packets that contained errors preventing them from being deliverable to a higher-layer protocol.  For character-oriented or fixed-length interfaces, the number of inbound transmission units that contained errors preventing them from being deliverable to a higher-layer protocol. Discontinuities in the value of this counter can occur at re-initialization of the management system, and at other times as indicated by the value of ifCounterDiscontinuityTime.</p>|SNMP agent|net.if.in.errors[ifInErrors.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li></ul>|
|Interface {#IFNAME}({#IFALIAS}): Outbound packets with errors|<p>MIB: IF-MIB</p><p>For packet-oriented interfaces, the number of outbound packets that contained errors preventing them from being deliverable to a higher-layer protocol.  For character-oriented or fixed-length interfaces, the number of outbound transmission units that contained errors preventing them from being deliverable to a higher-layer protocol. Discontinuities in the value of this counter can occur at re-initialization of the management system, and at other times as indicated by the value of ifCounterDiscontinuityTime.</p>|SNMP agent|net.if.out.errors[ifOutErrors.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li></ul>|
|Interface {#IFNAME}({#IFALIAS}): Outbound packets discarded|<p>MIB: IF-MIB</p><p>The number of outbound packets which were chosen to be discarded</p><p>even though no errors had been detected to prevent their being deliverable to a higher-layer protocol.</p><p>One possible reason for discarding such a packet could be to free up buffer space.</p><p>Discontinuities in the value of this counter can occur at re-initialization of the management system,</p><p>and at other times as indicated by the value of ifCounterDiscontinuityTime.</p>|SNMP agent|net.if.out.discards[ifOutDiscards.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li></ul>|
|Interface {#IFNAME}({#IFALIAS}): Inbound packets discarded|<p>MIB: IF-MIB</p><p>The number of inbound packets which were chosen to be discarded</p><p>even though no errors had been detected to prevent their being deliverable to a higher-layer protocol.</p><p>One possible reason for discarding such a packet could be to free up buffer space.</p><p>Discontinuities in the value of this counter can occur at re-initialization of the management system,</p><p>and at other times as indicated by the value of ifCounterDiscontinuityTime.</p>|SNMP agent|net.if.in.discards[ifInDiscards.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li>Change per second</li></ul>|
|Interface {#IFNAME}({#IFALIAS}): Interface type|<p>MIB: IF-MIB</p><p>The type of interface.</p><p>Additional values for ifType are assigned by the Internet Assigned Numbers Authority (IANA),</p><p>through updating the syntax of the IANAifType textual convention.</p>|SNMP agent|net.if.type[ifType.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|Interface {#IFNAME}({#IFALIAS}): Speed|<p>MIB: IF-MIB</p><p>An estimate of the interface's current bandwidth in units of 1,000,000 bits per second. If this object reports a value of `n' then the speed of the interface is somewhere in the range of `n-500,000' to`n+499,999'.  For interfaces which do not vary in bandwidth or for those where no accurate estimation can be made, this object should contain the nominal bandwidth. For a sub-layer which has no concept of bandwidth, this object should be zero.</p>|SNMP agent|net.if.speed[ifHighSpeed.{#SNMPINDEX}]<p>**Preprocessing**</p><ul><li><p>Custom multiplier: `1000000`</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|

### Trigger prototypes for Network interfaces discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Interface {#IFNAME}({#IFALIAS}): Link down|<p>This trigger expression works as follows:<br>1. It can be triggered if the operations status is down.<br>2. `{$IFCONTROL:"{#IFNAME}"}=1` - a user can redefine context macro to value - 0. That marks this interface as not important. No new trigger will be fired if this interface is down.<br>3. `{TEMPLATE_NAME:METRIC.diff()}=1` - the trigger fires only if the operational status was up to (1) sometime before (so, do not fire for the 'eternal off' interfaces.)<br><br>WARNING: if closed manually - it will not fire again on the next poll, because of .diff.</p>|`{$IFCONTROL:"{#IFNAME}"}=1 and last(/MikroTik CRS305-1G-4SIN by SNMP/net.if.status[ifOperStatus.{#SNMPINDEX}])=2 and (last(/MikroTik CRS305-1G-4SIN by SNMP/net.if.status[ifOperStatus.{#SNMPINDEX}],#1)<>last(/MikroTik CRS305-1G-4SIN by SNMP/net.if.status[ifOperStatus.{#SNMPINDEX}],#2))`|Average|**Manual close**: Yes|
|Interface {#IFNAME}({#IFALIAS}): High bandwidth usage|<p>The utilization of the network interface is close to its estimated maximum bandwidth.</p>|`(avg(/MikroTik CRS305-1G-4SIN by SNMP/net.if.in[ifHCInOctets.{#SNMPINDEX}],15m)>({$IF.UTIL.MAX:"{#IFNAME}"}/100)*last(/MikroTik CRS305-1G-4SIN by SNMP/net.if.speed[ifHighSpeed.{#SNMPINDEX}]) or avg(/MikroTik CRS305-1G-4SIN by SNMP/net.if.out[ifHCOutOctets.{#SNMPINDEX}],15m)>({$IF.UTIL.MAX:"{#IFNAME}"}/100)*last(/MikroTik CRS305-1G-4SIN by SNMP/net.if.speed[ifHighSpeed.{#SNMPINDEX}])) and last(/MikroTik CRS305-1G-4SIN by SNMP/net.if.speed[ifHighSpeed.{#SNMPINDEX}])>0`|Warning|**Manual close**: Yes<br>**Depends on**:<br><ul><li>Interface {#IFNAME}({#IFALIAS}): Link down</li></ul>|
|Interface {#IFNAME}({#IFALIAS}): High error rate|<p>It recovers when it is below 80% of the `{$IF.ERRORS.WARN:"{#IFNAME}"}` threshold.</p>|`min(/MikroTik CRS305-1G-4SIN by SNMP/net.if.in.errors[ifInErrors.{#SNMPINDEX}],5m)>{$IF.ERRORS.WARN:"{#IFNAME}"} or min(/MikroTik CRS305-1G-4SIN by SNMP/net.if.out.errors[ifOutErrors.{#SNMPINDEX}],5m)>{$IF.ERRORS.WARN:"{#IFNAME}"}`|Warning|**Manual close**: Yes<br>**Depends on**:<br><ul><li>Interface {#IFNAME}({#IFALIAS}): Link down</li></ul>|
|Interface {#IFNAME}({#IFALIAS}): Ethernet has changed to lower speed than it was before|<p>This Ethernet connection has transitioned down from its known maximum speed. This might be a sign of autonegotiation issues. Acknowledge to close the problem manually.</p>|`change(/MikroTik CRS305-1G-4SIN by SNMP/net.if.speed[ifHighSpeed.{#SNMPINDEX}])<0 and last(/MikroTik CRS305-1G-4SIN by SNMP/net.if.speed[ifHighSpeed.{#SNMPINDEX}])>0 and ( last(/MikroTik CRS305-1G-4SIN by SNMP/net.if.type[ifType.{#SNMPINDEX}])=6 or last(/MikroTik CRS305-1G-4SIN by SNMP/net.if.type[ifType.{#SNMPINDEX}])=7 or last(/MikroTik CRS305-1G-4SIN by SNMP/net.if.type[ifType.{#SNMPINDEX}])=11 or last(/MikroTik CRS305-1G-4SIN by SNMP/net.if.type[ifType.{#SNMPINDEX}])=62 or last(/MikroTik CRS305-1G-4SIN by SNMP/net.if.type[ifType.{#SNMPINDEX}])=69 or last(/MikroTik CRS305-1G-4SIN by SNMP/net.if.type[ifType.{#SNMPINDEX}])=117 ) and (last(/MikroTik CRS305-1G-4SIN by SNMP/net.if.status[ifOperStatus.{#SNMPINDEX}])<>2)`|Info|**Manual close**: Yes<br>**Depends on**:<br><ul><li>Interface {#IFNAME}({#IFALIAS}): Link down</li></ul>|

## Feedback

Please report any issues with the template at [`https://support.zabbix.com`](https://support.zabbix.com)

You can also provide feedback, discuss the template, or ask for help at [`ZABBIX forums`](https://www.zabbix.com/forum/zabbix-suggestions-and-feedback)

