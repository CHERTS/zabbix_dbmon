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


require_once dirname(__FILE__).'/../../include/CWebTest.php';
require_once dirname(__FILE__).'/../../include/helpers/CDataHelper.php';

/**
 * @backup hosts
 *
 * @onBefore prepareProblemsData
 */
class testFormUpdateProblem extends CWebTest {

	/**
	 * Id of the host with problems.
	 *
	 * @var integer
	 */
	protected static $hostid;

	/**
	 * Time when acknowledge was created.
	 *
	 * @var string
	 */
	protected static $acktime;

	/**
	 * Attach MessageBehavior to the test.
	 *
	 * @return array
	 */
	public function getBehaviors() {
		return [CMessageBehavior::class];
	}

	/**
	 * Get all events related tables hash values.
	 */
	public static function getHash() {
		return CDBHelper::getHash('SELECT * FROM events').
				CDBHelper::getHash('SELECT * FROM problem').
				CDBHelper::getHash('SELECT * FROM triggers').
				CDBHelper::getHash('SELECT * FROM acknowledges').
				CDBHelper::getHash('SELECT * FROM event_suppress');
	}

	public function prepareProblemsData() {
		// Create hostgroup for hosts with items triggers.
		$hostgroups = CDataHelper::call('hostgroup.create', [['name' => 'Group for Problems Update']]);
		$this->assertArrayHasKey('groupids', $hostgroups);
		$groupid = $hostgroups['groupids'][0];

		// Create host for items and triggers.
		$hosts = CDataHelper::call('host.create', [
			'host' => 'Host for Problems Update',
			'groups' => [['groupid' => $groupid]]
		]);
		$this->assertArrayHasKey('hostids', $hosts);
		self::$hostid = $hosts['hostids'][0];

		// Create items on previously created host.
		$item_names = ['float', 'char', 'log', 'unsigned', 'text'];

		$items_data = [];
		foreach ($item_names as $i => $item) {
			$items_data[] = [
				'hostid' => self::$hostid,
				'name' => $item,
				'key_' => $item,
				'type' => 2,
				'value_type' => $i
			];
		}

		$items = CDataHelper::call('item.create', $items_data);
		$this->assertArrayHasKey('itemids', $items);

		// Create triggers based on items.
		$triggers = CDataHelper::call('trigger.create', [
			[
				'description' => 'Trigger for float',
				'expression' => 'last(/Host for Problems Update/float)=0',
				'priority' => TRIGGER_SEVERITY_NOT_CLASSIFIED
			],
			[
				'description' => 'Trigger for char',
				'expression' => 'last(/Host for Problems Update/char)=0',
				'priority' => TRIGGER_SEVERITY_INFORMATION,
				'manual_close' => 1
			],
			[
				'description' => 'Trigger for log',
				'expression' => 'last(/Host for Problems Update/log)=0',
				'priority' => TRIGGER_SEVERITY_WARNING
			],
			[
				'description' => 'Trigger for unsigned',
				'expression' => 'last(/Host for Problems Update/unsigned)=0',
				'priority' => TRIGGER_SEVERITY_AVERAGE
			],
			[
				'description' => 'Trigger for text',
				'expression' => 'last(/Host for Problems Update/text)=0',
				'priority' => TRIGGER_SEVERITY_HIGH
			]
		]);
		$this->assertArrayHasKey('triggerids', $triggers);

		// Create problems and events.
		$time = time();
		foreach (CDataHelper::getIds('description') as $name => $id) {
			CDBHelper::setTriggerProblem($name, TRIGGER_VALUE_TRUE, $time);
		}

		$eventids = [];
		foreach (['Trigger for text', 'Trigger for unsigned'] as $event_name) {
			$eventids[$event_name] = CDBHelper::getValue('SELECT eventid FROM events WHERE name='.zbx_dbstr($event_name));
		}

		// Suppress the problem: 'Trigger for text'.
		DBexecute('INSERT INTO event_suppress (event_suppressid, eventid, maintenanceid, suppress_until) VALUES (10050, '.
				$eventids['Trigger for text'].', NULL, 0)'
		);

		// Acknowledge the problem: 'Trigger for unsigned' and get acknowledge time.
		CDataHelper::call('event.acknowledge', [
			'eventids' => $eventids['Trigger for unsigned'],
			'action' => 6,
			'message' => 'Acknowledged event'
		]);

		$event = CDataHelper::call('event.get', [
			'eventids' => $eventids['Trigger for unsigned'],
			'select_acknowledges' => ['clock']
		]);
		self::$acktime = CTestArrayHelper::get($event, '0.acknowledges.0.clock');
	}

