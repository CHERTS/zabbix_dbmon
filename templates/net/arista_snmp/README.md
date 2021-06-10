
# Arista SNMP

## Overview

For Zabbix version: 5.0 and higher  

This template was tested on:

- Arista DCS-7050Q-16, version EOS version 4.12.6

## Setup

Refer to the vendor documentation.

## Zabbix configuration

No specific Zabbix configuration is required.

### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$FAN_CRIT_STATUS} |<p>-</p> |`3` |
|{$MEMORY.NAME.NOT_MATCHES} |<p>Filter is overridden to ignore RAM(Cache) and RAM(Buffers) memory objects.</p> |`(Buffer|Cache)` |
|{$PSU_CRIT_STATUS} |<p>-</p> |`2` |
|{$VFS.FS.PUSED.MAX.CRIT} |<p>-</p> |`95` |
|{$VFS.FS.PUSED.MAX.WARN} |<p>-</p> |`90` |

## Template links

|Name|
|----|
|EtherLike-MIB SNMP |
|Generic SNMP |
|HOST-RESOURCES-MIB SNMP |
|Interfaces SNMP |

## Discovery rules

|Name|Description|Type|Key and additional info|
|----|-----------|----|----|
|Temperature discovery |<p>ENTITY-SENSORS-MIB::EntitySensorDataType discovery with celsius filter</p> |DEPENDENT |temp.discovery<p>**Filter**:</p>AND <p>- B: {#SENSOR_TYPE} MATCHES_REGEX `8`</p><p>- B: {#SENSOR_PRECISION} MATCHES_REGEX `1`</p> |
|Fan discovery |<p>ENTITY-SENSORS-MIB::EntitySensorDataType discovery with rpm filter</p> |DEPENDENT |fan.discovery<p>**Filter**:</p>OR <p>- B: {#SENSOR_TYPE} MATCHES_REGEX `10`</p> |
|Voltage discovery |<p>ENTITY-SENSORS-MIB::EntitySensorDataType discovery with volts filter</p> |DEPENDENT |voltage.discovery<p>**Filter**:</p>OR <p>- B: {#SENSOR_TYPE} MATCHES_REGEX `3|4`</p> |
|Entity discovery |<p>-</p> |SNMP |entity.discovery<p>**Filter**:</p>AND_OR <p>- A: {#ENT_CLASS} MATCHES_REGEX `3`</p> |
|PSU discovery |<p>-</p> |SNMP |psu.discovery<p>**Filter**:</p>AND_OR <p>- A: {#ENT_CLASS} MATCHES_REGEX `6`</p> |

## Items collected

|Group|Name|Description|Type|Key and additional info|
|-----|----|-----------|----|---------------------|
|Fans |{#SENSOR_INFO}: Fan speed |<p>MIB: ENTITY-SENSORS-MIB</p><p>The most recent measurement obtained by the agent for this sensor.</p><p>To correctly interpret the value of this object, the associated entPhySensorType,</p><p>entPhySensorScale, and entPhySensorPrecision objects must also be examined.</p> |SNMP |sensor.fan.speed[entPhySensorValue.{#SNMPINDEX}] |
|Fans |{#SENSOR_INFO}: Fan status |<p>MIB: ENTITY-SENSORS-MIB</p><p>The operational status of the sensor {#SENSOR_INFO}</p> |SNMP |sensor.fan.status[entPhySensorOperStatus.{#SNMPINDEX}] |
|Inventory |{#ENT_NAME}: Hardware model name |<p>MIB: ENTITY-MIB</p> |SNMP |system.hw.model[entPhysicalModelName.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Inventory |{#ENT_NAME}: Hardware serial number |<p>MIB: ENTITY-MIB</p> |SNMP |system.hw.serialnumber[entPhysicalSerialNum.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `6h`</p> |
|Power_supply |{#ENT_NAME}: Power supply status |<p>MIB: ENTITY-STATE-MIB</p> |SNMP |sensor.psu.status[entStateOper.{#SNMPINDEX}] |
|Temperature |{#SENSOR_INFO}: Temperature |<p>MIB: ENTITY-SENSORS-MIB</p><p>The most recent measurement obtained by the agent for this sensor.</p><p>To correctly interpret the value of this object, the associated entPhySensorType,</p><p>entPhySensorScale, and entPhySensorPrecision objects must also be examined.</p> |SNMP |sensor.temp.value[entPhySensorValue.{#SNMPINDEX}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.1`</p> |
|Temperature |{#SENSOR_INFO}: Temperature status |<p>MIB: ENTITY-SENSORS-MIB</p><p>The operational status of the sensor {#SENSOR_INFO}</p> |SNMP |sensor.temp.status[entPhySensorOperStatus.{#SNMPINDEX}] |
|Voltage |{#SENSOR_INFO}: Voltage |<p>MIB: ENTITY-SENSORS-MIB</p><p>The most recent measurement obtained by the agent for this sensor.</p><p>To correctly interpret the value of this object, the associated entPhySensorType,</p><p>entPhySensorScale, and entPhySensorPrecision objects must also be examined.</p> |SNMP |sensor.voltage.value[entPhySensorValue.{#SNMPINDEX}] |
|Zabbix_raw_items |Get sensors |<p>Gets sensors with type, description, and thresholds.</p> |SNMP |sensors.get<p>**Preprocessing**:</p><p>- JAVASCRIPT: `Text is too long. Please see the template.`</p> |

## Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----|----|----|
|{#SENSOR_INFO}: Fan speed is below the warning threshold of {#THRESHOLD_LO_WARN}rpm for 5m |<p>This trigger uses fan sensor values defined in the device.</p> |`{TEMPLATE_NAME:sensor.fan.speed[entPhySensorValue.{#SNMPINDEX}].max(5m)} < {#THRESHOLD_LO_WARN}` |WARNING |<p>**Depends on**:</p><p>- {#SENSOR_INFO}: Fan is in critical state</p><p>- {#SENSOR_INFO}: Fan speed is below the critical threshold of {#THRESHOLD_LO_CRIT}rpm for 5m</p> |
|{#SENSOR_INFO}: Fan speed is below the critical threshold of {#THRESHOLD_LO_CRIT}rpm for 5m |<p>This trigger uses fan sensor values defined in the device.</p> |`{TEMPLATE_NAME:sensor.fan.speed[entPhySensorValue.{#SNMPINDEX}].max(5m)} < {#THRESHOLD_LO_CRIT}` |HIGH |<p>**Depends on**:</p><p>- {#SENSOR_INFO}: Fan is in critical state</p> |
|{#SENSOR_INFO}: Fan speed is above the warning threshold of {#THRESHOLD_HI_WARN}rpm for 5m |<p>This trigger uses fan sensor values defined in the device.</p> |`{TEMPLATE_NAME:sensor.fan.speed[entPhySensorValue.{#SNMPINDEX}].min(5m)} > {#THRESHOLD_HI_WARN}` |WARNING |<p>**Depends on**:</p><p>- {#SENSOR_INFO}: Fan is in critical state</p><p>- {#SENSOR_INFO}: Fan speed is above the critical threshold of {#THRESHOLD_HI_CRIT}rpm for 5m</p> |
|{#SENSOR_INFO}: Fan speed is above the critical threshold of {#THRESHOLD_HI_CRIT}rpm for 5m |<p>This trigger uses fan sensor values defined in the device.</p> |`{TEMPLATE_NAME:sensor.fan.speed[entPhySensorValue.{#SNMPINDEX}].min(5m)} > {#THRESHOLD_HI_CRIT}` |HIGH |<p>**Depends on**:</p><p>- {#SENSOR_INFO}: Fan is in critical state</p> |
|{#SENSOR_INFO}: Fan is in critical state |<p>Please check the fan unit</p> |`{TEMPLATE_NAME:sensor.fan.status[entPhySensorOperStatus.{#SNMPINDEX}].count(#1,{$FAN_CRIT_STATUS},eq)}=1` |AVERAGE | |
|{#ENT_NAME}: Device has been replaced (new serial number received) |<p>Device serial number has changed. Ack to close</p> |`{TEMPLATE_NAME:system.hw.serialnumber[entPhysicalSerialNum.{#SNMPINDEX}].diff()}=1 and {TEMPLATE_NAME:system.hw.serialnumber[entPhysicalSerialNum.{#SNMPINDEX}].strlen()}>0` |INFO |<p>Manual close: YES</p> |
|{#ENT_NAME}: Power supply is in critical state |<p>Please check the power supply unit for errors</p> |`{TEMPLATE_NAME:sensor.psu.status[entStateOper.{#SNMPINDEX}].count(#1,{$PSU_CRIT_STATUS},eq)}=1` |AVERAGE | |
|{#SENSOR_INFO}: Temperature is below the warning threshold of {#THRESHOLD_LO_WARN}°C for 5m |<p>This trigger uses temperature sensor values defined in the device.</p> |`{TEMPLATE_NAME:sensor.temp.value[entPhySensorValue.{#SNMPINDEX}].max(5m)} < {#THRESHOLD_LO_WARN}` |WARNING |<p>**Depends on**:</p><p>- {#SENSOR_INFO}: Temperature is below the critical threshold of {#THRESHOLD_LO_CRIT}°C for 5m</p> |
|{#SENSOR_INFO}: Temperature is below the critical threshold of {#THRESHOLD_LO_CRIT}°C for 5m |<p>This trigger uses temperature sensor values defined in the device.</p> |`{TEMPLATE_NAME:sensor.temp.value[entPhySensorValue.{#SNMPINDEX}].max(5m)} < {#THRESHOLD_LO_CRIT}` |HIGH | |
|{#SENSOR_INFO}: Temperature is above the warning threshold of {#THRESHOLD_HI_WARN}°C for 5m |<p>This trigger uses temperature sensor values defined in the device.</p> |`{TEMPLATE_NAME:sensor.temp.value[entPhySensorValue.{#SNMPINDEX}].min(5m)} > {#THRESHOLD_HI_WARN}` |WARNING |<p>**Depends on**:</p><p>- {#SENSOR_INFO}: Temperature is above the critical threshold of {#THRESHOLD_HI_CRIT}°C for 5m</p> |
|{#SENSOR_INFO}: Temperature is above the critical threshold of {#THRESHOLD_HI_CRIT}°C for 5m |<p>This trigger uses temperature sensor values defined in the device.</p> |`{TEMPLATE_NAME:sensor.temp.value[entPhySensorValue.{#SNMPINDEX}].min(5m)} > {#THRESHOLD_HI_CRIT}` |HIGH | |
|{#SENSOR_INFO}: Voltage is below the warning threshold of {#THRESHOLD_LO_WARN}V for 5m |<p>This trigger uses voltage sensor values defined in the device.</p> |`{TEMPLATE_NAME:sensor.voltage.value[entPhySensorValue.{#SNMPINDEX}].max(5m)} < {#THRESHOLD_LO_WARN}` |WARNING |<p>**Depends on**:</p><p>- {#SENSOR_INFO}: Voltage is below the critical threshold of {#THRESHOLD_LO_CRIT}V for 5m</p> |
|{#SENSOR_INFO}: Voltage is below the critical threshold of {#THRESHOLD_LO_CRIT}V for 5m |<p>This trigger uses voltage sensor values defined in the device.</p> |`{TEMPLATE_NAME:sensor.voltage.value[entPhySensorValue.{#SNMPINDEX}].max(5m)} < {#THRESHOLD_LO_CRIT}` |HIGH | |
|{#SENSOR_INFO}: Voltage is above the warning threshold of {#THRESHOLD_HI_WARN}V for 5m |<p>This trigger uses voltage sensor values defined in the device.</p> |`{TEMPLATE_NAME:sensor.voltage.value[entPhySensorValue.{#SNMPINDEX}].min(5m)} > {#THRESHOLD_HI_WARN}` |WARNING |<p>**Depends on**:</p><p>- {#SENSOR_INFO}: Voltage is above the critical threshold of {#THRESHOLD_HI_CRIT}V for 5m</p> |
|{#SENSOR_INFO}: Voltage is above the critical threshold of {#THRESHOLD_HI_CRIT}V for 5m |<p>This trigger uses voltage sensor values defined in the device.</p> |`{TEMPLATE_NAME:sensor.voltage.value[entPhySensorValue.{#SNMPINDEX}].min(5m)} > {#THRESHOLD_HI_CRIT}` |HIGH | |

## Feedback

Please report any issues with the template at https://support.zabbix.com

