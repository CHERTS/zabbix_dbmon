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

require_once dirname(__FILE__).'/common/testFormAdministrationGeneral.php';

/**
 * @backup config
 */
class testFormAdministrationGeneralOtherParams extends testFormAdministrationGeneral {

	public $config_link = 'zabbix.php?action=miscconfig.edit';
	public $form_selector = 'name:otherForm';

	public $default_values = [
		'Frontend URL' => '',
		'Group for discovered hosts' => 'Empty group',
		'Default host inventory mode' => 'Disabled',
		'User group for database down message' => 'Zabbix administrators',
		'Log unmatched SNMP traps' => true,
		// Authorization.
		'Login attempts' => 5,
		'Login blocking interval' => '30s',
		// Security.
		'id:validate_uri_schemes' => true,
		'id:uri_valid_schemes' => 'http,https,ftp,file,mailto,tel,ssh',
		'id:x_frame_header_enabled' => true,
		'id:x_frame_options' => 'SAMEORIGIN',
		'id:iframe_sandboxing_enabled' => true,
		'id:iframe_sandboxing_exceptions' => '',
		// Communication with Zabbix server.
		'Network timeout' => '3s',
		'Connection timeout' => '3s',
		'Network timeout for media type test' => '65s',
		'Network timeout for script execution' => '60s',
		'Network timeout for item test' => '60s'
	];

	public $db_default_values = [
		'url' => '',
		'discovery_groupid' => 50006,
		'default_inventory_mode' => -1,
		'alert_usrgrpid' => 7,
		'snmptrap_logging' => 1,
		// Authorization.
		'login_attempts' => 5,
		'login_block' => '30s',
		// Security.
		'validate_uri_schemes' => 1,
		'uri_valid_schemes' => 'http,https,ftp,file,mailto,tel,ssh',
		'x_frame_options' => 'SAMEORIGIN',
		'iframe_sandboxing_enabled' => 1,
		'iframe_sandboxing_exceptions' => '',
		// Communication with Zabbix server.
		'socket_timeout' => '3s',
		'connect_timeout' => '3s',
		'media_type_test_timeout' => '65s',
		'script_timeout' => '60s',
		'item_test_timeout' => '60s'
	];

	public $custom_values = [
		'Frontend URL' => 'http://zabbix.com',
		'Group for discovered hosts' => 'Hypervisors',
		'Default host inventory mode' => 'Automatic',
		'User group for database down message' => 'Test timezone',
		'Log unmatched SNMP traps' => false,
		// Authorization.
		'Login attempts' => 13,
		'Login blocking interval' => '52s',
		// Security.
		'id:validate_uri_schemes' => true,
		'id:uri_valid_schemes' => 'custom_scheme',
		'id:x_frame_header_enabled' => true,
		'id:x_frame_options' => 'SOME-NEW-VALUE',
		'id:iframe_sandboxing_enabled' => true,
		'id:iframe_sandboxing_exceptions' => 'some-new-flag',
		// Communication with Zabbix server.
		'Network timeout' => '7s',
		'Connection timeout' => '4s',
		'Network timeout for media type test' => '91s',
		'Network timeout for script execution' => '46s',
		'Network timeout for item test' => '76s'
	];

