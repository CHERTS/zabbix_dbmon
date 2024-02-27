
# OpenBSD by Zabbix agent

## Overview

It is an Official OpenBSD template. It requires Zabbix agent 6.0 or newer.    


## Requirements

Zabbix version: 6.0 and higher.

## Tested versions

This template has been tested on:
- OpenBSD

## Configuration

> Zabbix should be configured according to the instructions in the [Templates out of the box](https://www.zabbix.com/documentation/6.0/manual/config/templates_out_of_the_box) section.

## Setup

Install Zabbix agent on OpenBSD according to Zabbix documentation.


### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$AGENT.TIMEOUT}|<p>The timeout after which the agent is considered unavailable. It works only for the agents reachable from Zabbix server/proxy (in passive mode).</p>|`3m`|

### Items

|Name|Description|Type|Key and additional info|
|----|-----------|----|-----------------------|
|OpenBSD: Maximum number of opened files|<p>It could be increased by using the sysctl utility or modifying the file /etc/sysctl.conf.</p>|Zabbix agent|kernel.maxfiles|
|OpenBSD: Maximum number of processes|<p>It could be increased by using the sysctl utility or modifying the file /etc/sysctl.conf.</p>|Zabbix agent|kernel.maxproc|
|OpenBSD: Number of running processes|<p>The number of processes in a running state.</p>|Zabbix agent|proc.num[,,run]|
|OpenBSD: Number of processes|<p>The total number of processes in any state.</p>|Zabbix agent|proc.num[]|
|OpenBSD: Host boot time||Zabbix agent|system.boottime|
|OpenBSD: Interrupts per second||Zabbix agent|system.cpu.intr<p>**Preprocessing**</p><ul><li>Change per second: </li></ul>|
|OpenBSD: Processor load (1 min average per core)|<p>The processor load is calculated as the system CPU load divided by the number of CPU cores.</p>|Zabbix agent|system.cpu.load[percpu,avg1]|
|OpenBSD: Processor load (5 min average per core)|<p>The processor load is calculated as the system CPU load divided by the number of CPU cores.</p>|Zabbix agent|system.cpu.load[percpu,avg5]|
|OpenBSD: Processor load (15 min average per core)|<p>The processor load is calculated as the system CPU load divided by the number of CPU cores.</p>|Zabbix agent|system.cpu.load[percpu,avg15]|
|OpenBSD: Context switches per second||Zabbix agent|system.cpu.switches<p>**Preprocessing**</p><ul><li>Change per second: </li></ul>|
|OpenBSD: CPU idle time|<p>The time the CPU has spent doing nothing.</p>|Zabbix agent|system.cpu.util[,idle]|
|OpenBSD: CPU interrupt time|<p>The amount of time the CPU has been servicing hardware interrupts.</p>|Zabbix agent|system.cpu.util[,interrupt]|
|OpenBSD: CPU nice time|<p>The time the CPU has spent running users' processes that have been niced.</p>|Zabbix agent|system.cpu.util[,nice]|
|OpenBSD: CPU system time|<p>The time the CPU has spent running the kernel and its processes.</p>|Zabbix agent|system.cpu.util[,system]|
|OpenBSD: CPU user time|<p>The time the CPU has spent running users' processes that are not niced.</p>|Zabbix agent|system.cpu.util[,user]|
|OpenBSD: Host name|<p>A host name of the system.</p>|Zabbix agent|system.hostname<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|OpenBSD: Host local time||Zabbix agent|system.localtime|
|OpenBSD: Free swap space||Zabbix agent|system.swap.size[,free]|
|OpenBSD: Free swap space in %||Zabbix agent|system.swap.size[,pfree]|
|OpenBSD: Total swap space||Zabbix agent|system.swap.size[,total]|
|OpenBSD: System information|<p>The information as normally returned by the 'uname -a'.</p>|Zabbix agent|system.uname<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|OpenBSD: System uptime||Zabbix agent|system.uptime|
|OpenBSD: Number of logged in users|<p>The number of users who are currently logged in.</p>|Zabbix agent|system.users.num|
|OpenBSD: Checksum of /etc/passwd||Zabbix agent|vfs.file.cksum[/etc/passwd,sha256]<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1h`</p></li></ul>|
|OpenBSD: Available memory|<p>The available memory is defined as free+cached+buffers memory.</p>|Zabbix agent|vm.memory.size[available]|
|OpenBSD: Total memory||Zabbix agent|vm.memory.size[total]|
|OpenBSD: Version of Zabbix agent running||Zabbix agent|agent.version<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|OpenBSD: Host name of Zabbix agent running||Zabbix agent|agent.hostname<p>**Preprocessing**</p><ul><li><p>Discard unchanged with heartbeat: `1d`</p></li></ul>|
|OpenBSD: Zabbix agent ping|<p>The agent always returns 1 for this item. It could be used in combination with nodata() for the availability check.</p>|Zabbix agent|agent.ping|
|OpenBSD: Zabbix agent availability|<p>Monitoring the availability status of the agent.</p>|Zabbix internal|zabbix[host,agent,available]|

### Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----------|--------|--------------------------------|
|OpenBSD: Configured max number of opened files is too low on {HOST.NAME}||`last(/OpenBSD by Zabbix agent/kernel.maxfiles)<1024`|Info||
|OpenBSD: Configured max number of processes is too low on {HOST.NAME}||`last(/OpenBSD by Zabbix agent/kernel.maxproc)<256`|Info||
|OpenBSD: Too many processes running on {HOST.NAME}||`avg(/OpenBSD by Zabbix agent/proc.num[,,run],5m)>30`|Warning||
|OpenBSD: Too many processes on {HOST.NAME}||`avg(/OpenBSD by Zabbix agent/proc.num[],5m)>300`|Warning||
|OpenBSD: Processor load is too high on {HOST.NAME}||`avg(/OpenBSD by Zabbix agent/system.cpu.load[percpu,avg1],5m)>5`|Warning||
|OpenBSD: Hostname was changed on {HOST.NAME}||`last(/OpenBSD by Zabbix agent/system.hostname,#1)<>last(/OpenBSD by Zabbix agent/system.hostname,#2)`|Info||
|OpenBSD: Lack of free swap space on {HOST.NAME}|<p>It probably means that the systems requires more physical memory.</p>|`last(/OpenBSD by Zabbix agent/system.swap.size[,pfree])<50`|Warning||
|OpenBSD: Host information was changed on {HOST.NAME}||`last(/OpenBSD by Zabbix agent/system.uname,#1)<>last(/OpenBSD by Zabbix agent/system.uname,#2)`|Info||
|OpenBSD: {HOST.NAME} has just been restarted||`change(/OpenBSD by Zabbix agent/system.uptime)<0`|Info||
|OpenBSD: /etc/passwd has been changed on {HOST.NAME}||`last(/OpenBSD by Zabbix agent/vfs.file.cksum[/etc/passwd,sha256],#1)<>last(/OpenBSD by Zabbix agent/vfs.file.cksum[/etc/passwd,sha256],#2)`|Warning||
|OpenBSD: Lack of available memory on server {HOST.NAME}||`last(/OpenBSD by Zabbix agent/vm.memory.size[available])<20M`|Average||
|OpenBSD: Zabbix agent is not available|<p>For passive checks only the availability of the agents and a host is used with {$AGENT.TIMEOUT} as the time threshold.</p>|`max(/OpenBSD by Zabbix agent/zabbix[host,agent,available],{$AGENT.TIMEOUT})=0`|Average|**Manual close**: Yes|

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
|Mounted filesystem discovery|<p>Discovery of different types of file systems as defined in the global regular expression "File systems for discovery".</p>|Zabbix agent|vfs.fs.discovery|

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
|{#FSNAME}: Free inodes is less than 20%||`last(/OpenBSD by Zabbix agent/vfs.fs.inode[{#FSNAME},pfree])<20`|Warning||
|{#FSNAME}: Free disk space is less than 20%||`last(/OpenBSD by Zabbix agent/vfs.fs.size[{#FSNAME},pfree])<20`|Warning||

## Feedback

Please report any issues with the template at [`https://support.zabbix.com`](https://support.zabbix.com)

You can also provide feedback, discuss the template, or ask for help at [`ZABBIX forums`](https://www.zabbix.com/forum/zabbix-suggestions-and-feedback)