	public function getLayoutData() {
		return [
			[
				[
					'problems' => ['Trigger for float'],
					'hintboxes' => [
						'Suppress' => 'Manual problem suppression. Date-time input accepts relative and absolute time format.',
						'Unsuppress' => 'Deactivates manual suppression.',
						'Acknowledge' => 'Confirms the problem is noticed (acknowledging user will be recorded). '.
								'Status change triggers action update operation.'
					],
					'history' => [],
					'Acknowledge' => true,
					'check_suppress' => true
				]
			],
			[
				[
					'problems' => ['Trigger for char'],
					'close_enabled' => true,
					'history' => [],
					'Acknowledge' => true
				]
			],
			[
				[
					'problems' => ['Trigger for text'],
					'unsuppress_enabled' => true,
					'history' => [],
					'Acknowledge' => true
				]
			],
			[
				[
					'problems' => ['Trigger for unsigned'],
					// If problem is Acknowledged - label is changed to Unacknowledge.
					'labels' => ['Problem', 'Message', 'History', 'Scope', 'Change severity', 'Unacknowledge', 'Close problem', ''],
					'message' => 'Acknowledged event',
					'Unacknowledge' => true,
					'history' => [' Admin (Zabbix Administrator) Acknowledged event'],
					'hintboxes' => [
						'Suppress' => 'Manual problem suppression. Date-time input accepts relative and absolute time format.',
						'Unsuppress' => 'Deactivates manual suppression.',
						'Unacknowledge' => 'Undo problem acknowledgement.'
					]
				]
			],
			[
				[
					'problems' => ['Trigger for log'],
					'history' => [],
					'Acknowledge' => true
				]
			],
			// Two problems.
			[
				[
					'problems' => ['Trigger for float', 'Trigger for char'],
					// If more than one problem selected - History label is absent.
					'labels' => ['Problem', 'Message', 'Scope', 'Change severity', 'Acknowledge', 'Close problem', ''],
					'close_enabled' => true,
					'Acknowledge' => true,
					'hintboxes' => [
						'Suppress' => 'Manual problem suppression. Date-time input accepts relative and absolute time format.',
						'Unsuppress' => 'Deactivates manual suppression.',
						'Acknowledge' => 'Confirms the problem is noticed (acknowledging user will be recorded). '.
								'Status change triggers action update operation.'
					]
				]
			],
			// Five problems.
			[
				[
					'problems' => ['Trigger for float', 'Trigger for char', 'Trigger for log', 'Trigger for unsigned', 'Trigger for text'],
					// If more than one problems selected - History label is absent.
					'labels' => ['Problem', 'Message', 'Scope', 'Change severity', 'Acknowledge', 'Unacknowledge', 'Close problem', ''],
					'hintboxes' => [
						'Suppress' => 'Manual problem suppression. Date-time input accepts relative and absolute time format.',
						'Unsuppress' => 'Deactivates manual suppression.',
						'Acknowledge' => 'Confirms the problem is noticed (acknowledging user will be recorded). '.
								'Status change triggers action update operation.',
						'Unacknowledge' => 'Undo problem acknowledgement.'
					],
					'close_enabled' => true,
					'unsuppress_enabled' => true,
					'Acknowledge' => true,
					'Unacknowledge' => true
				]
			]
		];
	}