	/**
	 * Test for checking form layout.
	 */
	public function testFormAdministrationGeneralOtherParams_CheckLayout() {
		$this->page->login()->open($this->config_link);
		$this->page->assertTitle('Other configuration parameters');
		$this->page->assertHeader('Other configuration parameters');
		$form = $this->query($this->form_selector)->waitUntilReady()->asForm()->one();

		foreach (['Authorization', 'Security', 'Communication with Zabbix server'] as $header) {
			$this->assertTrue($this->query('xpath://h4[text()="'.$header.'"]')->one()->isVisible());
		}

		$limits = [
			'url' => 255,
			'login_attempts' => 2,
			'login_block' => 32,
			'uri_valid_schemes' => 255,
			'x_frame_options' => 255,
			'iframe_sandboxing_exceptions' => 255,
			'socket_timeout' => 32,
			'connect_timeout' => 32,
			'media_type_test_timeout' => 32,
			'script_timeout' => 32,
			'item_test_timeout' => 32
		];
		foreach ($limits as $id => $limit) {
			$this->assertEquals($limit, $this->query('id', $id)->one()->getAttribute('maxlength'));
		}

		foreach ([true, false] as $status) {
			$checkboxes = [
				'snmptrap_logging',
				'validate_uri_schemes',
				'x_frame_header_enabled',
				'iframe_sandboxing_enabled'
			];
			foreach ($checkboxes as $checkbox) {
				$form->getField('id:'.$checkbox)->fill($status);
			}

			foreach (['uri_valid_schemes','iframe_sandboxing_exceptions', 'x_frame_options'] as $input) {
				$this->assertTrue($this->query('id', $input)->one()->isEnabled($status));
			}
		}

		// Check X-Frame-Options hintbox.
		$form->getLabel('Use X-Frame-Options HTTP header')->query('xpath:./a[@data-hintbox]')->one()->waitUntilClickable()->click();
		$hint = $this->query('xpath://div[@class="overlay-dialogue"]')->asOverlayDialog()->waitUntilPresent()->one();

		$hint_text = "X-Frame-Options HTTP header supported values:\n".
				"SAMEORIGIN or 'self' - allows the page to be displayed only in a frame on the same origin as the page itself\n".
				"DENY or 'none' - prevents the page from being displayed in a frame, regardless of the site attempting to do so\n".
				"a string of space-separated hostnames; adding 'self' to the list allows the page to be displayed in a frame on the same origin as the page itself\n".
				"\n".
				"Note that 'self' or 'none' will be regarded as hostnames if used without single quotes.";

		$this->assertEquals($hint_text, $hint->getText());
		$hint->close();

		foreach (['Update', 'Reset defaults'] as $button) {
			$this->assertTrue($this->query('button', $button)->one()->isEnabled());
		}
	}

	/**
	 * Test for checking form update without changing any data.
	 */
	public function testFormAdministrationGeneralOtherParams_SimpleUpdate() {
		$this->executeSimpleUpdate();
	}

	/**
	 * Test for checking 'Reset defaults' button.
	 */
	public function testFormAdministrationGeneralOtherParams_ResetButton() {
		$this->executeResetButtonTest(true);
	}

