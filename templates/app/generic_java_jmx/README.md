
# Template App Generic Java JMX

## Overview

For Zabbix version: 5.0 and higher  
Official JMX Template from Zabbix distribution. Could be useful for many Java Applications (JMX).



## Setup

Refer to the vendor documentation.

## Zabbix configuration

No specific Zabbix configuration is required.

### Macros used

|Name|Description|Default|
|----|-----------|-------|
|{$JMX.CPU.LOAD.MAX} |<p>A threshold in percent for CPU utilization trigger.</p> |`85` |
|{$JMX.CPU.LOAD.TIME} |<p>The time during which the CPU utilization may exceed the threshold.</p> |`5m` |
|{$JMX.FILE.DESCRIPTORS.MAX} |<p>A threshold in percent for file descriptors count trigger.</p> |`85` |
|{$JMX.FILE.DESCRIPTORS.TIME} |<p>The time during which the file descriptors count may exceed the threshold.</p> |`3m` |
|{$JMX.HEAP.MEM.USAGE.MAX} |<p>A threshold in percent for Heap memory utilization trigger.</p> |`85` |
|{$JMX.HEAP.MEM.USAGE.TIME} |<p>The time during which the Heap memory utilization may exceed the threshold.</p> |`10m` |
|{$JMX.MEM.POOL.NAME.MATCHES} |<p>This macro used in memory pool discovery as a filter.</p> |`Old Gen|G1|Perm Gen|Code Cache|Tenured Gen` |
|{$JMX.MP.USAGE.MAX} |<p>A threshold in percent for memory pools utilization trigger. Use a context to change the threshold for a specific pool.</p> |`85` |
|{$JMX.MP.USAGE.TIME} |<p>The time during which the memory pools utilization may exceed the threshold.</p> |`10m` |
|{$JMX.NONHEAP.MEM.USAGE.MAX} |<p>A threshold in percent for Non-heap memory utilization trigger.</p> |`85` |
|{$JMX.NONHEAP.MEM.USAGE.TIME} |<p>The time during which the Non-heap memory utilization may exceed the threshold.</p> |`10m` |
|{$JMX.PASSWORD} |<p>JMX password.</p> |`` |
|{$JMX.USER} |<p>JMX username.</p> |`` |

## Template links

There are no template links in this template.

## Discovery rules