	/**
	 * @dataProvider getLayoutData
	 */
	public function testFormUpdateProblem_Layout($data) {
		// Open filtered Problems list.
		$this->page->login()->open('zabbix.php?&action=problem.view&show_suppressed=1&hostids%5B%5D='.self::$hostid)->waitUntilReady();
		$table = $this->query('class:list-table')->asTable()->one();
		$table->findRows('Problem', $data['problems'])->select();
		$this->query('button:Mass update')->waitUntilClickable()->one()->click();

		$dialog = COverlayDialogElement::find()->one()->waitUntilReady();
		$this->assertEquals('Update problem', $dialog->getTitle());
		$form = $dialog->query('id:acknowledge_form')->asForm()->one();

		// Check form labels.
		$count = count($data['problems']);
		$default_labels = ['Problem', 'Message', 'History', 'Scope', 'Change severity', 'Acknowledge', 'Close problem', ''];
		$this->assertEquals(CTestArrayHelper::get($data, 'labels', $default_labels), $form->getLabels()->asText());

		// Check "Problem" field value.
		$problem = $count > 1 ? $count.' problems selected.' : $data['problems'][0];
		$this->assertTrue($form->query('xpath:.//div[@class="wordbreak" and text()='.
				CXPathHelper::escapeQuotes($problem).']')->exists()
		);

		// Check first label in Scope field.
		$scope_field = $form->getField('Scope');
		$scope_label_query = $count > 1
			? 'xpath:.//label[text()="Only selected problems"]/sup[text()='.CXPathHelper::escapeQuotes($count.' events').']'
			: 'xpath:.//label[text()="Only selected problem"]';
		$this->assertTrue($scope_field->query($scope_label_query)->exists());

		// Check second label in Scope field.
		$this->assertTrue($scope_field->query("xpath:.//label[text()=".
				"\"Selected and all other problems of related triggers\"]/sup[text()=".
				CXPathHelper::escapeQuotes($count > 1 ? $count.' events' : '1 event')."]")->exists()
		);

		// Check History field.
		if (array_key_exists('history', $data)) {
			$history = ($data['history'] === []) ? $data['history'] : [date('Y-m-d H:i:s', self::$acktime).$data['history'][0]];
			$history_table = $form->getField('History')->asTable();
			$this->assertEquals(['Time', 'User', 'User action', 'Message'], $history_table->getHeadersText());
			$this->assertEquals($history, $history_table->getRows()->asText());

			if ($data['problems'] === ['Trigger for unsigned']) {
				foreach (['Acknowledged', 'Message'] as $icon) {
					$this->assertTrue($history_table->query("xpath:.//span[@class=".CXPathHelper::fromClass('icon-action').
							" and @title=".CXPathHelper::escapeQuotes($icon)."]")->exists()
					);
				}
			}
		}

		// Check fields' default values and attributes.
		$fields = [
			'id:message' => ['value' => '', 'maxlength' => 2048, 'enabled' => true],
			'id:scope_0' => ['value' => true, 'enabled' => true],    // Only selected problem.
			'id:scope_1' => ['value' => false, 'enabled' => true],   // Selected and all other problems of related triggers.
			'id:change_severity' => ['value' => false, 'enabled' => true],
			'id:severity' => ['value' => 'Not classified', 'enabled' => false],
			'Close problem' => ['value' => false, 'enabled' => CTestArrayHelper::get($data, 'close_enabled', false)]
		];

		foreach ($fields as $field => $attributes) {
			$this->assertEquals($attributes['value'], $form->getField($field)->getValue());
			$this->assertTrue($form->getField($field)->isEnabled($attributes['enabled']));

			if (array_key_exists('maxlength', $attributes)) {
				$this->assertEquals($attributes['maxlength'], $form->getField($field)->getAttribute('maxlength'));
			}
		}

		// Check default values for 'Acknowledge' and  'Unacknowledge' fields.
		foreach (['Acknowledge', 'Unacknowledge'] as $label) {
			if (array_key_exists($label, $data)) {
				$field = $form->getField($label);
				$this->assertEquals(false, $field->getValue());
				$this->assertTrue($field->isEnabled());
			}
		}

		// Check other buttons in overlay.
		$button_queries = [
			'xpath:.//button[@title="Close"]' => true,
			'button:Update' => true,
			'button:Cancel' => true
		];

		foreach ($button_queries as $query => $clickable) {
			$this->assertEquals($clickable, $dialog->query($query)->one()->isClickable());
		}

		// Check asterisk text.
		$this->assertTrue($form->query('xpath:.//label[@class="form-label-asterisk" and '.
				'text()="At least one update operation or message must exist."]')->exists()
		);
		$dialog->close();
	}

