#!/usr/bin/python

#
# Program: Consolidate backup for Oracle (Zabbix DBMON) <dbmon_consolidate_backup.py>
#
# Author: Mikhail Grigorev <sleuthhound at gmail dot com>
# 
# Current Version: 1.0.0
#
# License:
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# Required: pip install py-zabbix
#

from pyzabbix import ZabbixAPI, ZabbixAPIException
import os
import argparse
import ConfigParser
import time
import sys
import itertools
import collections
import re
from datetime import datetime, timedelta

zbx_api = ""
zbx_api_url = ""
zbx_api_user = ""
zbx_api_password = ""
zbx_template_name = "DBS_Template Oracle for "
enable_verbose_mode = 0
enable_trigger_change = 1


def print_date(print_str, show_date=True):
    if print_date:
        print "%s: %s" % (datetime.now().strftime("%Y-%m-%d %H:%M:%S"), print_str)
    else:
        print "%s" % print_str


def zbx_update_trigger(trigger_id, trigger_status):
    if trigger_status == 0:
        trigger_status_desc = 'Enabled'
    else:
        trigger_status_desc = 'Disabled'
    if enable_trigger_change:
        try:
            trigger_update = zbx_api.trigger.update(triggerid=trigger_id, status=trigger_status)
        except ZabbixAPIException as e:
            print_date('Problem reading data via Zabbix API, error: {0}'.format(e))
        else:
            if trigger_update['triggerids'][0] == trigger_id:
                print_date('Trigger (ID: {0}, Status: {1} ({2})) updated!'.format(trigger_id, trigger_status_desc,
                                                                                  trigger_status))
            else:
                print_date('Trigger (ID: {0}, Status: {1} ({2})) NOT updated!'.format(trigger_id, trigger_status_desc,
                                                                                      trigger_status))
    else:
        print_date('Trigger (ID: {0}, Status: {1} ({2})) FAKE updated!'.format(trigger_id, trigger_status_desc,
                                                                               trigger_status))


def zbx_get_trigger_info(trigger_id):
    try:
        triggers = zbx_api.do_request('trigger.get',
                                      {'triggerids': [trigger_id], 'output': ['triggerids', 'description']})
    except ZabbixAPIException as e:
        print_date('Problem reading data via Zabbix API, error: {0}'.format(e))
    else:
        trigger_ids = [item['triggerid'] for item in triggers['result']]
        trigger_names = [item['description'] for item in triggers['result']]
        for trigger_id, trigger_name in itertools.izip(trigger_ids, trigger_names):
            print_date('TriggerID: {0} | TriggerName: {1}'.format(trigger_id, trigger_name))


def get_item_triggerid(host_id, item_id):
    try:
        triggers = zbx_api.do_request('item.get', {'hostids': [host_id], 'output': ['itemid', 'name', 'triggers'],
                                                   'with_triggers': True, 'selectTriggers': 'triggers',
                                                   'filter': {'itemid': item_id}})
        return triggers['result'][0]['triggers']
    except ZabbixAPIException as e:
        print_date('Problem reading data via Zabbix API, error: {0}'.format(e))
    else:
        return triggers['result'][0]['triggers']


def zbx_get_backup_info(host_id, host_item_key):
    instance_backup_info = []
    trigger_info = []
    try:
        if enable_verbose_mode:
            print_date('Get backup item: HostID: {0} | ItemName: {1}'.format(host_id, host_item_key))
        items = zbx_api.do_request('item.get', {'hostids': [host_id], 'output': ['itemid', 'name', 'lastvalue'],
                                                'search': {'key_': host_item_key}})
    except ZabbixAPIException as e:
        print_date('Problem reading data via Zabbix API, error: {0}'.format(e))
        sys.exit(0)
    else:
        #print '\n'.join(str(p) for p in items['result'])
        if (items['result'] != 0) and (len(items['result']) != 0):
            backup_ids = [item['itemid'] for item in items['result']]
            backup_names = [item['name'] for item in items['result']]
            backup_lastvalues = [item['lastvalue'] for item in items['result']]
            for backup_id, backup_name, backup_lastvalue in itertools.izip(backup_ids, backup_names, backup_lastvalues):
                if enable_verbose_mode:
                    print_date('Backup info: ItemID: {0} | ItemName: {1} | ItemLastValue: {2}'.format(backup_id, backup_name, backup_lastvalue))
                trigger_info = get_item_triggerid(host_id, backup_id)
                instance_backup_info.append(
                    {"hostid": host_id, "itemid": backup_id, "itemname": backup_name, "item_lastvalue": backup_lastvalue,
                     "triggers": trigger_info})
        return instance_backup_info