	/**
	 * Test data for Other parameters form.
	 */
	public function getCheckFormData() {
		return [
			// #0 Minimal valid values. In period fields minimal valid time in seconds with 's'.
			[
				[
					'fields' => [
						'Frontend URL' => 'a',
						'Group for discovered hosts' => 'Hypervisors',
						'Default host inventory mode' => 'Manual',
						'User group for database down message' => 'Test timezone',
						'Log unmatched SNMP traps' => false,
						// Authorization.
						'Login attempts' => 1,
						'Login blocking interval' => '30s',
						// Security.
						'id:validate_uri_schemes' => false,
						'id:x_frame_header_enabled' => true,
						'id:x_frame_options' => 'X',
						'id:iframe_sandboxing_enabled' => false,
						// Communication with Zabbix server.
						'Network timeout' => '1s',
						'Connection timeout' => '1s',
						'Network timeout for media type test' => '1s',
						'Network timeout for script execution' => '1s',
						'Network timeout for item test' => '1s'
					],
					'db' => [
						'url' => 'a',
						'discovery_groupid' => 7,
						'default_inventory_mode' => 0,
						'alert_usrgrpid' => 92,
						'snmptrap_logging' => 0,
						// Authorization.
						'login_attempts' => 1,
						'login_block' => '30s',
						// Security.
						'validate_uri_schemes' => 0,
						'x_frame_options' => 'X',
						'iframe_sandboxing_enabled' => 0,
						// Communication with Zabbix server.
						'socket_timeout' => '1s',
						'connect_timeout' => '1s',
						'media_type_test_timeout' => '1s',
						'script_timeout' => '1s',
						'item_test_timeout' => '1s'
					]
				]
			],
			// #1 Minimal valid values. In period fields minimal valid time in seconds without 's'.
			[
				[
					'fields' => [
						'Frontend URL' => 'zabbix.php',
						'Default host inventory mode' => 'Automatic',
						'Log unmatched SNMP traps' => true,
						// Authorization.
						'Login blocking interval' => '30',
						// Security.
						'id:validate_uri_schemes' => true,
						'id:uri_valid_schemes' => '',
						'id:iframe_sandboxing_enabled' => true,
						'id:iframe_sandboxing_exceptions' => '',
						// Communication with Zabbix server.
						'Network timeout' => '1',
						'Connection timeout' => '1',
						'Network timeout for media type test' => '1',
						'Network timeout for script execution' => '1',
						'Network timeout for item test' => '1'
					],
					'db' => [
						'url' => 'zabbix.php',
						'default_inventory_mode' => 1,
						'snmptrap_logging' => 1,
						// Authorization.
						'login_block' => '30',
						// Security.
						'validate_uri_schemes' => 1,
						'uri_valid_schemes' => '',
						'iframe_sandboxing_enabled' => 1,
						'iframe_sandboxing_exceptions' => '',
						// Communication with Zabbix server.
						'socket_timeout' => '1',
						'connect_timeout' => '1',
						'media_type_test_timeout' => '1',
						'script_timeout' => '1',
						'item_test_timeout' => '1'
					]
				]
			],
			// #2 In period fields minimal valid time in minutes.
			[
				[
					'fields' => [
						// Authorization.
						'Login blocking interval' => '1m',
						// Communication with Zabbix server.
						'Network timeout' => '1m',
						'Network timeout for media type test' => '1m',
						'Network timeout for script execution' => '1m',
						'Network timeout for item test' => '1m'
					],
					'db' => [
						// Authorization.
						'login_block' => '1m',
						// Communication with Zabbix server.
						'socket_timeout' => '1m',
						'media_type_test_timeout' => '1m',
						'script_timeout' => '1m',
						'item_test_timeout' => '1m'
					]
				]
			],
			// #3 In period fields minimal valid time in hours.
			[
				[
					'fields' => [
						// Authorization.
						'Login blocking interval' => '1h'
					],
					'db' => [
						// Authorization.
						'login_block' => '1h'
					]
				]
			],
			// #4 Maximal valid values in seconds with "s".
			[
				[
					'fields' => [
						// Authorization.
						'Login attempts' => 32,
						'Login blocking interval' => '3600s',
						// Security.
						'id:validate_uri_schemes' => true,
						'id:uri_valid_schemes' => 'http,https,ftp,file,mailto,tel,ssh,http,https,ftp,file,mailto,tel,ssh,http,'.
								'https,ftp,file,mailto,tel,ssh,http,https,ftp,file,mailto,tel,ssh,http,https,ftp,file,mailto,'.
								'tel,ssh,http,https,ftp,file,mailto,tel,ssh,http,https,ftp,file,mailto,tel,ssh,http,https',
						'id:x_frame_header_enabled' => true,
						'id:x_frame_options' => 'SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN '.
								'SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN '.
								'SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SA',
						'id:iframe_sandboxing_enabled' => true,
						'id:iframe_sandboxing_exceptions' => 'some-new-flag-some-new-flag-some-new-flag-some-new-flag-some-new-'.
								'flag-some-new-flag-some-new-flag-some-new-flag-some-new-flag-some-new-flag-some-new-flag-some-new-'.
								'flag-some-new-flag-some-new-flag-some-new-flag-some-new-flag-some-new-flag-some-new-flag-som',
						// Communication with Zabbix server.
						'Network timeout' => '300s',
						'Connection timeout' => '30s',
						'Network timeout for media type test' => '300s',
						'Network timeout for script execution' => '300s',
						'Network timeout for item test' => '300s'
					],
					'db' => [
						// Authorization.
						'login_attempts' => 32,
						'login_block' => '3600s',
						// Security.
						'validate_uri_schemes' => 1,
						'uri_valid_schemes' => 'http,https,ftp,file,mailto,tel,ssh,http,https,ftp,file,mailto,tel,ssh,http,https,'.
								'ftp,file,mailto,tel,ssh,http,https,ftp,file,mailto,tel,ssh,http,https,ftp,file,mailto,tel,ssh,'.
						'http,https,ftp,file,mailto,tel,ssh,http,https,ftp,file,mailto,tel,ssh,http,https',
						'x_frame_options' => 'SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN '.
								'SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN '.
								'SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SAMEORIGIN SA',
						'iframe_sandboxing_enabled' => 1,
						'iframe_sandboxing_exceptions' => 'some-new-flag-some-new-flag-some-new-flag-some-new-flag-some-new-flag'.
								'-some-new-flag-some-new-flag-some-new-flag-some-new-flag-some-new-flag-some-new-flag-some-new-flag'.
								'-some-new-flag-some-new-flag-some-new-flag-some-new-flag-some-new-flag-some-new-flag-som',
						// Communication with Zabbix server.
						'socket_timeout' => '300s',
						'connect_timeout' => '30s',
						'media_type_test_timeout' => '300s',
						'script_timeout' => '300s',
						'item_test_timeout' => '300s'
					]
				]
			],
			// #5 In period fields maximal valid values in seconds without "s".
			[
				[
					'fields' => [
						// Authorization.
						'Login blocking interval' => '3600',
						// Communication with Zabbix server.
						'Network timeout' => '300',
						'Connection timeout' => '30',
						'Network timeout for media type test' => '300',
						'Network timeout for script execution' => '300',
						'Network timeout for item test' => '300'
					],
					'db' => [
						// Authorization.
						'login_block' => '3600',
						// Communication with Zabbix server.
						'socket_timeout' => '300',
						'connect_timeout' => '30',
						'media_type_test_timeout' => '300',
						'script_timeout' => '300',
						'item_test_timeout' => '300'
					]
				]
			],
			// #6 In period fields maximal valid values in minutes.
			[
				[
					'fields' => [
						// Authorization.
						'Login blocking interval' => '60m',
						// Communication with Zabbix server.
						'Network timeout' => '5m',
						'Network timeout for media type test' => '5m',
						'Network timeout for script execution' => '5m',
						'Network timeout for item test' => '5m'
					],
					'db' => [
						// Authorization.
						'login_block' => '60m',
						// Communication with Zabbix server.
						'socket_timeout' => '5m',
						'media_type_test_timeout' => '5m',
						'script_timeout' => '5m',
						'item_test_timeout' => '5m'
					]
				]
			],
			// #7 Symbol trimming in Login attempts.
			[
				[
					'fields' => [
						'Login attempts' => '3M'
					],
					'db' => [
						'login_attempts' => 3
					]
				]
			],
			// #8 Invalid empty values.
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						'Group for discovered hosts' => '',
						'User group for database down message' => '',
						// Authorization.
						'Login attempts' => '',
						'Login blocking interval' => '',
						// Security.
						'id:x_frame_options' => '',
						// Communication with Zabbix server.
						'Network timeout' => '',
						'Connection timeout' => '',
						'Network timeout for media type test' => '',
						'Network timeout for script execution' => '',
						'Network timeout for item test' => ''
					],
					'details' => [
						'Incorrect value for field "login_attempts": value must be no less than "1".',
						'Incorrect value for field "login_block": a time unit is expected.',
						'Incorrect value for field "socket_timeout": a time unit is expected.',
						'Incorrect value for field "connect_timeout": a time unit is expected.',
						'Incorrect value for field "media_type_test_timeout": a time unit is expected.',
						'Incorrect value for field "script_timeout": a time unit is expected.',
						'Incorrect value for field "item_test_timeout": a time unit is expected.'
					]
				]
			],
			// #9 Invalid string values.
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						// Authorization.
						'Login attempts' => 'text',
						'Login blocking interval' => 'text',
						// Communication with Zabbix server.
						'Network timeout' => 'text',
						'Connection timeout' => 'text',
						'Network timeout for media type test' => 'text',
						'Network timeout for script execution' => 'text',
						'Network timeout for item test' => 'text'
					],
					'details' => [
						'Incorrect value for field "login_attempts": value must be no less than "1".',
						'Incorrect value for field "login_block": a time unit is expected.',
						'Incorrect value for field "socket_timeout": a time unit is expected.',
						'Incorrect value for field "connect_timeout": a time unit is expected.',
						'Incorrect value for field "media_type_test_timeout": a time unit is expected.',
						'Incorrect value for field "script_timeout": a time unit is expected.',
						'Incorrect value for field "item_test_timeout": a time unit is expected.'
					]
				]
			],
			// #10 Invalid special symbol values.
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						// Authorization.
						'Login attempts' => '!@#$%^&*()_+',
						'Login blocking interval' => '!@#$%^&*()_+',
						// Communication with Zabbix server.
						'Network timeout' => '!@#$%^&*()_+',
						'Connection timeout' => '!@#$%^&*()_+',
						'Network timeout for media type test' => '!@#$%^&*()_+',
						'Network timeout for script execution' => '!@#$%^&*()_+',
						'Network timeout for item test' => '!@#$%^&*()_+'
					],
					'details' => [
						'Incorrect value for field "login_attempts": value must be no less than "1".',
						'Incorrect value for field "login_block": a time unit is expected.',
						'Incorrect value for field "socket_timeout": a time unit is expected.',
						'Incorrect value for field "connect_timeout": a time unit is expected.',
						'Incorrect value for field "media_type_test_timeout": a time unit is expected.',
						'Incorrect value for field "script_timeout": a time unit is expected.',
						'Incorrect value for field "item_test_timeout": a time unit is expected.'
					]
				]
			],
			// #11 Invalid zero values.
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						// Authorization.
						'Login attempts' => 0,
						'Login blocking interval' => 0,
						// Communication with Zabbix server.
						'Network timeout' => 0,
						'Connection timeout' => 0,
						'Network timeout for media type test' => 0,
						'Network timeout for script execution' => 0,
						'Network timeout for item test' => 0
					],
					'details' => [
						'Incorrect value for field "login_attempts": value must be no less than "1".',
						'Incorrect value for field "login_block": value must be one of 30-3600.',
						'Incorrect value for field "socket_timeout": value must be one of 1-300.',
						'Incorrect value for field "connect_timeout": value must be one of 1-30.',
						'Incorrect value for field "media_type_test_timeout": value must be one of 1-300.',
						'Incorrect value for field "script_timeout": value must be one of 1-300.',
						'Incorrect value for field "item_test_timeout": value must be one of 1-300.'
					]
				]
			],
			// #12 Invalid zero values in seconds with "s".
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						// Authorization.
						'Login blocking interval' => '0s',
						// Communication with Zabbix server.
						'Network timeout' => '0s',
						'Connection timeout' => '0s',
						'Network timeout for media type test' => '0s',
						'Network timeout for script execution' => '0s',
						'Network timeout for item test' => '0s'
					],
					'details' => [
						'Incorrect value for field "login_block": value must be one of 30-3600.',
						'Incorrect value for field "socket_timeout": value must be one of 1-300.',
						'Incorrect value for field "connect_timeout": value must be one of 1-30.',
						'Incorrect value for field "media_type_test_timeout": value must be one of 1-300.',
						'Incorrect value for field "script_timeout": value must be one of 1-300.',
						'Incorrect value for field "item_test_timeout": value must be one of 1-300.'
					]
				]
			],
			// #13 In period fields minimal invalid time in seconds without "s".
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						// Authorization.
						'Login blocking interval' => '29'
					],
					'details' => [
						'Incorrect value for field "login_block": value must be one of 30-3600.'
					]
				]
			],
			// #14 In period fields minimal invalid time in seconds with "s".
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						// Authorization.
						'Login blocking interval' => '29s'
					],
					'details' => [
						'Incorrect value for field "login_block": value must be one of 30-3600.'
					]
				]
			],
			// #15 In period fields maximal invalid time in seconds without "s".
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						// Authorization.
						'Login attempts' => 33,
						'Login blocking interval' => '3601',
						// Communication with Zabbix server.
						'Network timeout' => '301',
						'Connection timeout' => '31',
						'Network timeout for media type test' => '301',
						'Network timeout for script execution' => '301',
						'Network timeout for item test' => '301'
					],
					'details' => [
						'Incorrect value for field "login_attempts": value must be no greater than "32".',
						'Incorrect value for field "login_block": value must be one of 30-3600.',
						'Incorrect value for field "socket_timeout": value must be one of 1-300.',
						'Incorrect value for field "connect_timeout": value must be one of 1-30.',
						'Incorrect value for field "media_type_test_timeout": value must be one of 1-300.',
						'Incorrect value for field "script_timeout": value must be one of 1-300.',
						'Incorrect value for field "item_test_timeout": value must be one of 1-300.'
					]
				]
			],
			// #16 Maximal invalid time in seconds with "s".
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						// Authorization.
						'Login blocking interval' => '3601s',
						// Communication with Zabbix server.
						'Network timeout' => '301s',
						'Connection timeout' => '31s',
						'Network timeout for media type test' => '301s',
						'Network timeout for script execution' => '301s',
						'Network timeout for item test' => '301s'
					],
					'details' => [
						'Incorrect value for field "login_block": value must be one of 30-3600.',
						'Incorrect value for field "socket_timeout": value must be one of 1-300.',
						'Incorrect value for field "connect_timeout": value must be one of 1-30.',
						'Incorrect value for field "media_type_test_timeout": value must be one of 1-300.',
						'Incorrect value for field "script_timeout": value must be one of 1-300.',
						'Incorrect value for field "item_test_timeout": value must be one of 1-300.'
					]
				]
			],
			// #17 Maximal invalid time in minutes.
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						// Authorization.
						'Login blocking interval' => '61m',
						// Communication with Zabbix server.
						'Network timeout' => '6m',
						'Connection timeout' => '1m',
						'Network timeout for media type test' => '6m',
						'Network timeout for script execution' => '6m',
						'Network timeout for item test' => '6m'
					],
					'details' => [
						'Incorrect value for field "login_block": value must be one of 30-3600.',
						'Incorrect value for field "socket_timeout": value must be one of 1-300.',
						'Incorrect value for field "connect_timeout": value must be one of 1-30.',
						'Incorrect value for field "media_type_test_timeout": value must be one of 1-300.',
						'Incorrect value for field "script_timeout": value must be one of 1-300.',
						'Incorrect value for field "item_test_timeout": value must be one of 1-300.'
					]
				]
			],
			// #18 Maximal invalid time in hours.
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						// Authorization.
						'Login blocking interval' => '2h',
						// Communication with Zabbix server.
						'Network timeout' => '1h',
						'Connection timeout' => '1h',
						'Network timeout for media type test' => '1h',
						'Network timeout for script execution' => '1h',
						'Network timeout for item test' => '1h'
					],
					'details' => [
						'Incorrect value for field "login_block": value must be one of 30-3600.',
						'Incorrect value for field "socket_timeout": value must be one of 1-300.',
						'Incorrect value for field "connect_timeout": value must be one of 1-30.',
						'Incorrect value for field "media_type_test_timeout": value must be one of 1-300.',
						'Incorrect value for field "script_timeout": value must be one of 1-300.',
						'Incorrect value for field "item_test_timeout": value must be one of 1-300.'
					]
				]
			],
			// #19 Maximal invalid time in weeks.
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						// Authorization.
						'Login blocking interval' => '1w',
						// Communication with Zabbix server.
						'Network timeout' => '1w',
						'Connection timeout' => '1w',
						'Network timeout for media type test' => '1w',
						'Network timeout for script execution' => '1w',
						'Network timeout for item test' => '1w'
					],
					'details' => [
						'Incorrect value for field "login_block": value must be one of 30-3600.',
						'Incorrect value for field "socket_timeout": value must be one of 1-300.',
						'Incorrect value for field "connect_timeout": value must be one of 1-30.',
						'Incorrect value for field "media_type_test_timeout": value must be one of 1-300.',
						'Incorrect value for field "script_timeout": value must be one of 1-300.',
						'Incorrect value for field "item_test_timeout": value must be one of 1-300.'
					]
				]
			],
			// #20 Maximal invalid time in Months (Months not supported).
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						// Authorization.
						'Login blocking interval' => '1M',
						// Communication with Zabbix server.
						'Network timeout' => '1M',
						'Connection timeout' => '1M',
						'Network timeout for media type test' => '1M',
						'Network timeout for script execution' => '1M',
						'Network timeout for item test' => '1M'
					],
					'details' => [
						'Incorrect value for field "login_block": a time unit is expected.',
						'Incorrect value for field "socket_timeout": a time unit is expected.',
						'Incorrect value for field "connect_timeout": a time unit is expected.',
						'Incorrect value for field "media_type_test_timeout": a time unit is expected.',
						'Incorrect value for field "script_timeout": a time unit is expected.',
						'Incorrect value for field "item_test_timeout": a time unit is expected.'
					]
				]
			],
			// #21 Maximal invalid time in years (years not supported).
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						// Authorization.
						'Login blocking interval' => '1y',
						// Communication with Zabbix server.
						'Network timeout' => '1y',
						'Connection timeout' => '1y',
						'Network timeout for media type test' => '1y',
						'Network timeout for script execution' => '1y',
						'Network timeout for item test' => '1y'
					],
					'details' => [
						'Incorrect value for field "login_block": a time unit is expected.',
						'Incorrect value for field "socket_timeout": a time unit is expected.',
						'Incorrect value for field "connect_timeout": a time unit is expected.',
						'Incorrect value for field "media_type_test_timeout": a time unit is expected.',
						'Incorrect value for field "script_timeout": a time unit is expected.',
						'Incorrect value for field "item_test_timeout": a time unit is expected.'
					]
				]
			],
			// #22 Maximal invalid values.
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						// Authorization.
						'Login attempts' => '99',
						'Login blocking interval' => '99999999999999999999999999999999',
						// Communication with Zabbix server.
						'Network timeout' => '99999999999999999999999999999999',
						'Connection timeout' => '99999999999999999999999999999999',
						'Network timeout for media type test' => '99999999999999999999999999999999',
						'Network timeout for script execution' => '99999999999999999999999999999999',
						'Network timeout for item test' => '99999999999999999999999999999999'
					],
					'details' => [
						'Incorrect value for field "login_attempts": value must be no greater than "32".',
						'Incorrect value for field "login_block": value must be one of 30-3600.',
						'Incorrect value for field "socket_timeout": value must be one of 1-300.',
						'Incorrect value for field "connect_timeout": value must be one of 1-30.',
						'Incorrect value for field "media_type_test_timeout": value must be one of 1-300.',
						'Incorrect value for field "script_timeout": value must be one of 1-300.',
						'Incorrect value for field "item_test_timeout": value must be one of 1-300.'
					]
				]
			],
			// #23 Negative values.
			[
				[
					'expected' => TEST_BAD,
					'fields' => [
						// Authorization.
						'Login attempts' => '-1',
						'Login blocking interval' => '-1',
						// Communication with Zabbix server.
						'Network timeout' => '-1',
						'Connection timeout' => '-1',
						'Network timeout for media type test' => '-1',
						'Network timeout for script execution' => '-1',
						'Network timeout for item test' => '-1'
					],
					'details' => [
						'Incorrect value for field "login_block": a time unit is expected.',
						'Incorrect value for field "socket_timeout": a time unit is expected.',
						'Incorrect value for field "connect_timeout": a time unit is expected.',
						'Incorrect value for field "media_type_test_timeout": a time unit is expected.',
						'Incorrect value for field "script_timeout": a time unit is expected.',
						'Incorrect value for field "item_test_timeout": a time unit is expected.'
					]
				]
			],
			// #24 Trimming spaces.
			[
				[
					'trim' => true,
					'fields' => [
						'Frontend URL' => '    zabbix.php    ',
						// Authorization.
						'Login attempts' => ' 5',
						'Login blocking interval' => '    32s   ',
						// Security.
						'id:uri_valid_schemes' => '   mailto,tel,ssh   ',
						'id:x_frame_options' => '    SAMEORIGIN    ',
						'id:iframe_sandboxing_exceptions' => '   test   ',
						// Communication with Zabbix server.
						'Network timeout' => '  1m   ',
						'Connection timeout' => '   3s    ',
						'Network timeout for media type test' => '    1m    ',
						'Network timeout for script execution' => '    1m    ',
						'Network timeout for item test' => '    1m    ',
						'Network timeout for scheduled report test' => '    1m    '
					],
					'db' => [
						'url' => 'zabbix.php',
						// Authorization.
						'login_attempts' => 5,
						'login_block' => '32s',
						// Security.
						'uri_valid_schemes' => 'mailto,tel,ssh',
						'x_frame_options' => 'SAMEORIGIN',
						'iframe_sandboxing_exceptions' => 'test',
						// Communication with Zabbix server.
						'socket_timeout' => '1m',
						'connect_timeout' => '3s',
						'media_type_test_timeout' => '1m',
						'script_timeout' => '1m',
						'item_test_timeout' => '1m',
						'report_test_timeout' => '1m'
					]
				]
			]
		];
	}

	/**
	 * @dataProvider getCheckFormData
	 */
	public function testFormAdministrationGeneralOtherParams_CheckForm($data) {
		$this->executeCheckForm($data, true);
	}
}
