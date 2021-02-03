
# Morningstar ProStar MPPT SNMP

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
|Array |Array: Voltage |<p>MIB: PROSTAR-MPPT</p><p>Array Voltage</p><p>  Description:Array Voltage</p><p>  Scaling Factor:1.0</p><p>  Units:V</p><p>  Range:[0, 80]</p><p>  Modbus address:0x0013</p> |SNMP |array.voltage[arrayVoltage.0] |
|Array |Array: Sweep Vmp |<p>MIB: PROSTAR-MPPT</p><p>Array Vmp</p><p>  Description:Array Max. Power Point Voltage</p><p>  Scaling Factor:1.0</p><p>  Units:V</p><p>  Range:[0.0, 5000.0]</p><p>  Modbus address:0x003D</p> |SNMP |array.sweep_vmp[arrayVmp.0] |
|Array |Array: Sweep Voc |<p>MIB: PROSTAR-MPPT</p><p>Array Voc</p><p> Description:Array Open Circuit Voltage</p><p> Scaling Factor:1.0</p><p> Units:V</p><p> Range:[0.0, 80.0]</p><p> Modbus address:0x003F</p> |SNMP |array.sweep_voc[arrayVoc.0] |
|Array |Array: Sweep Pmax |<p>MIB: PROSTAR-MPPT</p><p>Array Max. Power (sweep)</p><p> Description:Array Max. Power (last sweep)</p><p> Scaling Factor:1.0</p><p> Units:W</p><p> Range:[0.0, 500]</p><p> Modbus address:0x003E</p> |SNMP |array.sweep_pmax[arrayMaxPowerSweep.0] |
|Battery |Battery: Charge State |<p>MIB: PROSTAR-MPPT</p><p>Charge State</p><p>  Description:Control State</p><p>  Modbus address:0x0021</p><p>  0: Start</p><p>  1: NightCheck</p><p>  2: Disconnect</p><p>  3: Night</p><p>  4: Fault</p><p>  5: BulkMppt</p><p>  6: Absorption</p><p>  7: Float</p><p>  8: Equalize</p><p>  9: Slave</p><p>  10: Fixed</p> |SNMP |charge.state[chargeState.0]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Battery |Battery: Target Voltage |<p>MIB: PROSTAR-MPPT</p><p>Target Voltage</p><p> Description:Target Regulation Voltage</p><p> Scaling Factor:1.0</p><p> Units:V</p><p> Range:[0.0, 80.0]</p><p> Modbus address:0x0024</p> |SNMP |target.voltage[targetVoltage.0]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Battery |Battery: Charge Current |<p>MIB: PROSTAR-MPPT</p><p>Charge Current</p><p>  Description:Charge Current</p><p>  Scaling Factor:1.0</p><p>  Units:A</p><p>  Range:[0, 40]</p><p>  Modbus address:0x0010</p> |SNMP |charge.current[chargeCurrent.0] |
|Battery |Battery: Voltage{#SINGLETON} |<p>MIB: PROSTAR-MPPT</p><p>Battery Terminal Voltage</p><p>Description:Battery  Terminal Voltage</p><p>Scaling Factor:1.0</p><p>Units:V</p><p>Range:[0.0, 80.0]</p><p>Modbus address:0x0012</p> |SNMP |battery.voltage[batteryTerminalVoltage.0{#SINGLETON}] |
|Counter |Counter: Charge Amp-hours |<p>MIB: PROSTAR-MPPT</p><p>Ah Charge (Resettable)</p><p> Description:Ah Charge (Resettable)</p><p> Scaling Factor:0.1</p><p> Units:Ah</p><p> Range:[0.0, 4294967294]</p><p> Modbus addresses:H=0x0026 L=0x0027</p> |SNMP |counter.charge_amp_hours[ahChargeResettable.0]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.1`</p> |
|Counter |Counter: Charge KW-hours |<p>MIB: PROSTAR-MPPT</p><p>kWh Charge (Resettable)</p><p>Description:Kilowatt Hours Charge (Resettable)</p><p>Scaling Factor:1.0</p><p>Units:kWh</p><p>Range:[0.0, 65535]</p><p>Modbus address:0x002A</p> |SNMP |counter.charge_kw_hours[kwhChargeResettable.0] |
|Counter |Counter: Load Amp-hours |<p>MIB: PROSTAR-MPPT</p><p>Description:Ah Load (Resettable)</p><p>Scaling Factor:0.1</p><p>Units:Ah</p><p>Range:[0.0, 4294967294]</p><p>Modbus addresses:H=0x0032 L=0x0033</p> |SNMP |counter.load_amp_hours[ahLoadResettable.0]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.1`</p> |
|Load |Load: State |<p>MIB: PROSTAR-MPPT</p><p>Load State</p><p> Description:Load State</p><p> Modbus address:0x002E</p><p> 0: Start</p><p>1: Normal</p><p>2: LvdWarning</p><p>3: Lvd</p><p>4: Fault</p><p>5: Disconnect</p><p>6: NormalOff</p><p>7: Override</p><p>8: NotUsed</p> |SNMP |load.state[loadState.0]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Load |Load: Voltage |<p>MIB: PROSTAR-MPPT</p><p>Load Voltage</p><p> Description:Load Voltage</p><p> Scaling Factor:1.0</p><p> Units:V</p><p> Range:[0, 80]</p><p> Modbus address:0x0014</p> |SNMP |load.voltage[loadVoltage.0] |
|Load |Load: Current |<p>MIB: PROSTAR-MPPT</p><p>Load Current</p><p> Description:Load Current</p><p> Scaling Factor:1.0</p><p> Units:A</p><p> Range:[0, 60]</p><p> Modbus address:0x0016</p> |SNMP |load.current[loadCurrent.0] |
|Status |Status: Uptime |<p>Device uptime in seconds</p> |SNMP |status.uptime<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.01`</p> |
|Status |Status: Array Faults |<p>MIB: PROSTAR-MPPT</p><p>Description:Array Faults</p><p>Modbus address:0x0022</p> |SNMP |status.array_faults[arrayFaults.0]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p><p>- JAVASCRIPT: `Text is too long. Please see the template.`</p> |
|Status |Status: Load Faults |<p>MIB: PROSTAR-MPPT</p><p>Description:Array Faults</p><p>Modbus address:0x0022</p> |SNMP |status.load_faults[loadFaults.0]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p><p>- JAVASCRIPT: `Text is too long. Please see the template.`</p> |
|Status |Status: Alarms |<p>MIB: PROSTAR-MPPT</p><p>Description:Alarms</p><p>Modbus addresses:H=0x0038 L=0x0039</p> |SNMP |status.alarms[alarms.0]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p><p>- JAVASCRIPT: `Text is too long. Please see the template.`</p> |
|Temperature |Temperature: Ambient |<p>MIB: PROSTAR-MPPT</p><p>Ambient Temperature</p><p> Description:Ambient Temperature</p><p> Scaling Factor:1.0</p><p> Units:deg C</p><p> Range:[-128, 127]</p><p> Modbus address:0x001C</p> |SNMP |temp.ambient[ambientTemperature.0] |
|Temperature |Temperature: Battery |<p>MIB: PROSTAR-MPPT</p><p>Battery Temperature</p><p>  Description:Battery Temperature</p><p>  Scaling Factor:1.0</p><p>  Units:deg C</p><p>  Range:[-128, 127]</p><p>  Modbus address:0x001B</p> |SNMP |temp.battery[batteryTemperature.0] |
|Temperature |Temperature: Heatsink |<p>MIB: PROSTAR-MPPT</p><p>Heatsink Temperature</p><p> Description:Heatsink Temperature</p><p> Scaling Factor:1.0</p><p> Units:deg C</p><p> Range:[-128, 127]</p><p> Modbus address:0x001A</p> |SNMP |temp.heatsink[heatsinkTemperature.0] |
|Zabbix_raw_items |Battery: Battery Voltage discovery |<p>MIB: PROSTAR-MPPT</p> |SNMP |battery.voltage.discovery[batteryTerminalVoltage.0] |

## Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----|----|----|
|Battery: Device charge in warning state |<p>-</p> |`{TEMPLATE_NAME:charge.state[chargeState.0].last()}={$CHARGE.STATE.WARN}` |WARNING |<p>**Depends on**:</p><p>- Battery: Device charge in critical state</p> |
|Battery: Device charge in critical state |<p>-</p> |`{TEMPLATE_NAME:charge.state[chargeState.0].last()}={$CHARGE.STATE.CRIT}` |HIGH | |
|Battery: Low battery voltage (below {#VOLTAGE.MIN.WARN}V for 5m) |<p>-</p> |`{TEMPLATE_NAME:battery.voltage[batteryTerminalVoltage.0{#SINGLETON}].max(5m)}<{#VOLTAGE.MIN.WARN}` |WARNING |<p>**Depends on**:</p><p>- Battery: Critically low battery voltage (below {#VOLTAGE.MIN.CRIT}V for 5m)</p> |
|Battery: Critically low battery voltage (below {#VOLTAGE.MIN.CRIT}V for 5m) |<p>-</p> |`{TEMPLATE_NAME:battery.voltage[batteryTerminalVoltage.0{#SINGLETON}].max(5m)}<{#VOLTAGE.MIN.CRIT}` |HIGH | |
|Battery: High battery voltage (over {#VOLTAGE.MAX.WARN}V for 5m) |<p>-</p> |`{TEMPLATE_NAME:battery.voltage[batteryTerminalVoltage.0{#SINGLETON}].min(5m)}>{#VOLTAGE.MAX.WARN}` |WARNING |<p>**Depends on**:</p><p>- Battery: Critically high battery voltage (over {#VOLTAGE.MAX.CRIT}V for 5m)</p> |
|Battery: Critically high battery voltage (over {#VOLTAGE.MAX.CRIT}V for 5m) |<p>-</p> |`{TEMPLATE_NAME:battery.voltage[batteryTerminalVoltage.0{#SINGLETON}].min(5m)}>{#VOLTAGE.MAX.CRIT}` |HIGH | |
|Load: Device load in warning state |<p>-</p> |`{TEMPLATE_NAME:load.state[loadState.0].last()}={$LOAD.STATE.WARN:"lvdWarning"}  or {TEMPLATE_NAME:load.state[loadState.0].last()}={$LOAD.STATE.WARN:"override"}` |WARNING |<p>**Depends on**:</p><p>- Load: Device load in critical state</p> |
|Load: Device load in critical state |<p>-</p> |`{TEMPLATE_NAME:load.state[loadState.0].last()}={$LOAD.STATE.CRIT:"lvd"} or {TEMPLATE_NAME:load.state[loadState.0].last()}={$LOAD.STATE.CRIT:"fault"}` |HIGH | |
|Status: Device has been restarted (uptime < 10m) |<p>Uptime is less than 10 minutes</p> |`{TEMPLATE_NAME:status.uptime.last()}<10m` |INFO |<p>Manual close: YES</p> |
|Status: Failed to fetch data (or no data for 5m) |<p>Zabbix has not received data for items for the last 5 minutes</p> |`{TEMPLATE_NAME:status.uptime.nodata(5m)}=1` |WARNING |<p>Manual close: YES</p> |
|Status: Device has "overcurrent" array faults flag |<p>-</p> |`{TEMPLATE_NAME:status.array_faults[arrayFaults.0].count(#3,"overcurrent","like")}=2` |HIGH | |
|Status: Device has "mosfetSShorted" array faults flag |<p>-</p> |`{TEMPLATE_NAME:status.array_faults[arrayFaults.0].count(#3,"mosfetSShorted","like")}=2` |HIGH | |
|Status: Device has "software" array faults flag |<p>-</p> |`{TEMPLATE_NAME:status.array_faults[arrayFaults.0].count(#3,"software","like")}=2` |HIGH | |
|Status: Device has "batteryHvd" array faults flag |<p>-</p> |`{TEMPLATE_NAME:status.array_faults[arrayFaults.0].count(#3,"batteryHvd","like")}=2` |HIGH | |
|Status: Device has "arrayHvd" array faults flag |<p>-</p> |`{TEMPLATE_NAME:status.array_faults[arrayFaults.0].count(#3,"arrayHvd","like")}=2` |HIGH | |
|Status: Device has "customSettingsEdit" array faults flag |<p>-</p> |`{TEMPLATE_NAME:status.array_faults[arrayFaults.0].count(#3,"customSettingsEdit","like")}=2` |HIGH | |
|Status: Device has "rtsShorted" array faults flag |<p>-</p> |`{TEMPLATE_NAME:status.array_faults[arrayFaults.0].count(#3,"rtsShorted","like")}=2` |HIGH | |
|Status: Device has "rtsNoLongerValid" array faults flag |<p>-</p> |`{TEMPLATE_NAME:status.array_faults[arrayFaults.0].count(#3,"rtsNoLongerValid","like")}=2` |HIGH | |
|Status: Device has "localTempSensorDamaged" array faults flag |<p>-</p> |`{TEMPLATE_NAME:status.array_faults[arrayFaults.0].count(#3,"localTempSensorDamaged","like")}=2` |HIGH | |
|Status: Device has "batteryLowVoltageDisconnect" array faults flag |<p>-</p> |`{TEMPLATE_NAME:status.array_faults[arrayFaults.0].count(#3,"batteryLowVoltageDisconnect","like")}=2` |HIGH | |
|Status: Device has "slaveTimeout" array faults flag |<p>-</p> |`{TEMPLATE_NAME:status.array_faults[arrayFaults.0].count(#3,"slaveTimeout","like")}=2` |HIGH | |
|Status: Device has "dipSwitchChanged" array faults flag |<p>-</p> |`{TEMPLATE_NAME:status.array_faults[arrayFaults.0].count(#3,"dipSwitchChanged","like")}=2` |HIGH | |
|Status: Device has "externalShortCircuit" load faults flag |<p>-</p> |`{TEMPLATE_NAME:status.load_faults[loadFaults.0].count(#3,"externalShortCircuit","like")}=2` |HIGH | |
|Status: Device has "overcurrent" load faults flag |<p>-</p> |`{TEMPLATE_NAME:status.load_faults[loadFaults.0].count(#3,"overcurrent","like")}=2` |HIGH | |
|Status: Device has "mosfetShorted" load faults flag |<p>-</p> |`{TEMPLATE_NAME:status.load_faults[loadFaults.0].count(#3,"mosfetShorted","like")}=2` |HIGH | |
|Status: Device has "software" load faults flag |<p>-</p> |`{TEMPLATE_NAME:status.load_faults[loadFaults.0].count(#3,"software","like")}=2` |HIGH | |
|Status: Device has "loadHvd" load faults flag |<p>-</p> |`{TEMPLATE_NAME:status.load_faults[loadFaults.0].count(#3,"loadHvd","like")}=2` |HIGH | |
|Status: Device has "highTempDisconnect" load faults flag |<p>-</p> |`{TEMPLATE_NAME:status.load_faults[loadFaults.0].count(#3,"highTempDisconnect","like")}=2` |HIGH | |
|Status: Device has "dipSwitchChanged" load faults flag |<p>-</p> |`{TEMPLATE_NAME:status.load_faults[loadFaults.0].count(#3,"dipSwitchChanged","like")}=2` |HIGH | |
|Status: Device has "customSettingsEdit" load faults flag |<p>-</p> |`{TEMPLATE_NAME:status.load_faults[loadFaults.0].count(#3,"customSettingsEdit","like")}=2` |HIGH | |
|Status: Device has "rtsShorted" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"rtsShorted","like")}=2` |WARNING | |
|Status: Device has "rtsDisconnected" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"rtsDisconnected","like")}=2` |WARNING | |
|Status: Device has "heatsinkTempSensorOpen" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"heatsinkTempSensorOpen","like")}=2` |WARNING | |
|Status: Device has "heatsinkTempSensorShorted" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"heatsinkTempSensorShorted","like")}=2` |WARNING | |
|Status: Device has "heatsinkTempLimit" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"heatsinkTempLimit","like")}=2` |WARNING | |
|Status: Device has "inductorTempSensorOpen" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"inductorTempSensorOpen","like")}=2` |WARNING | |
|Status: Device has "inductorTempSensorShorted" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"inductorTempSensorShorted","like")}=2` |WARNING | |
|Status: Device has "inductorTempLimit" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"inductorTempLimit","like")}=2` |WARNING | |
|Status: Device has "currentLimit" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"currentLimit","like")}=2` |WARNING | |
|Status: Device has "currentMeasurementError" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"currentMeasurementError","like")}=2` |WARNING | |
|Status: Device has "batterySenseOutOfRange" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"batterySenseOutOfRange","like")}=2` |WARNING | |
|Status: Device has "batterySenseDisconnected" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"batterySenseDisconnected","like")}=2` |WARNING | |
|Status: Device has "uncalibrated" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"uncalibrated","like")}=2` |WARNING | |
|Status: Device has "tb5v" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"tb5v","like")}=2` |WARNING | |
|Status: Device has "fp10SupplyOutOfRange" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"fp10SupplyOutOfRange","like")}=2` |WARNING | |
|Status: Device has "mosfetOpen" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"mosfetOpen","like")}=2` |WARNING | |
|Status: Device has "arrayCurrentOffset" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"arrayCurrentOffset","like")}=2` |WARNING | |
|Status: Device has "loadCurrentOffset" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"loadCurrentOffset","like")}=2` |WARNING | |
|Status: Device has "p33SupplyOutOfRange" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"p33SupplyOutOfRange","like")}=2` |WARNING | |
|Status: Device has "p12SupplyOutOfRange" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"p12SupplyOutOfRange","like")}=2` |WARNING | |
|Status: Device has "hightInputVoltageLimit" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"hightInputVoltageLimit","like")}=2` |WARNING | |
|Status: Device has "controllerReset" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"controllerReset","like")}=2` |WARNING | |
|Status: Device has "loadLvd" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"loadLvd","like")}=2` |WARNING | |
|Status: Device has "logTimeout" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"logTimeout","like")}=2` |WARNING | |
|Status: Device has "eepromAccessFailure" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"eepromAccessFailure","like")}=2` |WARNING | |
|Temperature: Low battery temperature (below {$BATTERY.TEMP.MIN.WARN}C for 5m) |<p>-</p> |`{TEMPLATE_NAME:temp.battery[batteryTemperature.0].max(5m)}<{$BATTERY.TEMP.MIN.WARN}` |WARNING |<p>**Depends on**:</p><p>- Temperature: Critically low battery temperature (below {$BATTERY.TEMP.MIN.WARN}C for 5m)</p> |
|Temperature: Critically low battery temperature (below {$BATTERY.TEMP.MIN.WARN}C for 5m) |<p>-</p> |`{TEMPLATE_NAME:temp.battery[batteryTemperature.0].max(5m)}<{$BATTERY.TEMP.MIN.CRIT}` |HIGH | |
|Temperature: High battery temperature (over {$BATTERY.TEMP.MAX.WARN}C for 5m) |<p>-</p> |`{TEMPLATE_NAME:temp.battery[batteryTemperature.0].min(5m)}>{$BATTERY.TEMP.MAX.WARN}` |WARNING |<p>**Depends on**:</p><p>- Temperature: Critically high battery temperature (over {$BATTERY.TEMP.MAX.CRIT}C for 5m)</p> |
|Temperature: Critically high battery temperature (over {$BATTERY.TEMP.MAX.CRIT}C for 5m) |<p>-</p> |`{TEMPLATE_NAME:temp.battery[batteryTemperature.0].min(5m)}>{$BATTERY.TEMP.MAX.CRIT}` |HIGH | |

## Feedback

Please report any issues with the template at https://support.zabbix.com