def zbx_get_dbid(host_id, host_instance, host_dbname):
    dbid_str = 'oracle.db[' + str(host_instance) + ',' + host_dbname + ',dbid]'
    try:
        items = zbx_api.do_request('item.get', {'hostids': [host_id], 'output': ['itemid', 'name', 'lastvalue'],
                                                'search': {'key_': dbid_str}})
    except ZabbixAPIException as e:
        print_date('Problem reading data via Zabbix API, error: {0}'.format(e))
        sys.exit(0)
    else:
        try:
            dbid_dbid = items['result'][0]['lastvalue']
        except (IndexError, ValueError) as e:
            print_date(
                'Function zbx_get_dbid: Error get DBID from instance \'{0}\', error: {1}'.format(host_instance, e))
            dbid_dbid = 0
        return dbid_dbid


def zbx_get_all_host_connected_to_template(template_id, template_name):
    host_info = []
    print_date('Get all host connected to template "{0}" (TemplateID: {1})'.format(template_name, template_id))
    try:
        hosts = zbx_api.host.get(templateids=template_id, sortfield=['hostid'], filter={'status': 0},
                                 output=['hostid', 'name'])
    except ZabbixAPIException as e:
        print_date('Problem reading data via Zabbix API, error: {0}'.format(e))
        sys.exit(0)
    else:
        host_ids = [host['hostid'] for host in hosts]
        host_names = [host['name'] for host in hosts]
        for host_id, host_name in itertools.izip(host_ids, host_names):
            if enable_verbose_mode:
                print_date('HostID: {0} | HostName: {1}'.format(host_id, host_name))
            macros = zbx_api.do_request('usermacro.get', {'hostids': [host_id], 'output': ['macro', 'value'],
                                                          'search': {'macro': '{$DBS_ORACLE_ENABLE_CONSOLIDATE_BACKUP:'}})
            if (macros['result'] != 0) and (len(macros['result']) != 0):
                macros_names = [item['macro'] for item in macros['result']]
                macros_values = [item['value'] for item in macros['result']]
                for macros_name, macros_value in itertools.izip(macros_names, macros_values):
                    if (macros_value == '1'):
                        result = re.findall(
                            r'^{\$DBS_ORACLE_ENABLE_CONSOLIDATE_BACKUP:["]{0,1}([A-Za-z0-9-_]+):([A-Za-z0-9-_]+)["]{0,1}}$',
                            macros_name)
                        instance = str(result[0][0])
                        dbname = str(result[0][1])
                        if enable_verbose_mode:
                            print_date('GetHostMacros = Instance: {0} | DBName: {1}'.format(instance, dbname))
                        dbid = zbx_get_dbid(host_id, instance, dbname)
                        if enable_verbose_mode:
                            print_date(
                                'GetDBID = Instance: {0} | DBName: {1} | DBID: {2}'.format(instance, dbname, dbid))
                        host_info.append({"hostid": host_id, "host_name": host_name, "instance": instance, "dbname": dbname,
                                                "dbid": dbid})
        return host_info


