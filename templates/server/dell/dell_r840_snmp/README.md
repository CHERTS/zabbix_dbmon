
# DELL PowerEdge R840 SNMP

## Overview

For Zabbix version: 5.0 and higher  
This is a template for monitoring DELL PowerEdge R840 servers with iDRAC version 7 and later via Zabbix SNMP agent that works without any external scripts.

## Setup

> See [Zabbix template operation](https://www.zabbix.com/documentation/5.0/manual/config/templates_out_of_the_box/network_devices) for basic instructions.

Refer to the vendor documentation.

## Zabbix configuration

No specific Zabbix configuration is required.

### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$DISK.ARRAY.CACHE.BATTERY.STATUS.CRIT} |<p>The critical status of the disk array cache battery for trigger expression.</p> |`3` |
|{$DISK.ARRAY.CACHE.BATTERY.STATUS.OK} |<p>The OK status of the disk array cache battery for trigger expression.</p> |`2` |
|{$DISK.ARRAY.CACHE.BATTERY.STATUS.WARN} |<p>The warning status of the disk array cache battery for trigger expression.</p> |`4` |
|{$DISK.ARRAY.STATUS.CRIT} |<p>The critical status of the disk array for trigger expression.</p> |`5` |
|{$DISK.ARRAY.STATUS.FAIL} |<p>The disaster status of the disk array for trigger expression.</p> |`6` |
|{$DISK.ARRAY.STATUS.WARN} |<p>The warning status of the disk array for trigger expression.</p> |`4` |
|{$DISK.SMART.STATUS.FAIL} |<p>The critical S.M.A.R.T status of the disk for trigger expression.</p> |`1` |
|{$DISK.STATUS.FAIL:"critical"} |<p>The critical status of the disk for trigger expression.</p> |`5` |
|{$DISK.STATUS.FAIL:"nonRecoverable"} |<p>The critical status of the disk for trigger expression.</p> |`6` |
|{$DISK.STATUS.WARN:"nonCritical"} |<p>The warning status of the disk for trigger expression.</p> |`4` |
|{$FAN.STATUS.CRIT:"criticalLower"} |<p>The critical value of the FAN sensor for trigger expression.</p> |`8` |
|{$FAN.STATUS.CRIT:"criticalUpper"} |<p>The critical value of the FAN sensor for trigger expression.</p> |`5` |
|{$FAN.STATUS.CRIT:"failed"} |<p>The critical value of the FAN sensor for trigger expression.</p> |`10` |
|{$FAN.STATUS.CRIT:"nonRecoverableLower"} |<p>The critical value of the FAN sensor for trigger expression.</p> |`9` |
|{$FAN.STATUS.CRIT:"nonRecoverableUpper"} |<p>The critical value of the FAN sensor for trigger expression.</p> |`6` |
|{$FAN.STATUS.WARN:"nonCriticalLower"} |<p>The warning value of the FAN sensor for trigger expression.</p> |`7` |
|{$FAN.STATUS.WARN:"nonCriticalUpper"} |<p>The warning value of the FAN sensor for trigger expression.</p> |`4` |
|{$HEALTH.STATUS.CRIT} |<p>The critical status of the health for trigger expression.</p> |`5` |
|{$HEALTH.STATUS.DISASTER} |<p>The disaster status of the health for trigger expression.</p> |`6` |
|{$HEALTH.STATUS.WARN} |<p>The warning status of the health for trigger expression.</p> |`4` |
|{$PSU.STATUS.CRIT:"critical"} |<p>The critical value of the PSU sensor for trigger expression.</p> |`5` |
|{$PSU.STATUS.CRIT:"nonRecoverable"} |<p>The critical value of the PSU sensor for trigger expression.</p> |`6` |
|{$PSU.STATUS.WARN:"nonCritical"} |<p>The warning value of the PSU sensor for trigger expression.</p> |`4` |
|{$SENSOR.TEMP.STATUS.CRIT:"criticalLower"} |<p>The critical status of the temperature probe for trigger expression.</p> |`8` |
|{$SENSOR.TEMP.STATUS.CRIT:"criticalUpper"} |<p>The critical status of the temperature probe for trigger expression.</p> |`5` |
|{$SENSOR.TEMP.STATUS.CRIT:"nonRecoverableLower"} |<p>The critical status of the temperature probe for trigger expression.</p> |`9` |
|{$SENSOR.TEMP.STATUS.CRIT:"nonRecoverableUpper"} |<p>The critical status of the temperature probe for trigger expression.</p> |`6` |
|{$SENSOR.TEMP.STATUS.OK} |<p>The OK status of the temperature probe for trigger expression.</p> |`3` |
|{$SENSOR.TEMP.STATUS.WARN:"nonCriticalLower"} |<p>The warning status of the temperature probe for trigger expression.</p> |`7` |
|{$SENSOR.TEMP.STATUS.WARN:"nonCriticalUpper"} |<p>The warning status of the temperature probe for trigger expression.</p> |`4` |
|{$SNMP.TIMEOUT} |<p>The time interval for SNMP agent availability trigger expression.</p> |`5m` |
|{$VDISK.STATUS.CRIT:"failed"} |<p>The critical status of the virtual disk for trigger expression.</p> |`3` |
|{$VDISK.STATUS.WARN:"degraded"} |<p>The warning status of the virtual disk for trigger expression.</p> |`4` |

## Template links

There are no template links in this template.

## Discovery rules

|Name|Description|Type|Key and additional info|
|----|-----------|----|----|
|Temperature discovery |<p>Scanning table of Temperature Probe Table IDRAC-MIB-SMIv2::temperatureProbeTable</p> |SNMP |temp.discovery<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|PSU discovery |<p>IDRAC-MIB-SMIv2::powerSupplyTable</p> |SNMP |psu.discovery<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|FAN discovery |<p>IDRAC-MIB-SMIv2::coolingDeviceTable</p> |SNMP |fan.discovery<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p><p>**Filter**:</p>AND_OR <p>- A: {#TYPE} MATCHES_REGEX `3`</p> |
|Array controller discovery |<p>Scanning table of Array controllers: IDRAC-MIB-SMIv2::controllerTable</p> |SNMP |array.discovery<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Array controller cache discovery |<p>Scanning table of Array controllers: IDRAC-MIB-SMIv2::batteryTable</p> |SNMP |array.cache.discovery<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Physical disk discovery |<p>Scanning  table of physical drive entries IDRAC-MIB-SMIv2::physicalDiskTable.</p> |SNMP |physicaldisk.discovery<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Virtual disk discovery |<p>IDRAC-MIB-SMIv2::virtualDiskTable</p> |SNMP |virtualdisk.discovery<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |

## Items collected

|Group|Name|Description|Type|Key and additional info|
|-----|----|-----------|----|---------------------|
|Disk_arrays |Dell R840: {#CNTLR_NAME} Status |<p>MIB: IDRAC-MIB-SMIv2</p><p>The status of the controller itself without the propagation of any contained component status.</p><p>Possible values:</p><p>1: Other</p><p>2: Unknown</p><p>3: OK</p><p>4: Non-critical</p><p>5: Critical</p><p>6: Non-recoverable</p> |SNMP |dell.server.hw.diskarray.status[controllerComponentStatus.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Disk_arrays |Dell R840: {#CNTLR_NAME} Model |<p>MIB: IDRAC-MIB-SMIv2</p><p>The controller's name as represented in Storage Management.</p> |SNMP |dell.server.hw.diskarray.model[controllerName.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Disk_arrays |Dell R840: {#BATTERY_NAME} Status |<p>MIB: IDRAC-MIB-SMIv2</p><p>Current state of battery.</p><p>Possible values:</p><p>1: The current state could not be determined.</p><p>2: The battery is operating normally.</p><p>3: The battery has failed and needs to be replaced.</p><p>4: The battery temperature is high or charge level is depleting.</p><p>5: The battery is missing or not detected.</p><p>6: The battery is undergoing the re-charge phase.</p><p>7: The battery voltage or charge level is below the threshold.</p><p>                </p> |SNMP |dell.server.hw.diskarray.cache.battery.status[batteryState.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Fans |Dell R840: {#FAN_DESCR} Status |<p>MIB: IDRAC-MIB-SMIv2</p><p>This attribute defines the probe status of the cooling device.</p> |SNMP |dell.server.sensor.fan.status[coolingDeviceStatus.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Fans |Dell R840: {#FAN_DESCR} Speed |<p>MIB: IDRAC-MIB-SMIv2</p><p>This attribute defines the reading for a cooling device</p><p>of subtype other than coolingDeviceSubTypeIsDiscrete. When the value</p><p>for coolingDeviceSubType is other than coolingDeviceSubTypeIsDiscrete, the</p><p>value returned for this attribute is the speed in RPM or the OFF/ON value</p><p>of the cooling device. When the value for coolingDeviceSubType is</p><p>coolingDeviceSubTypeIsDiscrete, a value is not returned for this attribute.</p> |SNMP |dell.server.sensor.fan.speed[coolingDeviceReading.{#SNMPINDEX}] |
|General |Dell R840: SNMP traps (fallback) |<p>The item is used to collect all SNMP traps unmatched by other snmptrap items</p> |SNMP_TRAP |snmptrap.fallback |
|General |Dell R840: System location |<p>MIB: SNMPv2-MIB</p><p>The physical location of this node (e.g., 'telephone closet, 3rd floor'). If the location is unknown, the value is the zero-length string.</p> |SNMP |dell.server.location[sysLocation]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|General |Dell R840: System contact details |<p>MIB: SNMPv2-MIB</p><p>The textual identification of the contact person for this managed node, together with information on how to contact this person. If no contact information is known, the value is the zero-length string.</p> |SNMP |dell.server.contact[sysContact]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1d`</p> |
|General |Dell R840: System object ID |<p>MIB: SNMPv2-MIB</p><p>The vendor's authoritative identification of the network management subsystem contained in the entity. This value is allocated within the SMI enterprises subtree (1.3.6.1.4.1) and provides an easy and unambiguous means for determining 'what kind of box' is being managed. For example, if vendor 'Flintstones, Inc.' was assigned the subtree 1.3.6.1.4.1.4242, it could assign the identifier 1.3.6.1.4.1.4242.1.1 to its 'Fred Router'.</p> |SNMP |dell.server.objectid[sysObjectID]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|General |Dell R840: System name |<p>MIB: SNMPv2-MIB</p><p>An administratively-assigned name for this managed node. By convention, this is the node's fully-qualified domain name. If the name is unknown, the value is the zero-length string.</p> |SNMP |dell.server.name[sysName]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|General |Dell R840: System description |<p>MIB: SNMPv2-MIB</p><p>A textual description of the entity. This value should</p><p>include the full name and version identification of the system's hardware type, software operating-system, and</p><p>networking software.</p> |SNMP |dell.server.descr[sysDescr]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1d`</p> |
|Inventory |Dell R840: Hardware model name |<p>MIB: IDRAC-MIB-SMIv2</p><p>This attribute defines the model name of the system.</p> |SNMP |dell.server.hw.model[systemModelName]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Inventory |Dell R840: Hardware serial number |<p>MIB: IDRAC-MIB-SMIv2</p><p>This attribute defines the service tag of the system.</p> |SNMP |dell.server.hw.serialnumber[systemServiceTag]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Inventory |Dell R840: Operating system |<p>MIB: IDRAC-MIB-SMIv2</p><p>This attribute defines the name of the operating system that the host is running.</p> |SNMP |dell.server.sw.os[systemOSName]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1d`</p> |
|Inventory |Dell R840: Firmware version |<p>MIB: IDRAC-MIB-SMIv2</p><p>This attribute defines the firmware version of a remote access card.</p> |SNMP |dell.server.hw.firmware[racFirmwareVersion]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1d`</p> |
|Physical_disks |Dell R840: {#DISK_NAME} Status |<p>MIB: IDRAC-MIB-SMIv2</p><p>The status of the physical disk itself without the propagation of any contained component status.</p><p>Possible values:</p><p>1: Other</p><p>2: Unknown</p><p>3: OK</p><p>4: Non-critical</p><p>5: Critical</p><p>6: Non-recoverable</p> |SNMP |dell.server.hw.physicaldisk.status[physicalDiskComponentStatus.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Physical_disks |Dell R840: {#DISK_NAME} S.M.A.R.T. Status |<p>MIB: IDRAC-MIB-SMIv2</p><p>Indicates whether the physical disk has received a predictive failure alert.</p> |SNMP |dell.server.hw.physicaldisk.smart_status[physicalDiskSmartAlertIndication.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Physical_disks |Dell R840: {#DISK_NAME} Serial number |<p>MIB: IDRAC-MIB-SMIv2</p><p>The physical disk's unique identification number from the manufacturer.</p> |SNMP |dell.server.hw.physicaldisk.serialnumber[physicalDiskSerialNo.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Physical_disks |Dell R840: {#DISK_NAME} Model name |<p>MIB: IDRAC-MIB-SMIv2</p><p>The model number of the physical disk.</p> |SNMP |dell.server.hw.physicaldisk.model[physicalDiskProductID.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Physical_disks |Dell R840: {#DISK_NAME} Media type |<p>MIB: IDRAC-MIB-SMIv2</p><p>The media type of the physical disk. Possible Values:</p><p>1: The media type could not be determined.</p><p>2: Hard Disk Drive (HDD).</p><p>3: Solid State Drive (SSD).</p> |SNMP |dell.server.hw.physicaldisk.media_type[physicalDiskMediaType.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Physical_disks |Dell R840: {#DISK_NAME} Size |<p>MIB: IDRAC-MIB-SMIv2</p><p>The size of the physical disk in megabytes.</p> |SNMP |dell.server.hw.physicaldisk.size[physicalDiskCapacityInMB.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `1048576`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Power_supply |Dell R840: {#PSU_DESCR} |<p>MIB: IDRAC-MIB-SMIv2</p><p>0600.0012.0001.0005 This attribute defines the status of the power supply.</p> |SNMP |dell.server.sensor.psu.status[powerSupplyStatus.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Status |Dell R840: Overall system health status |<p>MIB: IDRAC-MIB-SMIv2</p><p>This attribute defines the overall rollup status of all components in the system being monitored by the remote access card. Includes system, storage, IO devices, iDRAC, CPU, memory, etc.</p> |SNMP |dell.server.status[globalSystemStatus]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Status |Dell R840: Uptime |<p>MIB: SNMPv2-MIB</p><p>The time (in hundredths of a second) since the network management portion of the system was last re-initialized.</p> |SNMP |dell.server.uptime[sysUpTime]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.01`</p> |
|Status |Dell R840: SNMP agent availability |<p>-</p> |INTERNAL |zabbix[host,snmp,available]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Temperature |Dell R840: {#SENSOR_LOCALE} Value |<p>MIB: IDRAC-MIB-SMIv2</p><p>This attribute defines the reading for a temperature probe of type other than temperatureProbeTypeIsDiscrete. When the value for temperatureProbeType is other than temperatureProbeTypeIsDiscrete, the value returned for this attribute is the temperature that the probe is reading in Centigrade. When the value for temperatureProbeType is temperatureProbeTypeIsDiscrete, a value is not returned for this attribute.</p> |SNMP |dell.server.sensor.temp.value[temperatureProbeReading.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.1`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Temperature |Dell R840: {#SENSOR_LOCALE} Status |<p>MIB: IDRAC-MIB-SMIv2</p><p>This attribute defines the probe status of the temperature probe.</p><p>Possible values:</p><p>other(1),               -- probe status is not one of the following:</p><p>unknown(2),             -- probe status is unknown (not known or monitored)</p><p>ok(3),                  -- probe is reporting a value within the thresholds</p><p>nonCriticalUpper(4),    -- probe has crossed the upper noncritical threshold</p><p>criticalUpper(5),       -- probe has crossed the upper critical threshold</p><p>nonRecoverableUpper(6), -- probe has crossed the upper non-recoverable threshold</p><p>nonCriticalLower(7),    -- probe has crossed the lower noncritical threshold</p><p>criticalLower(8),       -- probe has crossed the lower critical threshold</p><p>nonRecoverableLower(9), -- probe has crossed the lower non-recoverable threshold</p><p>failed(10)              -- probe is not functional</p> |SNMP |dell.server.sensor.temp.status[temperatureProbeStatus.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Virtual_disks |Dell R840: {#DISK_NAME} Status |<p>MIB: IDRAC-MIB-SMIv2</p><p>The current state of this virtual disk (which includes any member physical disks.)</p><p>Possible states:</p><p>1: The current state could not be determined.</p><p>2: The virtual disk is operating normally or optimally.</p><p>3: The virtual disk has encountered a failure. Data on the disk is lost or is about to be lost.</p><p>4: The virtual disk encountered a failure with one or all of the constituent redundant physical disks.</p><p>The data on the virtual disk might no longer be fault tolerant.</p> |SNMP |dell.server.hw.virtualdisk.status[virtualDiskState.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Virtual_disks |Dell R840: {#DISK_NAME} Layout type |<p>MIB: IDRAC-MIB-SMIv2</p><p>The virtual disk's RAID type.</p><p>Possible values:</p><p>1: Not one of the following</p><p>2: RAID-0</p><p>3: RAID-1</p><p>4: RAID-5</p><p>5: RAID-6</p><p>6: RAID-10</p><p>7: RAID-50</p><p>8: RAID-60</p><p>9: Concatenated RAID 1</p><p>10: Concatenated RAID 5</p> |SNMP |dell.server.hw.virtualdisk.layout[virtualDiskLayout.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Virtual_disks |Dell R840: {#DISK_NAME} Size |<p>MIB: IDRAC-MIB-SMIv2</p><p>The size of the virtual disk in megabytes.</p> |SNMP |dell.server.hw.virtualdisk.size[virtualDiskSizeInMB.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `1048576`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Virtual_disks |Dell R840: {#DISK_NAME} Current state |<p>MIB: IDRAC-MIB-SMIv2</p><p>The state of the virtual disk when there are progressive operations ongoing.</p><p>Possible values:</p><p>1: There is no active operation running.</p><p>2: The virtual disk configuration has changed. The physical disks included in the virtual disk are being modified to support the new configuration.</p><p>3: A Consistency Check (CC) is being performed on the virtual disk.</p><p>4: The virtual disk is being initialized.</p><p>5: BackGround Initialization (BGI) is being performed on the virtual disk.</p> |SNMP |dell.server.hw.virtualdisk.state[virtualDiskOperationalState.{#SNMPINDEX}] |
|Virtual_disks |Dell R840: {#DISK_NAME} Read policy |<p>MIB: IDRAC-MIB-SMIv2</p><p>The read policy used by the controller for read operations on this virtual disk.</p><p>Possible values:</p><p>1: No Read Ahead.</p><p>2: Read Ahead.</p><p>3: Adaptive Read Ahead.</p> |SNMP |dell.server.hw.virtualdisk.readpolicy[virtualDiskReadPolicy.{#SNMPINDEX}] |
|Virtual_disks |Dell R840: {#DISK_NAME} Write policy |<p>MIB: IDRAC-MIB-SMIv2</p><p>The write policy used by the controller for write operations on this virtual disk.</p><p>Possible values:</p><p>1: Write Through.</p><p>2: Write Back.</p><p>3: Force Write Back.</p> |SNMP |dell.server.hw.virtualdisk.writepolicy[virtualDiskWritePolicy.{#SNMPINDEX}] |

## Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----|----|----|
|Dell R840: {#CNTLR_NAME} is in unrecoverable state |<p>Please check the device for faults.</p> |`{TEMPLATE_NAME:dell.server.hw.diskarray.status[controllerComponentStatus.{#SNMPINDEX}].last()}={$DISK.ARRAY.STATUS.FAIL}` |DISASTER | |
|Dell R840: {#CNTLR_NAME} is in critical state |<p>Please check the device for faults.</p> |`{TEMPLATE_NAME:dell.server.hw.diskarray.status[controllerComponentStatus.{#SNMPINDEX}].last()}={$DISK.ARRAY.STATUS.CRIT}` |HIGH |<p>**Depends on**:</p><p>- Dell R840: {#CNTLR_NAME} is in unrecoverable state</p> |
|Dell R840: {#CNTLR_NAME} is in warning state |<p>Please check the device for faults.</p> |`{TEMPLATE_NAME:dell.server.hw.diskarray.status[controllerComponentStatus.{#SNMPINDEX}].last()}={$DISK.ARRAY.STATUS.WARN}` |AVERAGE |<p>**Depends on**:</p><p>- Dell R840: {#CNTLR_NAME} is in critical state</p><p>- Dell R840: {#CNTLR_NAME} is in unrecoverable state</p> |
|Dell R840: {#BATTERY_NAME} is in critical state |<p>Please check the device for faults.</p> |`{TEMPLATE_NAME:dell.server.hw.diskarray.cache.battery.status[batteryState.{#SNMPINDEX}].last()}={$DISK.ARRAY.CACHE.BATTERY.STATUS.CRIT}` |AVERAGE | |
|Dell R840: {#BATTERY_NAME} is in warning state |<p>Please check the device for faults.</p> |`{TEMPLATE_NAME:dell.server.hw.diskarray.cache.battery.status[batteryState.{#SNMPINDEX}].last()}={$DISK.ARRAY.CACHE.BATTERY.STATUS.WARN}` |WARNING |<p>**Depends on**:</p><p>- Dell R840: {#BATTERY_NAME} is in critical state</p> |
|Dell R840: {#BATTERY_NAME} is not in optimal state |<p>Please check the device for faults.</p> |`{TEMPLATE_NAME:dell.server.hw.diskarray.cache.battery.status[batteryState.{#SNMPINDEX}].last()}<>{$DISK.ARRAY.CACHE.BATTERY.STATUS.OK}` |WARNING |<p>**Depends on**:</p><p>- Dell R840: {#BATTERY_NAME} is in critical state</p><p>- Dell R840: {#BATTERY_NAME} is in warning state</p> |
|Dell R840: {#FAN_DESCR} is in critical state |<p>Please check the fan unit.</p> |`{TEMPLATE_NAME:dell.server.sensor.fan.status[coolingDeviceStatus.{#SNMPINDEX}].last()}={$FAN.STATUS.CRIT:"criticalUpper"} or {TEMPLATE_NAME:dell.server.sensor.fan.status[coolingDeviceStatus.{#SNMPINDEX}].last()}={$FAN.STATUS.CRIT:"nonRecoverableUpper"} or {TEMPLATE_NAME:dell.server.sensor.fan.status[coolingDeviceStatus.{#SNMPINDEX}].last()}={$FAN.STATUS.CRIT:"criticalLower"} or {TEMPLATE_NAME:dell.server.sensor.fan.status[coolingDeviceStatus.{#SNMPINDEX}].last()}={$FAN.STATUS.CRIT:"nonRecoverableLower"} or {TEMPLATE_NAME:dell.server.sensor.fan.status[coolingDeviceStatus.{#SNMPINDEX}].last()}={$FAN.STATUS.CRIT:"failed"}` |AVERAGE | |
|Dell R840: {#FAN_DESCR} is in warning state |<p>Please check the fan unit.</p> |`{TEMPLATE_NAME:dell.server.sensor.fan.status[coolingDeviceStatus.{#SNMPINDEX}].last()}={$FAN.STATUS.WARN:"nonCriticalUpper"} or {TEMPLATE_NAME:dell.server.sensor.fan.status[coolingDeviceStatus.{#SNMPINDEX}].last()}={$FAN.STATUS.WARN:"nonCriticalLower"}` |WARNING |<p>**Depends on**:</p><p>- Dell R840: {#FAN_DESCR} is in critical state</p> |
|Dell R840: System name has changed (new name: {ITEM.VALUE}) |<p>System name has changed. Ack to close.</p> |`{TEMPLATE_NAME:dell.server.name[sysName].diff()}=1 and {TEMPLATE_NAME:dell.server.name[sysName].strlen()}>0` |INFO |<p>Manual close: YES</p> |
|Dell R840: Device has been replaced (new serial number received) |<p>Device serial number has changed. Ack to close</p> |`{TEMPLATE_NAME:dell.server.hw.serialnumber[systemServiceTag].diff()}=1 and {TEMPLATE_NAME:dell.server.hw.serialnumber[systemServiceTag].strlen()}>0` |INFO |<p>Manual close: YES</p> |
|Dell R840: Operating system description has changed |<p>Operating system description has changed. Possibly, the system has been updated or replaced. Ack to close.</p> |`{TEMPLATE_NAME:dell.server.sw.os[systemOSName].diff()}=1 and {TEMPLATE_NAME:dell.server.sw.os[systemOSName].strlen()}>0` |INFO |<p>Manual close: YES</p> |
|Dell R840: Firmware has changed |<p>Firmware version has changed. Ack to close.</p> |`{TEMPLATE_NAME:dell.server.hw.firmware[racFirmwareVersion].diff()}=1 and {TEMPLATE_NAME:dell.server.hw.firmware[racFirmwareVersion].strlen()}>0` |INFO |<p>Manual close: YES</p> |
|Dell R840: {#DISK_NAME} failed |<p>Please check physical disk for warnings or errors.</p> |`{TEMPLATE_NAME:dell.server.hw.physicaldisk.status[physicalDiskComponentStatus.{#SNMPINDEX}].last()}={$DISK.STATUS.FAIL:"critical"} or {TEMPLATE_NAME:dell.server.hw.physicaldisk.status[physicalDiskComponentStatus.{#SNMPINDEX}].last()}={$DISK.STATUS.FAIL:"nonRecoverable"}` |HIGH | |
|Dell R840: {#DISK_NAME} is in warning state |<p>Please check physical disk for warnings or errors.</p> |`{TEMPLATE_NAME:dell.server.hw.physicaldisk.status[physicalDiskComponentStatus.{#SNMPINDEX}].last()}={$DISK.STATUS.WARN:"nonCritical"}` |WARNING |<p>**Depends on**:</p><p>- Dell R840: {#DISK_NAME} failed</p> |
|Dell R840: {#DISK_NAME} S.M.A.R.T. failed |<p>Disk probably requires replacement.</p> |`{TEMPLATE_NAME:dell.server.hw.physicaldisk.smart_status[physicalDiskSmartAlertIndication.{#SNMPINDEX}].last()}={$DISK.SMART.STATUS.FAIL:"replaceDrive"} or {TEMPLATE_NAME:dell.server.hw.physicaldisk.smart_status[physicalDiskSmartAlertIndication.{#SNMPINDEX}].last()}={$DISK.SMART.STATUS.FAIL:"replaceDriveSSDWearOut"}` |HIGH |<p>**Depends on**:</p><p>- Dell R840: {#DISK_NAME} failed</p> |
|Dell R840: {#DISK_NAME} has been replaced (new serial number received) |<p>{#DISK_NAME} serial number has changed. Ack to close</p> |`{TEMPLATE_NAME:dell.server.hw.physicaldisk.serialnumber[physicalDiskSerialNo.{#SNMPINDEX}].diff()}=1 and {TEMPLATE_NAME:dell.server.hw.physicaldisk.serialnumber[physicalDiskSerialNo.{#SNMPINDEX}].strlen()}>0` |INFO |<p>Manual close: YES</p> |
|Dell R840: Power supply {#PSU_DESCR} is in critical state |<p>Please check the power supply unit for errors.</p> |`{TEMPLATE_NAME:dell.server.sensor.psu.status[powerSupplyStatus.{#SNMPINDEX}].last()}={$PSU.STATUS.CRIT:"critical"} or {TEMPLATE_NAME:dell.server.sensor.psu.status[powerSupplyStatus.{#SNMPINDEX}].last()}={$PSU.STATUS.CRIT:"nonRecoverable"}` |AVERAGE | |
|Dell R840: Power supply {#PSU_DESCR} is in warning state |<p>Please check the power supply unit for errors.</p> |`{TEMPLATE_NAME:dell.server.sensor.psu.status[powerSupplyStatus.{#SNMPINDEX}].last()}={$PSU.STATUS.WARN:"nonCritical"}` |WARNING |<p>**Depends on**:</p><p>- Dell R840: Power supply {#PSU_DESCR} is in critical state</p> |
|Dell R840: System is in unrecoverable state |<p>Please check the device for faults.</p> |`{TEMPLATE_NAME:dell.server.status[globalSystemStatus].last()}={$HEALTH.STATUS.DISASTER}` |DISASTER | |
|Dell R840: System status is in critical state |<p>Please check the device for errors.</p> |`{TEMPLATE_NAME:dell.server.status[globalSystemStatus].last()}={$HEALTH.STATUS.CRIT}` |HIGH | |
|Dell R840: System status is in warning state |<p>Please check the device for warnings.</p> |`{TEMPLATE_NAME:dell.server.status[globalSystemStatus].last()}={$HEALTH.STATUS.WARN}` |WARNING |<p>**Depends on**:</p><p>- Dell R840: System status is in critical state</p> |
|Dell R840: {HOST.NAME} has been restarted (uptime < 10m) |<p>Uptime is less than 10 minutes</p> |`{TEMPLATE_NAME:dell.server.uptime[sysUpTime].last()}<10m` |WARNING |<p>Manual close: YES</p> |
|Dell R840: No SNMP data collection |<p>SNMP is not available for polling. Please check device connectivity and SNMP settings.</p> |`{TEMPLATE_NAME:zabbix[host,snmp,available].max({$SNMP.TIMEOUT})}=0` |WARNING | |
|Dell R840: Probe {#SENSOR_LOCALE} is in critical status |<p>Please check the device for faults.</p> |`{TEMPLATE_NAME:dell.server.sensor.temp.status[temperatureProbeStatus.{#SNMPINDEX}].last()}={$SENSOR.TEMP.STATUS.CRIT:"criticalUpper"} or {TEMPLATE_NAME:dell.server.sensor.temp.status[temperatureProbeStatus.{#SNMPINDEX}].last()}={$SENSOR.TEMP.STATUS.CRIT:"nonRecoverableUpper"} or {TEMPLATE_NAME:dell.server.sensor.temp.status[temperatureProbeStatus.{#SNMPINDEX}].last()}={$SENSOR.TEMP.STATUS.CRIT:"criticalLower"} or {TEMPLATE_NAME:dell.server.sensor.temp.status[temperatureProbeStatus.{#SNMPINDEX}].last()}={$SENSOR.TEMP.STATUS.CRIT:"nonRecoverableLower"}` |AVERAGE | |
|Dell R840: Probe {#SENSOR_LOCALE} is in warning status |<p>Please check the device for faults.</p> |`{TEMPLATE_NAME:dell.server.sensor.temp.status[temperatureProbeStatus.{#SNMPINDEX}].last()}={$SENSOR.TEMP.STATUS.WARN:"nonCriticalUpper"} or {TEMPLATE_NAME:dell.server.sensor.temp.status[temperatureProbeStatus.{#SNMPINDEX}].last()}={$SENSOR.TEMP.STATUS.WARN:"nonCriticalLower"}` |WARNING |<p>**Depends on**:</p><p>- Dell R840: Probe {#SENSOR_LOCALE} is in critical status</p> |
|Dell R840: Probe {#SENSOR_LOCALE} is not in optimal status |<p>Please check the device for faults.</p> |`{TEMPLATE_NAME:dell.server.sensor.temp.status[temperatureProbeStatus.{#SNMPINDEX}].last()}<>{$SENSOR.TEMP.STATUS.OK}` |INFO |<p>Manual close: YES</p><p>**Depends on**:</p><p>- Dell R840: Probe {#SENSOR_LOCALE} is in critical status</p><p>- Dell R840: Probe {#SENSOR_LOCALE} is in warning status</p> |
|Dell R840: {#DISK_NAME} failed |<p>Please check the virtual disk for warnings or errors.</p> |`{TEMPLATE_NAME:dell.server.hw.virtualdisk.status[virtualDiskState.{#SNMPINDEX}].last()}={$VDISK.STATUS.CRIT:"failed"}` |HIGH | |
|Dell R840: {#DISK_NAME} is in warning state |<p>Please check the virtual disk for warnings or errors.</p> |`{TEMPLATE_NAME:dell.server.hw.virtualdisk.status[virtualDiskState.{#SNMPINDEX}].last()}={$VDISK.STATUS.WARN:"degraded"}` |AVERAGE |<p>**Depends on**:</p><p>- Dell R840: {#DISK_NAME} failed</p> |

## Feedback

Please report any issues with the template at https://support.zabbix.com

You can also provide a feedback, discuss the template or ask for help with it at [ZABBIX forums](https://www.zabbix.com/forum/zabbix-suggestions-and-feedback/426752-discussion-thread-for-official-zabbix-dell-templates).

