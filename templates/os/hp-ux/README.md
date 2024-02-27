
# HP-UX by Zabbix agent

## Overview

It is an official HP-UX template. It requires Zabbix agent 6.0 or newer.


## Requirements

Zabbix version: 6.0 and higher.

## Tested versions

This template has been tested on:
- HP-UX OS

## Configuration

> Zabbix should be configured according to the instructions in the [Templates out of the box](https://www.zabbix.com/documentation/6.0/manual/config/templates_out_of_the_box) section.

## Setup

Install Zabbix agent on the HP-UX OS according to Zabbix documentation.


### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$AGENT.TIMEOUT}|<p>The timeout after which the agent is considered unavailable. It works only for the agents reachable from Zabbix server/proxy (in passive mode).</p>|`3m`|

### Items

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|HP-UX: Processor load (1 min average per core)|<p>The processor load is calculated as the system CPU load divided by the number of CPU cores.</p>|Zabbix agent|system.cpu.load[percpu,avg1]|
|HP-UX: Processor load (5 min average per core)|<p>The processor load is calculated as the system CPU load divided by the number of CPU cores.</p>|Zabbix agent|system.cpu.load[percpu,avg5]|
|HP-UX: Processor load (15 min average per core)|<p>The processor load is calculated as the system CPU load divided by the number of CPU cores.</p>|Zabbix agent|system.cpu.load[percpu,avg15]|
|HP-UX: CPU idle time|<p>The time the CPU has spent doing nothing.</p>|Zabbix agent|system.cpu.util[,idle]|
|HP-UX: CPU nice time|<p>The time the CPU has spent running users' processes that have been niced.</p>|Zabbix agent|system.cpu.util[,nice]|
|HP-UX: CPU system time|<p>The time the CPU has spent running the kernel and its processes.</p>|Zabbix agent|system.cpu.util[,system]|
|HP-UX: CPU user time|<p>The time the CPU has spent running users' processes that are not niced.</p>|Zabbix agent|system.cpu.util[,user]|
|HP-UX: Host name|<p>A host name of the system.</p>|Zabbix agent|system.hostname<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|HP-UX: Host local time||Zabbix agent|system.localtime|
|HP-UX: System information|<p>The information as normally returned by the 'uname -a'.</p>|Zabbix agent|system.uname<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|HP-UX: Number of logged in users|<p>The number of users who are currently logged in.</p>|Zabbix agent|system.users.num|
|HP-UX: Checksum of /etc/passwd||Zabbix agent|vfs.file.cksum[/etc/passwd,sha256]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|HP-UX: Available memory|<p>The available memory is defined as free+cached+buffers memory.</p>|Zabbix agent|vm.memory.size[available]|
|HP-UX: Total memory||Zabbix agent|vm.memory.size[total]|
|HP-UX: Version of Zabbix agent running||Zabbix agent|agent.version<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|HP-UX: Host name of Zabbix agent running||Zabbix agent|agent.hostname<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|HP-UX: Zabbix agent ping|<p>The agent always returns 1 for this item. It could be used in combination with nodata() for the availability check.</p>|Zabbix agent|agent.ping|
|HP-UX: Zabbix agent availability|<p>Monitoring the availability status of the agent.</p>|Zabbix internal|zabbix[host,agent,available]|

### Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|HP-UX: Processor load is too high||`avg(/HP-UX by Zabbix agent/system.cpu.load[percpu,avg1],5m)>5`|Warning||
|HP-UX: Hostname was changed||`last(/HP-UX by Zabbix agent/system.hostname,#1)<>last(/HP-UX by Zabbix agent/system.hostname,#2)`|Info||
|HP-UX: Host information was changed||`last(/HP-UX by Zabbix agent/system.uname,#1)<>last(/HP-UX by Zabbix agent/system.uname,#2)`|Info||
|HP-UX: /etc/passwd has been changed||`last(/HP-UX by Zabbix agent/vfs.file.cksum[/etc/passwd,sha256],#1)<>last(/HP-UX by Zabbix agent/vfs.file.cksum[/etc/passwd,sha256],#2)`|Warning||
|HP-UX: Lack of available memory on server||`last(/HP-UX by Zabbix agent/vm.memory.size[available])<20M`|Average||
|HP-UX: Zabbix agent is not available|<p>For passive checks only the availability of the agents and a host is used with {$AGENT.TIMEOUT} as the time threshold.</p>|`max(/HP-UX by Zabbix agent/zabbix[host,agent,available],{$AGENT.TIMEOUT})=0`|Average|**Manual close**: Yes|

### LLD rule Network interface discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Network interface discovery|<p>The discovery of network interfaces as defined in the global regular expression "Network interfaces for discovery".</p>|Zabbix agent|net.if.discovery|

### Item prototypes for Network interface discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Interface {#IFNAME}: Incoming network traffic||Zabbix agent|net.if.in[{#IFNAME}]<p>**Preprocessing**</p><ul><li>Change per second: </li><li><p>Custom multiplier: `8`</p></li></ul>|
|Interface {#IFNAME}: Outgoing network traffic||Zabbix agent|net.if.out[{#IFNAME}]<p>**Preprocessing**</p><ul><li>Change per second: </li><li><p>Custom multiplier: `8`</p></li></ul>|

### LLD rule Mounted filesystem discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|Mounted filesystem discovery|<p>The discovery of different types of file systems as defined in the global regular expression "File systems for discovery".</p>|Zabbix agent|vfs.fs.discovery|

### Item prototypes for Mounted filesystem discovery

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|{#FSNAME}: Free inodes, %||Zabbix agent|vfs.fs.inode[{#FSNAME},pfree]|
|{#FSNAME}: Free disk space||Zabbix agent|vfs.fs.size[{#FSNAME},free]|
|{#FSNAME}: Free disk space, %||Zabbix agent|vfs.fs.size[{#FSNAME},pfree]|
|{#FSNAME}: Total disk space||Zabbix agent|vfs.fs.size[{#FSNAME},total]|
|{#FSNAME}: Used disk space||Zabbix agent|vfs.fs.size[{#FSNAME},used]|

### Trigger prototypes for Mounted filesystem discovery

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|{#FSNAME}: Free inodes is less than 20%||`last(/HP-UX by Zabbix agent/vfs.fs.inode[{#FSNAME},pfree])<20`|Warning||
|{#FSNAME}: Free disk space is less than 20%||`last(/HP-UX by Zabbix agent/vfs.fs.size[{#FSNAME},pfree])<20`|Warning||

## Feedback

Please report any issues with the template at [`https://support.zabbix.com`](https://support.zabbix.com)

You can also provide feedback, discuss the template, or ask for help at [`ZABBIX forums`](https://www.zabbix.com/forum/zabbix-suggestions-and-feedback)