|Name|Description|Type|Key and additional info|
|----|-----------|----|----|
|Garbage collector discovery |<p>Garbage collectors metrics discovery.</p> |JMX |jmx.discovery["beans","java.lang:name=*,type=GarbageCollector"] |
|Memory pool discovery |<p>Memory pools metrics discovery.</p> |JMX |jmx.discovery["beans","java.lang:name=*,type=MemoryPool"]<p>**Filter**:</p> <p>- A: {#JMXNAME} MATCHES_REGEX `{$JMX.MEM.POOL.NAME.MATCHES}`</p> |

## Items collected

|Group|Name|Description|Type|Key and additional info|
|-----|----|-----------|----|---------------------|
|JMX |ClassLoading: Loaded class count |<p>Displays number of classes that are currently loaded in the Java virtual machine.</p> |JMX |jmx["java.lang:type=ClassLoading","LoadedClassCount"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|JMX |ClassLoading: Total loaded class count |<p>Displays the total number of classes that have been loaded since the Java virtual machine has started execution.</p> |JMX |jmx["java.lang:type=ClassLoading","TotalLoadedClassCount"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|JMX |ClassLoading: Unloaded class count |<p>Displays the total number of classes that have been loaded since the Java virtual machine has started execution.</p> |JMX |jmx["java.lang:type=ClassLoading","UnloadedClassCount"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|JMX |Compilation: Name of the current JIT compiler |<p>Displays the total number of classes unloaded since the Java virtual machine has started execution.</p> |JMX |jmx["java.lang:type=Compilation","Name"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `30m`</p> |
|JMX |Compilation: Accumulated time spent |<p>Displays the approximate accumulated elapsed time spent in compilation, in seconds.</p> |JMX |jmx["java.lang:type=Compilation","TotalCompilationTime"]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.001`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|JMX |Memory: Heap memory committed |<p>Current heap memory allocated. This amount of memory is guaranteed for the Java virtual machine to use.</p> |JMX |jmx["java.lang:type=Memory","HeapMemoryUsage.committed"] |
|JMX |Memory: Heap memory maximum size |<p>Maximum amount of heap that can be used for memory management. This amount of memory is not guaranteed to be available if it is greater than the amount of committed memory. The Java virtual machine may fail to allocate memory even if the amount of used memory does not exceed this maximum size.</p> |JMX |jmx["java.lang:type=Memory","HeapMemoryUsage.max"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|JMX |Memory: Heap memory used |<p>Current memory usage outside the heap.</p> |JMX |jmx["java.lang:type=Memory","HeapMemoryUsage.used"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|JMX |Memory: Non-Heap memory committed |<p>Current memory allocated outside the heap. This amount of memory is guaranteed for the Java virtual machine to use.</p> |JMX |jmx["java.lang:type=Memory","NonHeapMemoryUsage.committed"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|JMX |Memory: Non-Heap memory maximum size |<p>Maximum amount of non-heap memory that can be used for memory management. This amount of memory is not guaranteed to be available if it is greater than the amount of committed memory. The Java virtual machine may fail to allocate memory even if the amount of used memory does not exceed this maximum size.</p> |JMX |jmx["java.lang:type=Memory","NonHeapMemoryUsage.max"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|JMX |Memory: Non-Heap memory used |<p>Current memory usage outside the heap</p> |JMX |jmx["java.lang:type=Memory","NonHeapMemoryUsage.used"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|JMX |Memory: Object pending finalization count |<p>The approximate number of objects for which finalization is pending.</p> |JMX |jmx["java.lang:type=Memory","ObjectPendingFinalizationCount"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|JMX |OperatingSystem: File descriptors maximum count |<p>This is the number of file descriptors we can have opened in the same process, as determined by the operating system. You can never have more file descriptors than this number.</p> |JMX |jmx["java.lang:type=OperatingSystem","MaxFileDescriptorCount"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|JMX |OperatingSystem: File descriptors opened |<p>This is the number of opened file descriptors at the moment, if this reaches the MaxFileDescriptorCount, the application will throw an IOException: Too many open files. This could mean you are opening file descriptors and never closing them.</p> |JMX |jmx["java.lang:type=OperatingSystem","OpenFileDescriptorCount"] |
|JMX |OperatingSystem: Process CPU Load |<p>ProcessCpuLoad represents the CPU load in this process.</p> |JMX |jmx["java.lang:type=OperatingSystem","ProcessCpuLoad"]<p>**Preprocessing**:</p><p>- MULTIPLIER: `100`</p> |
|JMX |Runtime: JVM uptime |<p>-</p> |JMX |jmx["java.lang:type=Runtime","Uptime"]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.001`</p> |
|JMX |Runtime: JVM name |<p>-</p> |JMX |jmx["java.lang:type=Runtime","VmName"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `30m`</p> |
|JMX |Runtime: JVM version |<p>-</p> |JMX |jmx["java.lang:type=Runtime","VmVersion"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `30m`</p> |
|JMX |Threading: Daemon thread count |<p>Number of daemon threads running.</p> |JMX |jmx["java.lang:type=Threading","DaemonThreadCount"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|JMX |Threading: Peak thread count |<p>Maximum number of threads being executed at the same time since the JVM was started or the peak was reset.</p> |JMX |jmx["java.lang:type=Threading","PeakThreadCount"] |
|JMX |Threading: Thread count |<p>The number of threads running at the current moment.</p> |JMX |jmx["java.lang:type=Threading","ThreadCount"] |
|JMX |Threading: Total started thread count |<p>The number of threads started since the JVM was launched.</p> |JMX |jmx["java.lang:type=Threading","TotalStartedThreadCount"] |
|JMX |GarbageCollector {#JMXNAME}: number of collections per second |<p>Displays the total number of collections that have occurred per second.</p> |JMX |jmx["java.lang:name={#JMXNAME},type=GarbageCollector","CollectionCount"]<p>**Preprocessing**:</p><p>- CHANGE_PER_SECOND |
|JMX |GarbageCollector {#JMXNAME}: accumulated time spent in collection |<p>Displays the approximate accumulated collection elapsed time, in seconds.</p> |JMX |jmx["java.lang:name={#JMXNAME},type=GarbageCollector","CollectionTime"]<p>**Preprocessing**:</p><p>- MULTIPLIER: `0.001`</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|JMX |Memory pool {#JMXNAME}: committed |<p>Current memory allocated.</p> |JMX |jmx["java.lang:name={#JMXNAME},type=MemoryPool","Usage.committed"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|JMX |Memory pool {#JMXNAME}: maximum size |<p>Maximum amount of memory that can be used for memory management. This amount of memory is not guaranteed to be available if it is greater than the amount of committed memory. The Java virtual machine may fail to allocate memory even if the amount of used memory does not exceed this maximum size.</p> |JMX |jmx["java.lang:name={#JMXNAME},type=MemoryPool","Usage.max"]<p>**Preprocessing**:</p><p>- DISCARD_UNCHANGED_HEARTBEAT: `10m`</p> |
|JMX |Memory pool {#JMXNAME}: used |<p>Current memory usage.</p> |JMX |jmx["java.lang:name={#JMXNAME},type=MemoryPool","Usage.used"] |

## Triggers

|Name|Description|Expression|Severity|Dependencies and additional info|
|----|-----------|----|----|----|
|Compilation: {HOST.NAME} uses suboptimal JIT compiler |<p>-</p> |`{TEMPLATE_NAME:jmx["java.lang:type=Compilation","Name"].str(Client)}=1` |INFO |<p>Manual close: YES</p> |
|Memory: Heap memory usage more than {$JMX.HEAP.USAGE.MAX}% for {$JMX.HEAP.MEM.USAGE.TIME} |<p>-</p> |`{TEMPLATE_NAME:jmx["java.lang:type=Memory","HeapMemoryUsage.used"].min({$JMX.HEAP.MEM.USAGE.TIME})}>({TEMPLATE_NAME:jmx["java.lang:type=Memory","HeapMemoryUsage.max"].last()}*{$JMX.HEAP.MEM.USAGE.MAX}/100) and {TEMPLATE_NAME:jmx["java.lang:type=Memory","HeapMemoryUsage.max"].last()}>0` |WARNING | |
|Memory: Non-Heap memory usage more than {$JMX.NONHEAP.MEM.USAGE.MAX}% for {$JMX.NONHEAP.MEM.USAGE.TIME} |<p>-</p> |`{TEMPLATE_NAME:jmx["java.lang:type=Memory","NonHeapMemoryUsage.used"].min({$JMX.NONHEAP.MEM.USAGE.TIME})}>({TEMPLATE_NAME:jmx["java.lang:type=Memory","NonHeapMemoryUsage.max"].last()}*{$JMX.NONHEAP.MEM.USAGE.MAX}/100) and {TEMPLATE_NAME:jmx["java.lang:type=Memory","NonHeapMemoryUsage.max"].last()}>0` |WARNING | |
|OperatingSystem: Opened file descriptor count more than {$JMX.FILE.DESCRIPTORS.MAX}% of maximum |<p>-</p> |`{TEMPLATE_NAME:jmx["java.lang:type=OperatingSystem","OpenFileDescriptorCount"].min({$JMX.FILE.DESCRIPTORS.TIME})}>({TEMPLATE_NAME:jmx["java.lang:type=OperatingSystem","MaxFileDescriptorCount"].last()}*{$JMX.FILE.DESCRIPTORS.MAX}/100)` |WARNING | |
|OperatingSystem: Process CPU Load more than {$JMX.CPU.LOAD.MAX}% for {$JMX.CPU.LOAD.TIME} |<p>-</p> |`{TEMPLATE_NAME:jmx["java.lang:type=OperatingSystem","ProcessCpuLoad"].min({$JMX.CPU.LOAD.TIME})}>{$JMX.CPU.LOAD.MAX}` |AVERAGE | |
|Runtime: JVM is not reachable |<p>-</p> |`{TEMPLATE_NAME:jmx["java.lang:type=Runtime","Uptime"].nodata(5m)}=1` |AVERAGE |<p>Manual close: YES</p> |
|Runtime: {HOST.NAME} runs suboptimal VM type |<p>-</p> |`{TEMPLATE_NAME:jmx["java.lang:type=Runtime","VmName"].str(Server)}<>1` |INFO |<p>Manual close: YES</p> |
|Memory pool {#JMXNAME}: {#JMXNAME} memory usage more than {$JMX.MP.USAGE.MAX:"{#JMXNAME}"}% for {$JMX.MP.USAGE.TIME:"{#JMXNAME}"} |<p>-</p> |`{TEMPLATE_NAME:jmx["java.lang:name={#JMXNAME},type=MemoryPool","Usage.used"].min({$JMX.MP.USAGE.TIME:"{#JMXNAME}"})}>({TEMPLATE_NAME:jmx["java.lang:name={#JMXNAME},type=MemoryPool","Usage.max"].last()}*{$JMX.MP.USAGE.MAX:"{#JMXNAME}"}/100) and {TEMPLATE_NAME:jmx["java.lang:name={#JMXNAME},type=MemoryPool","Usage.max"].last()}>0` |WARNING | |

## Feedback

Please report any issues with the template at https://support.zabbix.com

