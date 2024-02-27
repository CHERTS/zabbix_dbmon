
# Hikvision camera by HTTP

## Overview

Sample device overview page: https://www.hikvision.com/en/products/IP-Products/Network-Cameras/


## Requirements

Zabbix version: 6.0 and higher.

## Tested versions

This template has been tested on:
- DS-I220 
- DS-I450 
- DS-2CD2620F-I 
- DS-2CD1631FWD-I 
- DS-2CD2020F-I 
- DS-2CD2042WD-I 
- DS-2CD2T43G0-I5 
- DS-2DF5286-AEL 
- DS-2CD2T25FWD-I5 
- DS-2CD4A35FWD-IZHS 
- DS-I200 
- DS-2CD1031-I 
- DS-2CD2125FWD-IS 
- DS-I122 
- DS-I203 
- DS-N201 
- DS-2CD2622FWD-IZS 
- DS-2CD2023G0-I 

## Configuration

> Zabbix should be configured according to the instructions in the [Templates out of the box](https://www.zabbix.com/documentation/6.0/manual/config/templates_out_of_the_box) section.

## Setup

Define macros according to your camera configuration


### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$CPU.UTIL.CRIT}||`90`|
|{$HIKVISION_ISAPI_PORT}|<p>ISAPI port on device</p>|`80`|
|{$HIKVISION_MAIN_CHANNEL_ID}|<p>Main video stream ID</p>|`101`|
|{$HIKVISION_STREAM_HEIGHT}|<p>Main video stream image height</p>|`1080`|
|{$HIKVISION_STREAM_WIDTH}|<p>Main video stream image width</p>|`1920`|
|{$MEMORY.UTIL.MAX}||`95`|
|{$PASSWORD}||`1234`|
|{$USER}||`admin`|

### Items

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Hikvision camera: Boot loader released date||Dependent item|hikvision_cam.boot_released_date<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.bootReleasedDate`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: Boot loader version||Dependent item|hikvision_cam.boot_version<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.bootVersion`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: CPU utilization|<p>CPU utilization in %</p>|Dependent item|hikvision_cam.cpu.util<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceStatus.CPUList.CPU.cpuUtilization`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Hikvision camera: Current device time||Dependent item|hikvision_cam.current_device_time<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceStatus.currentDeviceTime`</p><p>⛔️Custom on fail: Discard value</p></li></ul>|
|Hikvision camera: Device description||Dependent item|hikvision_cam.device_description<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.deviceDescription`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: Device ID||Dependent item|hikvision_cam.device_id<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.deviceID`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: Device location||Dependent item|hikvision_cam.device_location<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.deviceLocation`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: Device name||Dependent item|hikvision_cam.device_name<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.deviceName`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|Hikvision camera: Device type||Dependent item|hikvision_cam.device_type<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.deviceType`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: Encoder released date||Dependent item|hikvision_cam.encoder_released_date<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.encoderReleasedDate`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: Encoder version||Dependent item|hikvision_cam.encoder_version<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.encoderVersion`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: Firmware released date||Dependent item|hikvision_cam.firmware_released_date<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.firmwareReleasedDate`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: Firmware version||Dependent item|hikvision_cam.firmware_version<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.firmwareVersion`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: Get device info|<p>Used to get the device information</p>|HTTP agent|hikvision_cam.get_info<p>**Preprocessing**</p><ul><li>Check for not supported value: <p>⛔️Custom on fail: Set value to: `{"html":{"head":{"title":"Connection error"}}}`</p></li><li>XML to JSON: <p>⛔️Custom on fail: Set value to: `{"html":{"head":{"title":"Connection error"}}}`</p></li></ul>|
|Hikvision camera: Get device info: Login status||Dependent item|hikvision_cam.get_info.login_status<p>**Preprocessing**</p><ul><li><p>JavaScript: `The text is too long. Please see the template.`</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Hikvision camera: Get system status|<p>It is used to get the status information of the device</p>|HTTP agent|hikvision_cam.get_status<p>**Preprocessing**</p><ul><li>Check for not supported value: <p>⛔️Custom on fail: Set value to: `{"html":{"head":{"title":"Connection error"}}}`</p></li><li>XML to JSON: <p>⛔️Custom on fail: Set value to: `{"html":{"head":{"title":"Connection error"}}}`</p></li></ul>|
|Hikvision camera: Get system status: Login status||Dependent item|hikvision_cam.get_status.login_status<p>**Preprocessing**</p><ul><li><p>JavaScript: `The text is too long. Please see the template.`</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Hikvision camera: Get streaming channels|<p>Used to get the properties of streaming channels for the device</p>|HTTP agent|hikvision_cam.get_streaming<p>**Preprocessing**</p><ul><li>Check for not supported value: <p>⛔️Custom on fail: Set value to: `{"html":{"head":{"title":"Connection error"}}}`</p></li><li>XML to JSON: <p>⛔️Custom on fail: Set value to: `{"html":{"head":{"title":"Connection error"}}}`</p></li></ul>|
|Hikvision camera: Get streaming channels: Login status||Dependent item|hikvision_cam.get_streaming.login_status<p>**Preprocessing**</p><ul><li><p>JavaScript: `The text is too long. Please see the template.`</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Hikvision camera: Hardware version||Dependent item|hikvision_cam.hardware_version<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.hardwareVersion`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: MACaddress||Dependent item|hikvision_cam.mac_address<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.macAddress`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: Memory utilization|<p>Memory utilization in %</p>|Dependent item|hikvision_cam.memory.usage<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceStatus.MemoryList.Memory.memoryUsage`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Hikvision camera: Model||Dependent item|hikvision_cam.model<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.model`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: Serial number||Dependent item|hikvision_cam.serial_number<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.serialNumber`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: Supported beep||Dependent item|hikvision_cam.support_beep<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.supportBeep`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: Supported video loss||Dependent item|hikvision_cam.support_video_loss<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.supportVideoLoss`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: System contact||Dependent item|hikvision_cam.system_contact<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.systemContact`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: Telecontrol ID||Dependent item|hikvision_cam.telecontrol_id<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceInfo.telecontrolID`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `24h`</p></li></ul>|
|Hikvision camera: Uptime|<p>The system uptime expressed in the following format: "N days, hh:mm:ss".</p>|Dependent item|hikvision_cam.uptime<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.DeviceStatus.deviceUpTime`</p><p>⛔️Custom on fail: Discard value</p></li></ul>|

### Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Hikvision camera: High CPU utilization|<p>The CPU utilization is too high. The system might be slow to respond.</p>|`min(/Hikvision camera by HTTP/hikvision_cam.cpu.util,5m)>{$CPU.UTIL.CRIT}`|Warning||
|Hikvision camera: Version has changed|<p>Hikvision camera version has changed. Acknowledge to close the problem manually.</p>|`last(/Hikvision camera by HTTP/hikvision_cam.firmware_version,#1)<>last(/Hikvision camera by HTTP/hikvision_cam.firmware_version,#2) and length(last(/Hikvision camera by HTTP/hikvision_cam.firmware_version))>0`|Info|**Manual close**: Yes|
|Hikvision camera: Authorisation error|<p>Check the correctness of the authorization data</p>|`last(/Hikvision camera by HTTP/hikvision_cam.get_info.login_status)=1 or last(/Hikvision camera by HTTP/hikvision_cam.get_streaming.login_status)=1 or last(/Hikvision camera by HTTP/hikvision_cam.get_status.login_status)=1`|Warning|**Manual close**: Yes|
|Hikvision camera: Error receiving data|<p>Check the availability of the HTTP port</p>|`last(/Hikvision camera by HTTP/hikvision_cam.get_info.login_status)=2 or last(/Hikvision camera by HTTP/hikvision_cam.get_streaming.login_status)=2 or last(/Hikvision camera by HTTP/hikvision_cam.get_status.login_status)=2`|Warning|**Manual close**: Yes|
|Hikvision camera: High memory utilization|<p>The system is running out of free memory.</p>|`min(/Hikvision camera by HTTP/hikvision_cam.memory.usage,5m)>{$MEMORY.UTIL.MAX}`|Average||
|Hikvision camera: Camera has been replaced|<p>Camera serial number has changed. Acknowledge to close the problem manually.</p>|`last(/Hikvision camera by HTTP/hikvision_cam.serial_number,#1)<>last(/Hikvision camera by HTTP/hikvision_cam.serial_number,#2) and length(last(/Hikvision camera by HTTP/hikvision_cam.serial_number))>0`|Info|**Manual close**: Yes|
|Hikvision camera: has been restarted|<p>Uptime is less than 10 minutes.</p>|`last(/Hikvision camera by HTTP/hikvision_cam.uptime)<10m`|Info|**Manual close**: Yes|

### LLD rule PTZ discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|PTZ discovery||HTTP agent|hikvision_cam.ptz.discovery<p>**Preprocessing**</p><ul><li>XML to JSON: </li><li><p>JavaScript: `The text is too long. Please see the template.`</p></li></ul>|

### Item prototypes for PTZ discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Hikvision camera: Get PTZ info: Channel "{#PTZ_CHANNEL_ID}": Login status||Dependent item|hikvision_cam.get_ptz.login_status[{#PTZ_CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JavaScript: `The text is too long. Please see the template.`</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Hikvision camera: Get PTZ info|<p>High precision positioning which is accurate to a bit after the decimal point</p>|HTTP agent|hikvision_cam.get_ptz[{#PTZ_CHANNEL_ID}]<p>**Preprocessing**</p><ul><li>Check for not supported value: <p>⛔️Custom on fail: Set value to: `{"html":{"head":{"title":"Connection error"}}}`</p></li><li>XML to JSON: <p>⛔️Custom on fail: Set value to: `{"html":{"head":{"title":"Connection error"}}}`</p></li></ul>|
|Channel "{#PTZ_CHANNEL_ID}": Absolute zoom||Dependent item|hikvision_cam.ptz.absolute_zoom[{#PTZ_CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.PTZStatus.AbsoluteHigh.absoluteZoom`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Custom multiplier: `0.1`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Channel "{#PTZ_CHANNEL_ID}": Azimuth||Dependent item|hikvision_cam.ptz.azimuth[{#PTZ_CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.PTZStatus.AbsoluteHigh.azimuth`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Custom multiplier: `0.1`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Channel "{#PTZ_CHANNEL_ID}": Elevation||Dependent item|hikvision_cam.ptz.elevation[{#PTZ_CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `$.PTZStatus.AbsoluteHigh.elevation`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Custom multiplier: `0.1`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|

### Trigger prototypes for PTZ discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Hikvision camera: Authorisation error on get PTZ channels|<p>Check the correctness of the authorization data</p>|`last(/Hikvision camera by HTTP/hikvision_cam.get_ptz.login_status[{#PTZ_CHANNEL_ID}])=1`|Warning|**Manual close**: Yes<br>**Depends on**:<br><ul><li>Hikvision camera: Authorisation error</li></ul>|
|Hikvision camera: Error receiving data on PTZ channels|<p>Check the availability of the HTTP port</p>|`last(/Hikvision camera by HTTP/hikvision_cam.get_ptz.login_status[{#PTZ_CHANNEL_ID}])=2`|Warning|**Manual close**: Yes<br>**Depends on**:<br><ul><li>Hikvision camera: Error receiving data</li></ul>|
|Channel "{#PTZ_CHANNEL_ID}": PTZ position changed|<p>The direction of the camera has changed</p>|`last(/Hikvision camera by HTTP/hikvision_cam.ptz.absolute_zoom[{#PTZ_CHANNEL_ID}],#1)<>last(/Hikvision camera by HTTP/hikvision_cam.ptz.absolute_zoom[{#PTZ_CHANNEL_ID}],#2) or last(/Hikvision camera by HTTP/hikvision_cam.ptz.azimuth[{#PTZ_CHANNEL_ID}],#1)<>last(/Hikvision camera by HTTP/hikvision_cam.ptz.azimuth[{#PTZ_CHANNEL_ID}],#2) or last(/Hikvision camera by HTTP/hikvision_cam.ptz.elevation[{#PTZ_CHANNEL_ID}],#1)<>last(/Hikvision camera by HTTP/hikvision_cam.ptz.elevation[{#PTZ_CHANNEL_ID}],#2)`|Info|**Manual close**: Yes|

### LLD rule Streaming channels discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Streaming channels discovery||HTTP agent|hikvision_cam.streaming.discovery<p>**Preprocessing**</p><ul><li>XML to JSON: </li><li><p>JavaScript: `The text is too long. Please see the template.`</p></li></ul>|

### Item prototypes for Streaming channels discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Channel "{#CHANNEL_ID}": Constant bitRate||Dependent item|hikvision_cam.constant_bit_rate[{#CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `The text is too long. Please see the template.`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JSON Path: `$.[0]`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Channel "{#CHANNEL_ID}": Fixed quality||Dependent item|hikvision_cam.fixed_quality[{#CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `The text is too long. Please see the template.`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JSON Path: `$[0]`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Channel "{#CHANNEL_ID}": GovLength||Dependent item|hikvision_cam.gov_length[{#CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `The text is too long. Please see the template.`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JSON Path: `$[0]`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Channel "{#CHANNEL_ID}": H264Profile||Dependent item|hikvision_cam.h264Profile[{#CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `The text is too long. Please see the template.`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JSON Path: `$[0]`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Channel "{#CHANNEL_ID}": Key frame interval||Dependent item|hikvision_cam.key_frame_interval[{#CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `The text is too long. Please see the template.`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JSON Path: `$[0]`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Custom multiplier: `0.01`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Channel "{#CHANNEL_ID}": Frame rate (max)||Dependent item|hikvision_cam.max_frame_rate[{#CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `The text is too long. Please see the template.`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JSON Path: `$[0]`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Custom multiplier: `0.01`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Channel "{#CHANNEL_ID}": Smoothing||Dependent item|hikvision_cam.smoothing[{#CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `The text is too long. Please see the template.`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JSON Path: `$[0]`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Channel "{#CHANNEL_ID}": Snapshot image type||Dependent item|hikvision_cam.snap_shot_image_type[{#CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `The text is too long. Please see the template.`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JSON Path: `$[0]`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Channel "{#CHANNEL_ID}": VBR lower||Dependent item|hikvision_cam.vbr_lower_cap[{#CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `The text is too long. Please see the template.`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JSON Path: `$[0]`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Channel "{#CHANNEL_ID}": VBR upper||Dependent item|hikvision_cam.vbr_upper_cap[{#CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `The text is too long. Please see the template.`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JSON Path: `$[0]`</p><p>⛔️Custom on fail: Discard value</p></li></ul>|
|Channel "{#CHANNEL_ID}": Video codec type||Dependent item|hikvision_cam.video_codec_type[{#CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `The text is too long. Please see the template.`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JSON Path: `$[0]`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Channel "{#CHANNEL_ID}": Video quality control type||Dependent item|hikvision_cam.video_quality_control_type[{#CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `The text is too long. Please see the template.`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JSON Path: `$[0]`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Channel "{#CHANNEL_ID}": Resolution height||Dependent item|hikvision_cam.video_resolution_height[{#CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `The text is too long. Please see the template.`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JSON Path: `$[0]`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Channel "{#CHANNEL_ID}": Resolution width||Dependent item|hikvision_cam.video_resolution_width[{#CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `The text is too long. Please see the template.`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JSON Path: `$[0]`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|Channel "{#CHANNEL_ID}": Video scan type||Dependent item|hikvision_cam.video_scan_type[{#CHANNEL_ID}]<p>**Preprocessing**</p><ul><li><p>JSON Path: `The text is too long. Please see the template.`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>JSON Path: `$[0]`</p><p>⛔️Custom on fail: Discard value</p></li><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|

### Trigger prototypes for Streaming channels discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|Channel "{#CHANNEL_ID}": Invalid video stream resolution parameters|<p>expected: {$HIKVISION_STREAM_WIDTH} px x {$HIKVISION_STREAM_HEIGHT} px<br>received: {ITEM.LASTVALUE2} x {ITEM.LASTVALUE1}</p>|`last(/Hikvision camera by HTTP/hikvision_cam.video_resolution_height[{#CHANNEL_ID}])<>{$HIKVISION_STREAM_HEIGHT} or last(/Hikvision camera by HTTP/hikvision_cam.video_resolution_width[{#CHANNEL_ID}])<>{$HIKVISION_STREAM_WIDTH}`|Warning|**Manual close**: Yes|
|Channel "{#CHANNEL_ID}": Parameters of video stream are changed||`last(/Hikvision camera by HTTP/hikvision_cam.fixed_quality[{#CHANNEL_ID}],#1)<>last(/Hikvision camera by HTTP/hikvision_cam.fixed_quality[{#CHANNEL_ID}],#2) or last(/Hikvision camera by HTTP/hikvision_cam.constant_bit_rate[{#CHANNEL_ID}],#1)<>last(/Hikvision camera by HTTP/hikvision_cam.constant_bit_rate[{#CHANNEL_ID}],#2) or last(/Hikvision camera by HTTP/hikvision_cam.video_quality_control_type[{#CHANNEL_ID}],#1)<>last(/Hikvision camera by HTTP/hikvision_cam.video_quality_control_type[{#CHANNEL_ID}],#2) or last(/Hikvision camera by HTTP/hikvision_cam.video_resolution_width[{#CHANNEL_ID}],#1)<>last(/Hikvision camera by HTTP/hikvision_cam.video_resolution_width[{#CHANNEL_ID}],#2) or last(/Hikvision camera by HTTP/hikvision_cam.video_resolution_height[{#CHANNEL_ID}],#1)<>last(/Hikvision camera by HTTP/hikvision_cam.video_resolution_height[{#CHANNEL_ID}],#2)`|Info|**Manual close**: Yes|

## Feedback

Please report any issues with the template at [`https://support.zabbix.com`](https://support.zabbix.com)

You can also provide feedback, discuss the template, or ask for help at [`ZABBIX forums`](https://www.zabbix.com/forum/zabbix-suggestions-and-feedback)

