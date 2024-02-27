<?php declare(strict_types = 0);
/*
** Zabbix
** Copyright (C) 2001-2024 Zabbix SIA
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/


/**
 * Controller for host update.
 */
class CControllerHostUpdate extends CControllerHostUpdateGeneral {

	private $host;

	protected function checkInput(): bool {
		$ret = $this->validateInput(['hostid' => 'required|db hosts.hostid'] + self::getValidationFields());

		if (!$ret) {
			$this->setResponse(
				new CControllerResponseData(['main_block' => json_encode([
					'error' => [
						'title' => _('Cannot update host'),
						'messages' => array_column(get_and_clear_messages(), 'message')
					]
				])])
			);
		}

		return $ret;
	}

	protected function checkPermissions(): bool {
		if (!$this->checkAccess(CRoleHelper::UI_CONFIGURATION_HOSTS)) {
			return false;
		}

		$this->host = API::Host()->get([
			'output' => ['hostid', 'host', 'name', 'status', 'description', 'proxy_hostid', 'ipmi_authtype',
				'ipmi_privilege', 'ipmi_username', 'ipmi_password', 'tls_connect', 'tls_accept', 'tls_issuer',
				'tls_subject', 'flags', 'inventory_mode'
			],
			'hostids' => $this->getInput('hostid'),
			'editable' => true
		]);

		if (!$this->host) {
			return false;
		}

		$this->host = $this->host[0];

		return true;
	}

	protected function doAction(): void {
		$result = false;

		try {
			DBstart();

			$inventory_enabled = false;
			if ($this->getInput('inventory_mode', $this->host['inventory_mode']) != HOST_INVENTORY_DISABLED) {
				$inventory_enabled = true;
			}

			$clear_templates = array_diff(
				$this->getInput('clear_templates', []),
				$this->getInput('add_templates', [])
			);

			$host = [
				'hostid' => $this->host['hostid'],
				'host' => $this->getInput('host', $this->host['host']),
				'name' => $this->getInput('visiblename', $this->host['name']),
				'status' => $this->getInput('status', $this->host['status']),
				'proxy_hostid' => $this->getInput('proxy_hostid', $this->host['proxy_hostid']),
				'groups' => $this->processHostGroups($this->getInput('groups', [])),
				'interfaces' => $this->processHostInterfaces($this->getInput('interfaces', [])),
				'tags' => $this->processTags($this->getInput('tags', [])),
				'templates' => $this->processTemplates([
					$this->getInput('add_templates', []), $this->getInput('templates', [])
				]),
				'templates_clear' => zbx_toObject($clear_templates, 'templateid'),
				'macros' => $this->processUserMacros($this->getInput('macros', [])),
				'inventory' => $inventory_enabled ? $this->getInput('host_inventory', []) : [],
				'tls_connect' => $this->getInput('tls_connect', $this->host['tls_connect']),
				'tls_accept' => $this->getInput('tls_accept', $this->host['tls_accept'])
			];

			$host_properties = [
				'description', 'ipmi_authtype', 'ipmi_privilege', 'ipmi_username', 'ipmi_password', 'tls_subject',
				'tls_issuer', 'inventory_mode'
			];

			foreach ($host_properties as $prop) {
				if (!array_key_exists($prop, $this->host) || $this->getInput($prop, '') !== $this->host[$prop]) {
					$host[$prop] = $this->getInput($prop, '');
				}
			}

			$this->getInputs($host, ['tls_psk_identity', 'tls_psk']);

			if ($host['tls_connect'] != HOST_ENCRYPTION_PSK && !($host['tls_accept'] & HOST_ENCRYPTION_PSK)) {
				unset($host['tls_psk'], $host['tls_psk_identity']);
			}

			if ($host['tls_connect'] != HOST_ENCRYPTION_CERTIFICATE
					&& !($host['tls_accept'] & HOST_ENCRYPTION_CERTIFICATE)) {
				unset($host['tls_issuer'], $host['tls_subject']);
			}

			if ($this->host['flags'] == ZBX_FLAG_DISCOVERY_CREATED) {
				$host = array_intersect_key($host, array_flip(['hostid', 'status', 'inventory', 'description']));
			}

			$hostids = API::Host()->update($host);

			if ($hostids === false || !$this->processValueMaps($this->getInput('valuemaps', []))) {
				throw new Exception();
			}

			$result = DBend(true);
		}
		catch (Exception $e) {
			DBend(false);
		}

		$output = [];

		if ($result) {
			$success = ['title' => _('Host updated')];

			if ($messages = get_and_clear_messages()) {
				$success['messages'] = array_column($messages, 'message');
			}

			$output['success'] = $success;
		}
		else {
			$output['error'] = [
				'title' => _('Cannot update host'),
				'messages' => array_column(get_and_clear_messages(), 'message')
			];
		}

		$this->setResponse(new CControllerResponseData(['main_block' => json_encode($output)]));
	}

	/**
	 * Save valuemaps.
	 *
	 * @param array $valuemaps Submitted valuemaps.
	 *
	 * @return bool Whether mappings saved/deleted.
	 */
	private function processValueMaps(array $valuemaps): bool {
		$ins_valuemaps = [];
		$upd_valuemaps = [];

		$del_valuemapids = API::ValueMap()->get([
			'output' => [],
			'hostids' => $this->host['hostid'],
			'preservekeys' => true
		]);

		foreach ($valuemaps as $valuemap) {
			if (array_key_exists('valuemapid', $valuemap)) {
				$upd_valuemaps[] = $valuemap;
				unset($del_valuemapids[$valuemap['valuemapid']]);
			}
			else {
				$ins_valuemaps[] = $valuemap + ['hostid' => $this->host['hostid']];
			}
		}

		if ($upd_valuemaps && !API::ValueMap()->update($upd_valuemaps)) {
			return false;
		}

		if ($ins_valuemaps && !API::ValueMap()->create($ins_valuemaps)) {
			return false;
		}

		if ($del_valuemapids && !API::ValueMap()->delete(array_keys($del_valuemapids))) {
			return false;
		}

		return true;
	}
}
