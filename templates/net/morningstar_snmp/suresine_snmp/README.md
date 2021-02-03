
# Morningstar SureSine SNMP

## Overview

For Zabbix version: 5.0 and higher  

## Setup

> See [Zabbix template operation](https://www.zabbix.com/documentation/5.0/manual/config/templates_out_of_the_box/zabbix_agent) for basic instructions.

Refer to the vendor documentation.

## Zabbix configuration

No specific Zabbix configuration is required.

### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$BATTERY.TEMP.MAX.CRIT} |<p>Battery high temperature critical value</p> |`60` |
|{$BATTERY.TEMP.MAX.WARN} |<p>Battery high temperature warning value</p> |`45` |
|{$BATTERY.TEMP.MIN.CRIT} |<p>Battery low temperature critical value</p> |`-20` |
|{$BATTERY.TEMP.MIN.WARN} |<p>Battery low temperature warning value</p> |`0` |
|{$CHARGE.STATE.CRIT} |<p>fault</p> |`4` |
|{$CHARGE.STATE.WARN} |<p>disconnect</p> |`2` |
|{$LOAD.STATE.CRIT:"fault"} |<p>fault</p> |`4` |
|{$LOAD.STATE.CRIT:"lvd"} |<p>lvd</p> |`3` |
|{$LOAD.STATE.WARN:"disconnect"} |<p>disconnect</p> |`5` |
|{$LOAD.STATE.WARN:"lvdWarning"} |<p>lvdWarning</p> |`2` |
|{$LOAD.STATE.WARN:"override"} |<p>override</p> |`7` |
|{$VOLTAGE.MAX.CRIT} | |`` |
|{$VOLTAGE.MAX.WARN} | |`` |
|{$VOLTAGE.MIN.CRIT} | |`` |
|{$VOLTAGE.MIN.WARN} | |`` |

## Template links

There are no template links in this template.

## Discovery rules

|Name|Description|Type|Key and additional info|
|----|-----------|----|----|
|Battery voltage discovery |<p>Discovery for battery voltage triggers</p> |DEPENDENT |battery.voltage.discovery<p>**Preprocessing**:</p><p>- JAVASCRIPT: `Text is too long. Please see the template.`</p> |

## Items collected

|Group|Name|Description|Type|Key and additional info|
|-----|----|-----------|----|---------------------|
|Battery |Battery: Voltage{#SINGLETON} |<p>MIB: SURESINE</p><p>Description:Battery Voltage(slow)</p><p>Scaling Factor:0.0002581787109375</p><p>Units:V</p><p>Range:[0.0, 17.0]</p><p>Modbus address:0x0004</p> |SNMP |battery.voltage[batteryVoltageSlow.0{#SINGLETON}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `2.581787109375E-4`</p><p>- REGEX: `^(\d+)(\.\d{1,2})? \1\2`</p> |
|Load |Load: State |<p>MIB: SURESINE</p><p>Description:Load State</p><p>Modbus address:0x000B</p><p> 0: Start</p><p>1: LoadOn</p><p>2: LvdWarning</p><p>3: LowVoltageDisconnect</p><p>4: Fault</p><p>5: Disconnect</p><p>6: NormalOff</p><p>7: UnknownState</p><p>8: Standby</p> |SNMP |load.state[loadState.0]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Load |Load: A/C Current |<p>MIB: SURESINE</p><p>Description:AC Output Current</p><p>Scaling Factor:0.0001953125</p><p>Units:A</p><p>Range:[0.0, 17]</p><p>Modbus address:0x0005</p> |SNMP |load.ac_current[acCurrent.0]<p>**Preprocessing**:</p><p>- MULTIPLIER: `1.953125E-4`</p><p>- REGEX: `^(\d+)(\.\d{1,2})? \1\2`</p> |
|Status |Status: Uptime |<p>Device uptime in seconds</p> |SNMP |status.uptime<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.01`</p> |
|Status |Status: Faults |<p>MIB: SURESINE</p><p>Description:Faults</p><p>Modbus address:0x0007</p> |SNMP |status.faults[faults.0]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p><p>- JAVASCRIPT: `Text is too long. Please see the template.`</p> |
|Status |Status: Alarms |<p>MIB: SURESINE</p><p>Description:Faults</p><p>Modbus address:0x0007</p> |SNMP |status.alarms[alarms.0]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p><p>- JAVASCRIPT: `Text is too long. Please see the template.`</p> |
|Temperature |Temperature: Heatsink |<p>MIB: SURESINE</p><p>Description:Heatsink Temperature</p><p>Scaling Factor:1</p><p>Units:C</p><p>Range:[-128, 127]</p><p>Modbus address:0x0006</p> |SNMP |temp.heatsink[heatsinkTemperature.0] |
|Zabbix_raw_items |Battery: Battery Voltage discovery |<p>MIB: SURESINE</p> |SNMP |battery.voltage.discovery[batteryVoltageSlow.0]<p>**Preprocessing**:</p><p>- MULTIPLIER: `2.581787109375E-4`</p><p>- REGEX: `^(\d+)(\.\d{1,2})? \1\2`</p> |

## Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----|----|----|
|Battery: Low battery voltage (below {#VOLTAGE.MIN.WARN}V for 5m) |<p>-</p> |`{TEMPLATE_NAME:battery.voltage[batteryVoltageSlow.0{#SINGLETON}].max(5m)}<{#VOLTAGE.MIN.WARN}` |WARNING |<p>**Depends on**:</p><p>- Battery: Critically low battery voltage (below {#VOLTAGE.MIN.CRIT}V for 5m)</p> |
|Battery: Critically low battery voltage (below {#VOLTAGE.MIN.CRIT}V for 5m) |<p>-</p> |`{TEMPLATE_NAME:battery.voltage[batteryVoltageSlow.0{#SINGLETON}].max(5m)}<{#VOLTAGE.MIN.CRIT}` |HIGH | |
|Battery: High battery voltage (over {#VOLTAGE.MAX.WARN}V for 5m) |<p>-</p> |`{TEMPLATE_NAME:battery.voltage[batteryVoltageSlow.0{#SINGLETON}].min(5m)}>{#VOLTAGE.MAX.WARN}` |WARNING |<p>**Depends on**:</p><p>- Battery: Critically high battery voltage (over {#VOLTAGE.MAX.CRIT}V for 5m)</p> |
|Battery: Critically high battery voltage (over {#VOLTAGE.MAX.CRIT}V for 5m) |<p>-</p> |`{TEMPLATE_NAME:battery.voltage[batteryVoltageSlow.0{#SINGLETON}].min(5m)}>{#VOLTAGE.MAX.CRIT}` |HIGH | |
|Load: Device load in warning state |<p>-</p> |`{TEMPLATE_NAME:load.state[loadState.0].last()}={$LOAD.STATE.WARN:"lvdWarning"}  or {TEMPLATE_NAME:load.state[loadState.0].last()}={$LOAD.STATE.WARN:"override"}` |WARNING |<p>**Depends on**:</p><p>- Load: Device load in critical state</p> |
|Load: Device load in critical state |<p>-</p> |`{TEMPLATE_NAME:load.state[loadState.0].last()}={$LOAD.STATE.CRIT:"lvd"} or {TEMPLATE_NAME:load.state[loadState.0].last()}={$LOAD.STATE.CRIT:"fault"}` |HIGH | |
|Status: Device has been restarted (uptime < 10m) |<p>Uptime is less than 10 minutes</p> |`{TEMPLATE_NAME:status.uptime.last()}<10m` |INFO |<p>Manual close: YES</p> |
|Status: Failed to fetch data (or no data for 5m) |<p>Zabbix has not received data for items for the last 5 minutes</p> |`{TEMPLATE_NAME:status.uptime.nodata(5m)}=1` |WARNING |<p>Manual close: YES</p> |
|Status: Device has "reset" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"reset","like")}=2` |HIGH | |
|Status: Device has "overcurrent" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"overcurrent","like")}=2` |HIGH | |
|Status: Device has "unknownFault" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"unknownFault","like")}=2` |HIGH | |
|Status: Device has "software" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"software","like")}=2` |HIGH | |
|Status: Device has "highVoltageDisconnect" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"highVoltageDisconnect","like")}=2` |HIGH | |
|Status: Device has "suresineHot" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"suresineHot","like")}=2` |HIGH | |
|Status: Device has "dipSwitchChanged" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"dipSwitchChanged","like")}=2` |HIGH | |
|Status: Device has "customSettingsEdit" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"customSettingsEdit","like")}=2` |HIGH | |
|Status: Device has "heatsinkTempSensorOpen" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"heatsinkTempSensorOpen","like")}=2` |WARNING | |
|Status: Device has "heatsinkTempSensorShort" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"heatsinkTempSensorShort","like")}=2` |WARNING | |
|Status: Device has "unknownAlarm" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"unknownAlarm","like")}=2` |WARNING | |
|Status: Device has "suresineHot" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"suresineHot","like")}=2` |WARNING | |

## Feedback

Please report any issues with the template at https://support.zabbix.com