def calculate_min_last_backup(last_backup_info):
    last_backup_item_values = []
    for data in last_backup_info:
        last_backup_item_value = [backup_info['item_lastvalue'] for backup_info in data][0]
        if enable_verbose_mode:
            print_date('HostID: {0} | ItemID: {1} | ItemName: {2} | ItemValue: {3} ({4})'.format(
                [backup_info['hostid'] for backup_info in data][0], [backup_info['itemid'] for backup_info in data][0],
                [backup_info['itemname'] for backup_info in data][0], last_backup_item_value,
                str(timedelta(seconds=int(last_backup_item_value)))))
        last_backup_item_values.append(last_backup_item_value)
    last_backup_item_values = map(int, last_backup_item_values)
    last_backup_item_min_value = min(last_backup_item_values)
    if enable_verbose_mode:
        print_date('BackupValuesList: {0} | Min: {1}'.format(last_backup_item_values, last_backup_item_min_value))
    for data in last_backup_info:
        last_backup_item_value = [backup_info['item_lastvalue'] for backup_info in data][0]
        last_backup_item_value = int(last_backup_item_value)
        if enable_verbose_mode:
            print_date('HostID: {0} | ItemID: {1} | ItemName: {2} | ItemValue: {3} ({4})'.format(
                [backup_info['hostid'] for backup_info in data][0], [backup_info['itemid'] for backup_info in data][0],
                [backup_info['itemname'] for backup_info in data][0], last_backup_item_value,
                str(timedelta(seconds=int(last_backup_item_value)))))
        triggers_list = [backup_info['triggers'] for backup_info in data]
        if last_backup_item_value == last_backup_item_min_value:
            if enable_verbose_mode:
                print_date('Run trigger activation procedure')
            for triggerids in triggers_list:
                for triggerid in [trigger_info['triggerid'] for trigger_info in triggerids]:
                    if enable_verbose_mode:
                        zbx_get_trigger_info(triggerid)
                    zbx_update_trigger(triggerid, 0)
        else:
            if enable_verbose_mode:
                print_date('Run trigger deactivation procedure')
            for triggerids in triggers_list:
                for triggerid in [trigger_info['triggerid'] for trigger_info in triggerids]:
                    if enable_verbose_mode:
                        zbx_get_trigger_info(triggerid)
                    zbx_update_trigger(triggerid, 1)
        if enable_verbose_mode:
            print_date('End trigger activation/deactivation procedure')


def get_host_backup_info(host_group_infos):
    full_backup_info = []
    incr_backup_info = []
    arch_backup_info = []
    cf_backup_info = []
    instance_full_backup_info = []
    instance_incr_backup_info = []
    instance_arch_backup_info = []
    instance_cf_backup_info = []
    host_ids = [host_info['hostid'] for host_info in host_group_infos]
    host_names = [host_info['host_name'] for host_info in host_group_infos]
    host_instances = [host_info['instance'] for host_info in host_group_infos]
    host_dbnames = [host_info['dbname'] for host_info in host_group_infos]
    host_dbids = [host_info['dbid'] for host_info in host_group_infos]
    for host_id, host_name, host_instance, host_dbname, host_dbid in itertools.izip(host_ids, host_names,
                                                                                    host_instances, host_dbnames,
                                                                                    host_dbids):
        print_date(
            'HostID: {0} | Host: {1} | Instance: {2} | DB: {3} | DBID: {4}'.format(host_id, host_name, host_instance,
                                                                                   host_dbname, host_dbid))
        instance_full_backup_info = zbx_get_backup_info(host_id, 'oracle.backup.full[')
        if (instance_full_backup_info != 0) and (len(instance_full_backup_info) != 0):
            full_backup_info.append(instance_full_backup_info)
        instance_incr_backup_info = zbx_get_backup_info(host_id, 'oracle.backup.incr[')
        if (instance_incr_backup_info != 0) and (len(instance_incr_backup_info) != 0):
            incr_backup_info.append(instance_incr_backup_info)
        instance_arch_backup_info = zbx_get_backup_info(host_id, 'oracle.backup.archivelog[')
        if (instance_arch_backup_info != 0) and (len(instance_arch_backup_info) != 0):
            arch_backup_info.append(instance_arch_backup_info)
        instance_cf_backup_info = zbx_get_backup_info(host_id, 'oracle.backup.cf[')
        if (instance_cf_backup_info != 0) and (len(instance_cf_backup_info) != 0):
            cf_backup_info.append(instance_cf_backup_info)
    if (full_backup_info != 0) and (len(full_backup_info) != 0):
        calculate_min_last_backup(full_backup_info)
    if (incr_backup_info != 0) and (len(incr_backup_info) != 0):
        calculate_min_last_backup(incr_backup_info)
    if (arch_backup_info != 0) and (len(arch_backup_info) != 0):
        calculate_min_last_backup(arch_backup_info)
    if (cf_backup_info != 0) and (len(cf_backup_info) != 0):
        calculate_min_last_backup(cf_backup_info)


def get_host_info(host_infos, dbid):
    host_group_info = []
    host_ids = [host_info['hostid'] for host_info in host_infos]
    host_names = [host_info['host_name'] for host_info in host_infos]
    host_instances = [host_info['instance'] for host_info in host_infos]
    host_dbnames = [host_info['dbname'] for host_info in host_infos]
    host_dbids = [host_info['dbid'] for host_info in host_infos]
    for host_id, host_name, host_instance, host_dbname, host_dbid in itertools.izip(host_ids, host_names,
                                                                                    host_instances, host_dbnames,
                                                                                    host_dbids):
        if host_dbid == dbid:
            host_group_info.append(
                {"hostid": host_id, "host_name": host_name, "instance": host_instance, "dbname": host_dbname,
                 "dbid": host_dbid})
    return host_group_info


