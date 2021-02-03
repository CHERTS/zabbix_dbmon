
# Morningstar TriStar PWM SNMP

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
|Charge mode discovery |<p>Discovery for device in charge mode</p> |DEPENDENT |controlmode.charge.discovery<p>**Preprocessing**:</p><p>- JAVASCRIPT: `return JSON.stringify(parseInt(value) === 0 ? [{'{#SINGLETON}': ''}] : []);`</p> |
|Load mode discovery |<p>Discovery for device in load mode</p> |DEPENDENT |controlmode.load.discovery<p>**Preprocessing**:</p><p>- JAVASCRIPT: `return JSON.stringify(parseInt(value) === 1 ? [{'{#SINGLETON}': ''}] : []);`</p> |
|Diversion mode discovery |<p>Discovery for device in diversion mode</p> |DEPENDENT |controlmode.diversion.discovery<p>**Preprocessing**:</p><p>- JAVASCRIPT: `return JSON.stringify(parseInt(value) === 2 ? [{'{#SINGLETON}': ''}] : []);`</p> |
|Charge + Diversion mode discovery |<p>Discovery for device in charge and diversion modes</p> |DEPENDENT |controlmode.charge_diversion.discovery<p>**Preprocessing**:</p><p>- JAVASCRIPT: `Text is too long. Please see the template.`</p> |
|Load + Diversion mode discovery |<p>Discovery for device in load and diversion modes</p> |DEPENDENT |controlmode.load_diversion.discovery<p>**Preprocessing**:</p><p>- JAVASCRIPT: `Text is too long. Please see the template.`</p> |

## Items collected

