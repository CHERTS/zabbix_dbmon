<?php
/*
** Zabbix
** Copyright (C) 2001-2021 Zabbix SIA
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

require_once dirname(__FILE__).'/common/testFormMacros.php';

/**
 * @backup hosts
 */
class testFormMacrosHost extends testFormMacros {

	use MacrosTrait;

	/**
	 * The name of the host for updating macros, id=20006.
	 *
	 * @var string
	 */
	protected $host_name_update = 'Host for trigger description macros';

	/**
	 * The name of the host for removing macros, id=30010.
	 *
	 * @var string
	 */
	protected $host_name_remove = 'Host for macros remove';

	/**
	 * The id of the host for removing inherited macros.
	 *
	 * @var integer
	 */
	protected static $hostid_remove_inherited;

	/**
	 * @dataProvider getCreateMacrosData
	 */
	public function testFormMacrosHost_Create($data) {
		$this->checkMacros($data, 'host');
	}

	/**
	 * @dataProvider getUpdateMacrosData
	 */
	public function testFormMacrosHost_Update($data) {
		$this->checkMacros($data, 'host', $this->host_name_update, true);
	}

	public function testFormMacrosHost_RemoveAll() {
		$this->checkRemoveAll($this->host_name_remove, 'host');
	}

	/**
	 * @dataProvider getCheckInheritedMacrosData
	 */
	public function testFormMacrosHost_ChangeInheritedMacro($data) {
		$this->checkChangeInheritedMacros($data, 'host');
	}

	public function prepareHostRemoveMacrosData() {
		$response = CDataHelper::call('host.create', [
				'host' => 'Host for Inherited macros removing',
				'groups' => [
					['groupid' => '4']
				],
				'interfaces' => [
					'type'=> 1,
					'main' => 1,
					'useip' => 1,
					'ip' => '192.168.3.1',
					'dns' => '',
					'port' => '10050'
				],
				'macros' => [
					[
						'macro' => '{$TEST_MACRO123}',
						'value' => 'test123',
						'description' => 'description 123'
					],
					[
						'macro' => '{$MACRO_FOR_DELETE_HOST1}',
						'value' => 'test1',
						'description' => 'description 1'
					],
					[
						'macro' => '{$MACRO_FOR_DELETE_HOST2}',
						'value' => 'test2',
						'description' => 'description 2'
					],
					[
						'macro' => '{$MACRO_FOR_DELETE_GLOBAL1}',
						'value' => 'test global 1',
						'description' => 'global description 1'
					],
					[
						'macro' => '{$MACRO_FOR_DELETE_GLOBAL2}',
						'value' => 'test global 2',
						'description' => 'global description 2'
					],
					[
						'macro' => '{$SNMP_COMMUNITY}',
						'value' => 'redefined value',
						'description' => 'redefined description'
					]
				]
		]);
		$this->assertArrayHasKey('hostids', $response);
		self::$hostid_remove_inherited = $response['hostids'][0];
	}

	/**
	 * @dataProvider getRemoveInheritedMacrosData
	 *
	 * @onBeforeOnce prepareHostRemoveMacrosData
	 */
	public function testFormMacrosHost_RemoveInheritedMacro($data) {
		$this->checkRemoveInheritedMacros($data, self::$hostid_remove_inherited, 'host');
	}

	public function getSecretMacrosLayoutData() {
		return [
			[
				[
					'macro' => '{$SECRET_HOST_MACRO}',
					'type' => 'Secret text'
				]
			],
			[
				[
					'macro' => '{$SECRET_HOST_MACRO}',
					'type' => 'Secret text',
					'chenge_type' => true
				]
			],
			[
				[
					'macro' => '{$TEXT_HOST_MACRO}',
					'type' => 'Text'
				]
			],
			[
				[
					'global' => true,
					'macro' => '{$X_TEXT_2_SECRET}',
					'type' => 'Text'
				]
			],
			[
				[
					'global' => true,
					'macro' => '{$X_SECRET_2_SECRET}',
					'type' => 'Secret text'
				]
			]
		];
	}