	public function getFormData() {
		return [
			[
				[
					'problems' => ['Trigger for log', 'Trigger for char', 'Trigger for float'],
					'fields' => [
						'id:scope_1' => true,
						'id:change_severity' => true,
						'id:severity' => 'Information'
					],
					'db_check' => [
						[
							'name' => 'Trigger for log',
							'db_fields' => ['message' => '', 'action' => 8, 'new_severity' => 1]
						],
						[
							'name' => 'Trigger for char',
							'db_fields' => false
						],
						[
							'name' => 'Trigger for float',
							'db_fields' => ['message' => '', 'action' => 8, 'new_severity' => 1]
						]
					]
				]
			],
			[
				[
					'problems' => ['Trigger for float'],
					'fields' => [
						'id:message' => 'test message text',
						'id:change_severity' => true,
						'id:severity' => 'Warning',
						'Acknowledge' => true
					],
					'db_check' => [
						[
							'name' => 'Trigger for float',
							'db_fields' => ['message' => 'test message text', 'action' => 14, 'new_severity' => 2]
						]
					]
				]
			],
			[
				[
					'problems' => ['Trigger for text', 'Trigger for log'],
					'fields' => [
						'id:change_severity' => true,
						'id:severity' => 'Not classified'
					],
					'db_check' => [
						[
							'name' => 'Trigger for text',
							'db_fields' => ['message' => '', 'action' => 8, 'new_severity' => 0]
						],
						[
							'name' => 'Trigger for text',
							'db_fields' => ['message' => '', 'action' => 8, 'new_severity' => 0]
						]
					]
				]
			],
			[
				[
					'problems' => ['Trigger for char'],
					'fields' => [
						'id:change_severity' => true,
						'id:severity' => 'Average'
					],
					'db_check' => [
						[
							'name' => 'Trigger for char',
							'db_fields' => ['message' => '', 'action' => 8, 'new_severity' => 3]
						]
					]
				]
			],
			[
				[
					'problems' => ['Trigger for unsigned'],
					'fields' => [
						'id:change_severity' => true,
						'id:severity' => 'High'
					],
					'db_check' => [
						[
							'name' => 'Trigger for unsigned',
							'db_fields' => ['message' => '', 'action' => 8, 'new_severity' => 4]
						]
					]
				]
			],
			[
				[
					'problems' => ['Trigger for log'],
					'fields' => [
						'id:change_severity' => true,
						'id:severity' => 'Disaster'
					],
					'db_check' => [
						[
							'name' => 'Trigger for log',
							'db_fields' => ['message' => '', 'action' => 8, 'new_severity' => 5]
						]
					]
				]
			],
			[
				[
					'problems' => ['Trigger for log', 'Trigger for char', 'Trigger for float', 'Trigger for text', 'Trigger for unsigned'],
					'fields' => [
						'id:message' => 'Update all 5 problems',
						'id:change_severity' => true,
						'id:severity' => 'High'
					],
					'db_check' => [
						[
							'name' => 'Trigger for log',
							'db_fields' => ['message' => 'Update all 5 problems', 'action' => 12, 'new_severity' => 4]
						],
						[
							'name' => 'Trigger for char',
							'db_fields' => ['message' => 'Update all 5 problems', 'action' => 12, 'new_severity' => 4]
						],
						[
							'name' => 'Trigger for float',
							'db_fields' => ['message' => 'Update all 5 problems', 'action' => 12, 'new_severity' => 4]
						],
						[
							'name' => 'Trigger for text',
							'db_fields' => ['message' => 'Update all 5 problems', 'action' => 12, 'new_severity' => 4]
						],
						[
							'name' => 'Trigger for unsigned',
							'db_fields' => ['message' => 'Update all 5 problems', 'action' => 4, 'new_severity' => 0]
						]
					]
				]
			],
			[
				[
					'problems' => ['Trigger for unsigned'],
					'fields' => [
						'Unacknowledge' => true
					],
					'db_check' => [
						[
							'name' => 'Trigger for unsigned',
							'db_fields' => ['message' => '', 'action' => 16, 'new_severity' => 0]
						]
					]
				]
			],
			[
				[
					'problems' => ['Trigger for char'],
					'fields' => [
						'Close problem' => true
					],
					'db_check' => [
						[
							'name' => 'Trigger for char',
							'db_fields' => ['message' => '', 'action' => 1, 'new_severity' => 0]
						]
					]
				]
			]
		];
	}