|Group|Name|Description|Type|Key and additional info|
|-----|----|-----------|----|---------------------|
|Array |Array: Voltage{#SINGLETON} |<p>MIB: TRISTAR</p><p>Description:Array/Load Voltage</p><p>Scaling Factor:0.00424652099609375</p><p>Units:V</p><p>Range:[0, 80]</p><p>Modbus address:0x000A</p> |SNMP |array.voltage[arrayloadVoltage.0{#SINGLETON}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.004246520996`</p><p>- REGEX: `^(\d+)(\.\d{1,2})? \1\2`</p> |
|Battery |Battery: Voltage{#SINGLETON} |<p>MIB: TRISTAR</p><p>Description:Battery voltage</p><p>Scaling Factor:0.002950042724609375</p><p>Units:V</p><p>Range:[0.0, 80.0]</p><p>Modbus address:0x0008</p> |SNMP |battery.voltage[batteryVoltage.0{#SINGLETON}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.002950042725`</p><p>- REGEX: `^(\d+)(\.\d{1,2})? \1\2`</p> |
|Battery |Battery: Charge Current{#SINGLETON} |<p>MIB: TRISTAR</p><p>Description:Charge Current</p><p>Scaling Factor:0.002034515380859375</p><p>Units:A</p><p>Range:[0, 60]</p><p>Modbus address:0x000B</p> |SNMP |charge.current[chargeCurrent.0{#SINGLETON}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.002034515381`</p><p>- REGEX: `^(\d+)(\.\d{1,2})? \1\2`</p> |
|Battery |Battery: Charge State{#SINGLETON} |<p>MIB: TRISTAR</p><p>Description:Control State</p><p>Modbus address:0x001B</p> |SNMP |charge.state[controlState.0{#SINGLETON}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Battery |Battery: Target Voltage{#SINGLETON} |<p>MIB: TRISTAR</p><p>Description:Target Regulation Voltage</p><p>Scaling Factor:0.002950042724609375</p><p>Units:V</p><p>Range:[0.0, 80.0]</p><p>Modbus address:0x0010</p> |SNMP |target.voltage[targetVoltage.0{#SINGLETON}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.002950042725`</p><p>- REGEX: `^(\d+)(\.\d{1,2})? \1\2`</p> |
|Counter |Counter: KW-hours |<p>MIB: TRISTAR</p><p>Description:Kilowatt Hours</p><p>Scaling Factor:1.0</p><p>Units:kWh</p><p>Range:[0.0, 5000.0]</p><p>Modbus address:0x001E</p> |SNMP |counter.charge_kw_hours[kilowattHours.0]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.001`</p><p>- REGEX: `^(\d+)(\.\d{1,2})? \1\2`</p> |
|Counter |Counter: Amp-hours |<p>MIB: TRISTAR</p><p>Description:Ah (Resettable)</p><p>Scaling Factor:0.1</p><p>Units:Ah</p><p>Range:[0.0, 50000.0]</p><p>Modbus addresses:H=0x0011 L=0x0012</p> |SNMP |counter.charge_amp_hours[ahResettable.0]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.1`</p> |
|Load |Load: State{#SINGLETON} |<p>MIB: TRISTAR</p><p>Description:Load State</p><p>Modbus address:0x001B</p><p>0: Start</p><p>1: Normal</p><p>2: LvdWarning</p><p>3: Lvd</p><p>4: Fault</p><p>5: Disconnect</p><p>6: LvdWarning1</p><p>7: OverrideLvd</p><p>8: Equalize</p> |SNMP |load.state[loadState.0{#SINGLETON}]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p> |
|Load |Load: PWM Duty Cycle{#SINGLETON} |<p>MIB: TRISTAR</p><p>Description:PWM Duty Cycle</p><p>Scaling Factor:0.392156862745098</p><p>Units:%</p><p>Range:[0.0, 100.0]</p><p>Modbus address:0x001C</p> |SNMP |diversion.pwm_duty_cycle[pwmDutyCycle.0{#SINGLETON}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.3921568627`</p><p>- REGEX: `^(\d+)(\.\d{1,2})? \1\2`</p> |
|Load |Load: Current{#SINGLETON} |<p>MIB: TRISTAR</p><p>Description:Load Current</p><p>Scaling Factor:0.00966400146484375</p><p>Units:A</p><p>Range:[0, 60]</p><p>Modbus address:0x000C</p> |SNMP |load.current[loadCurrent.0{#SINGLETON}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.009664001465`</p><p>- REGEX: `^(\d+)(\.\d{1,2})? \1\2`</p> |
|Load |Load: Voltage{#SINGLETON} |<p>MIB: TRISTAR</p><p>Description:Array/Load Voltage</p><p>Scaling Factor:0.00424652099609375</p><p>Units:V</p><p>Range:[0, 80]</p><p>Modbus address:0x000A</p> |SNMP |load.voltage[arrayloadVoltage.0{#SINGLETON}]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.004246520996`</p><p>- REGEX: `^(\d+)(\.\d{1,2})? \1\2`</p> |
|Status |Status: Uptime |<p>Device uptime in seconds</p> |SNMP |status.uptime<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.01`</p> |
|Status |Status: Control Mode |<p>MIB: TRISTAR</p><p>Description:Control Mode</p><p>Modbus address:0x001A</p><p>0: charge</p><p>1: loadControl</p><p>2: diversion</p><p>3: lighting</p> |SNMP |control.mode[controlMode.0] |
|Status |Status: Faults |<p>MIB: TRISTAR</p><p>Description:Battery voltage</p><p>Scaling Factor:0.002950042724609375</p><p>Units:V</p><p>Range:[0.0, 80.0]</p><p>Modbus address:0x0008</p> |SNMP |status.faults[faults.0]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p><p>- JAVASCRIPT: `Text is too long. Please see the template.`</p> |
|Status |Status: Alarms |<p>MIB: TRISTAR</p><p>Description:Alarms</p><p>Modbus addresses:H=0x001D L=0x0017</p> |SNMP |status.alarms[alarms.0]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `1h`</p><p>- JAVASCRIPT: `Text is too long. Please see the template.`</p> |
|Temperature |Temperature: Battery |<p>MIB: TRISTAR</p><p>Description:Battery Temperature</p><p>Scaling Factor:1.0</p><p>Units:deg C</p><p>Range:[-40, 120]</p><p>Modbus address:0x000F</p> |SNMP |temp.battery[batteryTemperature.0] |
|Temperature |Temperature: Heatsink |<p>MIB: TRISTAR</p><p>Description:Heatsink Temperature</p><p>Scaling Factor:1.0</p><p>Units:deg C</p><p>Range:[-40, 120]</p><p>Modbus address:0x000E</p> |SNMP |temp.heatsink[heatsinkTemperature.0] |
|Zabbix_raw_items |Battery: Battery Voltage discovery |<p>MIB: TRISTAR</p><p>Description:Battery voltage</p><p>Scaling Factor:0.002950042724609375</p><p>Units:V</p><p>Range:[0.0, 80.0]</p><p>Modbus address:0x0008</p> |SNMP |battery.voltage.discovery[batteryVoltage.0]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.002950042725`</p> |

## Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----|----|----|
|Battery: Low battery voltage (below {#VOLTAGE.MIN.WARN}V for 5m) |<p>-</p> |`{TEMPLATE_NAME:battery.voltage[batteryVoltage.0{#SINGLETON}].max(5m)}<{#VOLTAGE.MIN.WARN}` |WARNING |<p>**Depends on**:</p><p>- Battery: Critically low battery voltage (below {#VOLTAGE.MIN.CRIT}V for 5m)</p> |
|Battery: Critically low battery voltage (below {#VOLTAGE.MIN.CRIT}V for 5m) |<p>-</p> |`{TEMPLATE_NAME:battery.voltage[batteryVoltage.0{#SINGLETON}].max(5m)}<{#VOLTAGE.MIN.CRIT}` |HIGH | |
|Battery: High battery voltage (over {#VOLTAGE.MAX.WARN}V for 5m) |<p>-</p> |`{TEMPLATE_NAME:battery.voltage[batteryVoltage.0{#SINGLETON}].min(5m)}>{#VOLTAGE.MAX.WARN}` |WARNING |<p>**Depends on**:</p><p>- Battery: Critically high battery voltage (over {#VOLTAGE.MAX.CRIT}V for 5m)</p> |
|Battery: Critically high battery voltage (over {#VOLTAGE.MAX.CRIT}V for 5m) |<p>-</p> |`{TEMPLATE_NAME:battery.voltage[batteryVoltage.0{#SINGLETON}].min(5m)}>{#VOLTAGE.MAX.CRIT}` |HIGH | |
|Battery: Device charge in warning state |<p>-</p> |`{TEMPLATE_NAME:charge.state[controlState.0{#SINGLETON}].last()}={$CHARGE.STATE.WARN}` |WARNING |<p>**Depends on**:</p><p>- Battery: Device charge in critical state</p> |
|Battery: Device charge in critical state |<p>-</p> |`{TEMPLATE_NAME:charge.state[controlState.0{#SINGLETON}].last()}={$CHARGE.STATE.CRIT}` |HIGH | |
|Load: Device load in warning state |<p>-</p> |`{TEMPLATE_NAME:load.state[loadState.0{#SINGLETON}].last()}={$LOAD.STATE.WARN:"lvdWarning"}  or {TEMPLATE_NAME:load.state[loadState.0{#SINGLETON}].last()}={$LOAD.STATE.WARN:"override"}` |WARNING |<p>**Depends on**:</p><p>- Load: Device load in critical state</p> |
|Load: Device load in critical state |<p>-</p> |`{TEMPLATE_NAME:load.state[loadState.0{#SINGLETON}].last()}={$LOAD.STATE.CRIT:"lvd"} or {TEMPLATE_NAME:load.state[loadState.0{#SINGLETON}].last()}={$LOAD.STATE.CRIT:"fault"}` |HIGH | |
|Status: Device has been restarted (uptime < 10m) |<p>Uptime is less than 10 minutes</p> |`{TEMPLATE_NAME:status.uptime.last()}<10m` |INFO |<p>Manual close: YES</p> |
|Status: Failed to fetch data (or no data for 5m) |<p>Zabbix has not received data for items for the last 5 minutes</p> |`{TEMPLATE_NAME:status.uptime.nodata(5m)}=1` |WARNING |<p>Manual close: YES</p> |
|Status: Device has "externalShort" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"externalShort","like")}=2` |HIGH | |
|Status: Device has "overcurrent" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"overcurrent","like")}=2` |HIGH | |
|Status: Device has "mosfetSShorted" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"mosfetSShorted","like")}=2` |HIGH | |
|Status: Device has "softwareFault" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"softwareFault","like")}=2` |HIGH | |
|Status: Device has "highVoltageDisconnect" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"highVoltageDisconnect","like")}=2` |HIGH | |
|Status: Device has "tristarHot" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"tristarHot","like")}=2` |HIGH | |
|Status: Device has "dipSwitchChange" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"dipSwitchChange","like")}=2` |HIGH | |
|Status: Device has "customSettingsEdit" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"customSettingsEdit","like")}=2` |HIGH | |
|Status: Device has "reset" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"reset","like")}=2` |HIGH | |
|Status: Device has "systemMiswire" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"systemMiswire","like")}=2` |HIGH | |
|Status: Device has "rtsShorted" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"rtsShorted","like")}=2` |HIGH | |
|Status: Device has "rtsDisconnected" faults flag |<p>-</p> |`{TEMPLATE_NAME:status.faults[faults.0].count(#3,"rtsDisconnected","like")}=2` |HIGH | |
|Status: Device has "rtsShorted" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"rtsShorted","like")}=2` |WARNING | |
|Status: Device has "rtsDisconnected" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"rtsDisconnected","like")}=2` |WARNING | |
|Status: Device has "heatsinkTempSensorOpen" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"heatsinkTempSensorOpen","like")}=2` |WARNING | |
|Status: Device has "heatsinkTempSensorShorted" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"heatsinkTempSensorShorted","like")}=2` |WARNING | |
|Status: Device has "tristarHot" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"tristarHot","like")}=2` |WARNING | |
|Status: Device has "currentLimit" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"currentLimit","like")}=2` |WARNING | |
|Status: Device has "currentOffset" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"currentOffset","like")}=2` |WARNING | |
|Status: Device has "batterySense" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"batterySense","like")}=2` |WARNING | |
|Status: Device has "batterySenseDisconnected" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"batterySenseDisconnected","like")}=2` |WARNING | |
|Status: Device has "uncalibrated" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"uncalibrated","like")}=2` |WARNING | |
|Status: Device has "rtsMiswire" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"rtsMiswire","like")}=2` |WARNING | |
|Status: Device has "highVoltageDisconnect" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"highVoltageDisconnect","like")}=2` |WARNING | |
|Status: Device has "diversionLoadNearMax" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"diversionLoadNearMax","like")}=2` |WARNING | |
|Status: Device has "systemMiswire" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"systemMiswire","like")}=2` |WARNING | |
|Status: Device has "mosfetSOpen" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"mosfetSOpen","like")}=2` |WARNING | |
|Status: Device has "p12VoltageReferenceOff" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"p12VoltageReferenceOff","like")}=2` |WARNING | |
|Status: Device has "loadDisconnectState" alarm flag |<p>-</p> |`{TEMPLATE_NAME:status.alarms[alarms.0].count(#3,"loadDisconnectState","like")}=2` |WARNING | |
|Temperature: Low battery temperature (below {$BATTERY.TEMP.MIN.WARN}C for 5m) |<p>-</p> |`{TEMPLATE_NAME:temp.battery[batteryTemperature.0].max(5m)}<{$BATTERY.TEMP.MIN.WARN}` |WARNING |<p>**Depends on**:</p><p>- Temperature: Critically low battery temperature (below {$BATTERY.TEMP.MIN.WARN}C for 5m)</p> |
|Temperature: Critically low battery temperature (below {$BATTERY.TEMP.MIN.WARN}C for 5m) |<p>-</p> |`{TEMPLATE_NAME:temp.battery[batteryTemperature.0].max(5m)}<{$BATTERY.TEMP.MIN.CRIT}` |HIGH | |
|Temperature: High battery temperature (over {$BATTERY.TEMP.MAX.WARN}C for 5m) |<p>-</p> |`{TEMPLATE_NAME:temp.battery[batteryTemperature.0].min(5m)}>{$BATTERY.TEMP.MAX.WARN}` |WARNING |<p>**Depends on**:</p><p>- Temperature: Critically high battery temperature (over {$BATTERY.TEMP.MAX.CRIT}C for 5m)</p> |
|Temperature: Critically high battery temperature (over {$BATTERY.TEMP.MAX.CRIT}C for 5m) |<p>-</p> |`{TEMPLATE_NAME:temp.battery[batteryTemperature.0].min(5m)}>{$BATTERY.TEMP.MAX.CRIT}` |HIGH | |

## Feedback

Please report any issues with the template at https://support.zabbix.com