def read_config(args):
    global enable_verbose_mode
    global enable_trigger_change
    # Read config file
    config_file = str(args.config)
    if not os.path.exists(config_file):
        print_date('Error: Config file "{0}" not found.'.format(config_file))
        sys.exit(0)
    here = os.path.realpath('.')
    config = ConfigParser.ConfigParser(defaults={'here': here})
    config.read(config_file)
    global zbx_api_url, zbx_api_user, zbx_api_password
    zbx_api_url = config.get('zabbix', 'zbx_api_url')
    zbx_api_user = config.get('zabbix', 'zbx_api_user')
    zbx_api_password = config.get('zabbix', 'zbx_api_password')
    if args.notriggerchange:
        enable_trigger_change = 0
    else:
        enable_trigger_change = 1
    if args.verbose:
        enable_verbose_mode = 1
        print_date('Config: zbx_api_url={0}'.format(zbx_api_url))
        print_date('Config: zbx_api_user={0}'.format(zbx_api_user))
        print_date('Config: zbx_api_password={0}'.format('**********'))
        print_date('Config: enable_trigger_change={0}'.format(str(enable_trigger_change)))


def parse_args():
    # Parse argument
    parser = argparse.ArgumentParser(description='DBS consolidate backup control utility')
    parser.add_argument("-c", "--config", type=str, help='configuration file',
                        default='dbmon_consolidate_backup.conf')
    parser.add_argument("-v", "--verbose", action="store_true", help='increase output verbosity', default=False)
    parser.add_argument("-n", "--no-trigger-change", dest="notriggerchange", action="store_true",
                        help='no trigger state change', default=False)
    parser.set_defaults(func=read_config)
    return parser.parse_args()


def main():
    args = parse_args()
    args.func(args)
    # Create ZabbixAPI class instance
    global zbx_api, zbx_api_url, zbx_api_user, zbx_api_password
    try:
        #zbx_api = ZabbixAPI(zbx_api_url)
        #zbx_api.login(zbx_api_user, zbx_api_password)
        zbx_api = ZabbixAPI(url=zbx_api_url, user=zbx_api_user, password=zbx_api_password)
    except ZabbixAPIException as e:
        print_date('Problem reading data via Zabbix API, error: {0}'.format(e))
        sys.exit(0)
    else:
        if args.verbose:
            answer = zbx_api.do_request('apiinfo.version')
            print_date('zbx_api Version: {0}'.format(answer['result']))
        try:
            templates = zbx_api.do_request('template.get',
                                           {'output': ['templateid', 'name'], 'search': {'host': zbx_template_name}})
        except ZabbixAPIException as e:
            print_date('Problem reading data via Zabbix API, error: {0}'.format(e))
            sys.exit(0)
        else:
            if (templates['result'] != 0) and (len(templates['result']) != 0):
                template_ids = [template['templateid'] for template in templates['result']]
                template_names = [template['name'] for template in templates['result']]
                host_infos = []
                for template_id, template_name in itertools.izip(template_ids, template_names):
                    if args.verbose:
                        print_date('TemplateID: {0} | TemplateName: {1}'.format(template_id, template_name))
                    host_infos = zbx_get_all_host_connected_to_template(template_id, template_name)
                    host_dbids = [host_info['dbid'] for host_info in host_infos]
                    dbid_dupes = [item for item, count in collections.Counter(host_dbids).items() if count > 1]
                    dbid_dupes_list = set(dbid_dupes)
                    host_group_info = []
                    for dbid in dbid_dupes_list:
                        if dbid in host_dbids:
                            print_date('Start DBID: {0}'.format(dbid))
                            host_group_info = get_host_info(host_infos, dbid)
                            get_host_backup_info(host_group_info)
                            print_date('End DBID: {0}'.format(dbid))
            else:
                print_date('Warning: Zabbix template "{0}" not found.'.format(zbx_template_name))
                sys.exit(0)


if __name__ == '__main__':
    try:
        main()
    except (SystemExit, KeyboardInterrupt):
        sys.exit(0)
    except Exception as e:
        sys.exit(e)