	/**
	 * @dataProvider getFormData
	 */
	public function testFormUpdateProblem_Form($data) {
		// Open filtered Problems list.
		$this->page->login()->open('zabbix.php?&action=problem.view&filter_set=1&show_suppressed=1&hostids%5B%5D='.self::$hostid)->waitUntilReady();
		$table = $this->query('class:list-table')->asTable()->one();

		$count = count($data['problems']);
		$table->findRows('Problem', $data['problems']);

		if ($count > 1) {
			$table->findRows('Problem', $data['problems'])->select();
			$this->query('button:Mass update')->waitUntilClickable()->one()->click();
		}
		else {
			$table->findRow('Problem', $data['problems'][0])->getColumn('Ack')->query('tag:a')->waitUntilClickable()->one()->click();
		}

		$dialog = COverlayDialogElement::find()->one()->waitUntilReady();
		$form = $dialog->query('id:acknowledge_form')->asForm()->one();
		$form->fill($data['fields']);
		$form->submit();

		$dialog->ensureNotPresent();
		$this->page->waitUntilReady();
		$this->page->assertHeader('Problems');

		$message = ($count > 1) ? 'Events updated' : 'Event updated';
		$this->assertMessage(TEST_GOOD, $message);

		// Check db change.
		foreach ($data['db_check'] as $event) {
			$sql = CDBHelper::getRow('SELECT message, action, new_severity'.
					' FROM acknowledges'.
					' WHERE eventid=('.
						'SELECT eventid'.
						' FROM events'.
						' WHERE name='.zbx_dbstr($event['name']).
					') ORDER BY acknowledgeid DESC'
			);

			$this->assertEquals($event['db_fields'], $sql);
		}
	}

	public function getCancelData() {
		return [
			[
				[
					'case' => 'Cancel'
				]
			],
			[
				[
					'case' => 'Close'
				]
			]
		];
	}

	/**
	 * @dataProvider getCancelData
	 */
	public function testFormUpdateProblem_Cancel($data) {
		$old_hash = $this->getHash();

		// Open filtered Problems list.
		$this->page->login()->open('zabbix.php?&action=problem.view&show_suppressed=1&hostids%5B%5D='.self::$hostid)->waitUntilReady();
		$this->query('class:list-table')->asTable()->one()->findRow('Problem', 'Trigger for char')->getColumn('Ack')
				->query('tag:a')->waitUntilClickable()->one()->click();
		$dialog = COverlayDialogElement::find()->one()->waitUntilReady();
		$dialog->query('id:acknowledge_form')->asForm()->one()->fill([
				'id:scope_1' => true,
				'id:change_severity' => true,
				'id:severity' => 'Disaster',
				'Acknowledge' => true
		]);

		$dialog->query(($data['case'] === 'Close') ? 'xpath:.//button[@title="Close"]' : 'button:Cancel')->one()
				->waitUntilClickable()->click();

		$dialog->ensureNotPresent();
		$this->page->assertHeader('Problems');
		$this->assertEquals($old_hash, $this->getHash());
	}
}
