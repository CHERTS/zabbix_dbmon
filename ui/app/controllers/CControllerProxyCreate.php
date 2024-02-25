<?php
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


class CControllerProxyCreate extends CController {

	/**
	 * @var array
	 */
	private $clone_proxy;

	protected function checkInput() {
		$fields = [
			'host' =>				'db hosts.host',
			'status' =>				'db hosts.status|in '.HOST_STATUS_PROXY_ACTIVE.','.HOST_STATUS_PROXY_PASSIVE,
			'dns' =>				'db interface.dns',
			'ip' =>					'db interface.ip',
			'useip' =>				'db interface.useip|in 0,1',
			'port' =>				'db interface.port',
			'proxy_address' =>		'db hosts.proxy_address',
			'description' =>		'db hosts.description',
			'tls_connect' =>		'db hosts.tls_connect|in '.HOST_ENCRYPTION_NONE.','.HOST_ENCRYPTION_PSK.','.
				HOST_ENCRYPTION_CERTIFICATE,
			'tls_accept' =>			'db hosts.tls_accept|in 0,'.HOST_ENCRYPTION_NONE.','.HOST_ENCRYPTION_PSK.','.
				(HOST_ENCRYPTION_NONE | HOST_ENCRYPTION_PSK).','.
				HOST_ENCRYPTION_CERTIFICATE.','.
				(HOST_ENCRYPTION_NONE | HOST_ENCRYPTION_CERTIFICATE).','.
				(HOST_ENCRYPTION_PSK | HOST_ENCRYPTION_CERTIFICATE).','.
				(HOST_ENCRYPTION_NONE | HOST_ENCRYPTION_PSK | HOST_ENCRYPTION_CERTIFICATE),
			'tls_psk' =>			'db hosts.tls_psk',
			'tls_psk_identity' =>	'db hosts.tls_psk_identity',
			'psk_edit_mode' =>		'in 0,1',
			'tls_issuer' =>			'db hosts.tls_issuer',
			'tls_subject' =>		'db hosts.tls_subject',
			'clone_proxyid' =>		'db hosts.hostid',
			'form_refresh' =>		'int32'
		];

		$ret = $this->validateInput($fields);

		if (!$ret) {
			switch ($this->GetValidationError()) {
				case self::VALIDATION_ERROR:
					$response = new CControllerResponseRedirect('zabbix.php?action=proxy.edit');
					$response->setFormData($this->getInputAll());
					CMessageHelper::setErrorTitle(_('Cannot add proxy'));
					$this->setResponse($response);
					break;
				case self::VALIDATION_FATAL_ERROR:
					$this->setResponse(new CControllerResponseFatal());
					break;
			}
		}

		return $ret;
	}

	protected function checkPermissions() {
		if (!$this->checkAccess(CRoleHelper::UI_ADMINISTRATION_PROXIES)) {
			return false;
		}

		$clone_psk = $this->hasInput('clone_proxyid') && $this->getInput('psk_edit_mode', 0) == 0;

		if ($clone_psk) {
			$clone_psk = $this->getInput('tls_connect', HOST_ENCRYPTION_NONE) == HOST_ENCRYPTION_PSK
				|| ($this->getInput('tls_accept', HOST_ENCRYPTION_NONE) & HOST_ENCRYPTION_PSK);
		}

		if ($clone_psk) {
			$this->clone_proxy = API::Proxy()->get([
				'output' => ['tls_psk_identity', 'tls_psk'],
				'proxyids' => $this->getInput('clone_proxyid')
			]);

			if (!$this->clone_proxy) {
				return false;
			}

			$this->clone_proxy = $this->clone_proxy[0];
		}

		return true;
	}

	protected function doAction() {
		$proxy = [
			'tls_connect' => $this->getInput('tls_connect', HOST_ENCRYPTION_NONE),
			'tls_accept' => $this->getInput('tls_accept', HOST_ENCRYPTION_NONE)
		];
		$fields = ['host', 'status', 'description'];

		if ($proxy['tls_connect'] == HOST_ENCRYPTION_CERTIFICATE
				|| ($proxy['tls_accept'] & HOST_ENCRYPTION_CERTIFICATE)) {
			array_push($fields, 'tls_issuer', 'tls_subject');
		}

		if ($proxy['tls_connect'] == HOST_ENCRYPTION_PSK || ($proxy['tls_accept'] & HOST_ENCRYPTION_PSK)) {
			if ($this->getInput('psk_edit_mode', 0) == 1) {
				array_push($fields, 'tls_psk', 'tls_psk_identity');
			}
			elseif ($this->hasInput('clone_proxyid')) {
				$proxy['tls_psk_identity'] = $this->clone_proxy['tls_psk_identity'];
				$proxy['tls_psk'] = $this->clone_proxy['tls_psk'];
			}
		}

		if ($this->getInput('status', HOST_STATUS_PROXY_ACTIVE) == HOST_STATUS_PROXY_PASSIVE) {
			$proxy['interface'] = [];
			$this->getInputs($proxy['interface'], ['dns', 'ip', 'useip', 'port']);
		}
		else {
			array_push($fields, 'proxy_address');
		}

		$this->getInputs($proxy, $fields);

		DBstart();

		$result = API::Proxy()->create([$proxy]);

		$result = DBend($result);

		if ($result) {
			$response = new CControllerResponseRedirect((new CUrl('zabbix.php'))
				->setArgument('action', 'proxy.list')
				->setArgument('page', CPagerHelper::loadPage('proxy.list', null))
			);
			$response->setFormData(['uncheck' => '1']);
			CMessageHelper::setSuccessTitle(_('Proxy added'));
		}
		else {
			$response = new CControllerResponseRedirect((new CUrl('zabbix.php'))
				->setArgument('action', 'proxy.edit')
			);
			$response->setFormData($this->getInputAll());
			CMessageHelper::setErrorTitle(_('Cannot add proxy'));
		}
		$this->setResponse($response);
	}
}