	/**
	 * @dataProvider getSecretMacrosLayoutData
	 */
	public function testFormMacrosHost_CheckSecretMacrosLayout($data) {
		$this->checkSecretMacrosLayout($data, 'hosts.php?form=update&hostid=99011', 'hosts');
	}

	public function getCreateSecretMacrosData() {
		return [
			[
				[
					'macro_fields' => [
						'action' => USER_ACTION_UPDATE,
						'index' => 0,
						'macro' => '{$SECRET_MACRO}',
						'value' => [
							'text' => 'host secret value',
							'type' => 'Secret text'
						],
						'description' => 'secret description'
					],
					'check_default_type' => true
				]
			],
			[
				[
					'macro_fields' => [
						'macro' => '{$TEXT_MACRO}',
						'value' => [
							'text' => 'host plain text value',
							'type' => 'Secret text'
						],
						'description' => 'plain text description'
					],
					'back_to_text' => true
				]
			],
			[
				[
					'macro_fields' => [
						'macro' => '{$SECRET_EMPTY_MACRO}',
						'value' => [
							'text' => '',
							'type' => 'Secret text'
						],
						'description' => 'secret empty value'
					]
				]
			]
		];
	}

	/**
	 * @dataProvider getCreateSecretMacrosData
	 */
	public function testFormMacrosHost_CreateSecretMacros($data) {
		$this->createSecretMacros($data, 'hosts.php?form=update&hostid=99134', 'hosts');
	}

	public function getRevertSecretMacrosData() {
		return [
			[
				[
					'macro_fields' => [
						'macro' => '{$SECRET_HOST_MACRO_REVERT}',
						'value' => 'Secret host value'
					]
				]
			],
			[
				[
					'macro_fields' => [
						'macro' => '{$SECRET_HOST_MACRO_2_TEXT_REVERT}',
						'value' => 'Secret host value 2'
					],
					'set_to_text' => true
				]
			]
		];
	}

	/**
	 * @dataProvider getRevertSecretMacrosData
	 */
	public function testFormMacrosHost_RevertSecretMacroChanges($data) {
		$this->revertSecretMacroChanges($data, 'hosts.php?form=update&hostid=99135', 'hosts');
	}

	public function getUpdateSecretMacrosData() {
		return [
			[
				[
					'action' => USER_ACTION_UPDATE,
					'index' => 2,
					'macro' => '{$SECRET_HOST_MACRO_UPDATE}',
					'value' => [
						'text' => 'Updated secret value'
					]
				]
			],
			[
				[
					'action' => USER_ACTION_UPDATE,
					'index' => 3,
					'macro' => '{$SECRET_HOST_MACRO_UPDATE_2_TEXT}',
					'value' => [
						'text' => 'New text value',
						'type' => 'Text'
					]
				]
			],
			[
				[
					'action' => USER_ACTION_UPDATE,
					'index' => 4,
					'macro' => '{$TEXT_HOST_MACRO_2_SECRET}',
					'value' => [
						'text' => 'New secret value',
						'type' => 'Secret text'
					]
				]
			]
		];
	}

	/**
	 * @dataProvider getUpdateSecretMacrosData
	 */
	public function testFormMacrosHost_UpdateSecretMacros($data) {
		$this->updateSecretMacros($data, 'hosts.php?form=update&hostid=99135', 'hosts');
	}

	public function testFormMacrosHost_ResolveSecretMacro() {
		$macro = [
			'macro' => '{$X_SECRET_HOST_MACRO_2_RESOLVE}',
			'value' => 'Value 2 B resolved'
		];
		$this->resolveSecretMacro($macro, 'hosts.php?form=update&hostid=99135', 'hosts', 'host');
	}
}
