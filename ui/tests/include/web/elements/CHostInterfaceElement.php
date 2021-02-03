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

require_once 'vendor/autoload.php';

require_once dirname(__FILE__).'/../CElement.php';

use \Facebook\WebDriver\Remote\RemoteWebElement;
use \Facebook\WebDriver\Exception\UnrecognizedExceptionException;

/**
 * Host interface element.
 */
class CHostInterfaceElement extends CMultifieldTableElement {

	/**
	 * @inheritdoc
	 */
	public static function createInstance(RemoteWebElement $element, $options = []) {
		$instance = parent::createInstance($element, array_merge($options, ['normalized' => true]));

		$header = 'xpath:.//div['.CXPathHelper::fromClass('interface-row-header').']/div['.CXPathHelper::fromClass('interface-cell').']';
		$row = 'xpath:(.//div[('.CXPathHelper::fromClass('interface-row').') and not(contains(@class, "interface-row-header"))])';
		$column = 'xpath:.//div['.CXPathHelper::fromClass('interface-cell').']';
		$instance->selectors = [
			'header' => $header,
			'row' => $row,
			'column' => $column
		];

		return $instance;
	}

	/**
	 * Add new row.
	 *
	 * @param array $values    row values
	 *
	 * return $this
	 */
	public function addRow($values) {
		$rows = $this->getRows()->count();

		// Count amount of rows in each interface block.
		$interfaces = ['agent', 'SNMP', 'JMX', 'IPMI'];
		$interface_rows = [];
		foreach ($interfaces as $interface) {
			$container = $this->query('id', $interface.'Interfaces')->one();
			$interface_rows[] = $container->query($this->selectors['row'])->all()->count();
		}
		$interface_rows = array_combine($interfaces, $interface_rows);

		// Set the index of row to update.
		$index = 0;
		switch ($values['type']) {
			case 'JMX':
				$index += $interface_rows['JMX'];
			case 'SNMP':
				$index += $interface_rows['SNMP'];
			case 'Agent':
				$index += $interface_rows['agent'];
				break;
			case 'IPMI':
				$index = $rows;
				break;
		}

		$this->query('button:Add')->one()->click();
		CPopupMenuElement::find()->waitUntilVisible()->one()->fill($values['type']);
		// Wait until new table row appears.
		$this->query('xpath:'.CXPathHelper::fromSelector($this->selectors['row']).'['.($rows + 1).']')->waitUntilPresent();
		unset($values['type']);
		return $this->updateRow($index, $values);
	}

	/**
	 * Update row by index.
	 *
	 * @param integer $index     row index
	 * @param array   $values    row values
	 *
	 * @throws Exception    if not all fields could be found within a row
	 *
	 * return $this
	 */
	public function updateRow($index, $values) {
		$row = $this->getRow($index);
		$controls = $this->getRowControls($row);
		foreach ($values as $name => $value) {
			if (array_key_exists($name, $controls)) {
				try {
					$controls[$name]->fill($value);
				}
				catch (UnrecognizedExceptionException $e1) {
					try {
						$controls = $this->getRowControls($this->getRow($index));
						$controls[$name]->fill($value);
					}
					catch (\Exception $e2) {
						throw $e1;
					}
				}
				unset($values[$name]);
			}
			else {
				$xpath = 'xpath:.//div['.CXPathHelper::fromClass('list-accordion-item-body').']';
				$form = $row->query($xpath)->asForm(['normalized' => true])->one(false);
				if ($form->isValid()) {
					$fields = $form->getFields();

					if ($fields->exists($name)) {
						$fields->get($name)->fill($value);
						unset($values[$name]);
					}
				}
			}
		}

		if ($values) {
			throw new Exception('Failed to set values for fields ['.implode(', ', array_keys($values)).'] when filling'.
					' multifield row (controls are not present for those fields).'
			);
		}

		return $this;
	}

	/**
	 * Get controls from row.
	 *
	 * @param CTableRowElement $row        table row
	 * @param array            $headers    table headers
	 *
	 * @return array
	 */
	protected function getRowControls($row, $headers = null) {
		$controls = parent::getRowControls($row, $headers);

		$xpath = 'xpath:.//div['.CXPathHelper::fromClass('list-accordion-item-body').']';
		$form = $row->query($xpath)->asForm(['normalized' => true])->one(false);

		if ($form->isValid()) {
			// Expand row for SNMP interface.
			$button = $row->getColumn(0)->query('tag:button')->one();
			if (in_array($button->getAttribute('title'), ['', 'Expand'])) {
				$button->click();
			}

			foreach ($form->getFields()->filter(new CElementFilter(CElementFilter::VISIBLE)) as $label => $field) {
				$controls[$label] = $field;
			}
		}

		return $controls;
	}
}
