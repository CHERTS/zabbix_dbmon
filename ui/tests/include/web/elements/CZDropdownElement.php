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

/**
 * Custom dropdown (z-select) element.
 */
class CZDropdownElement extends CElement {

	/**
	 * Get collection of options.
	 *
	 * @return CElementCollection
	 */
	public function getOptions() {
		return $this->query('xpath:.//li[not(@optgroup)]')->all();
	}

	/**
	 * Get text of selected element.
	 *
	 * @return string
	 */
	public function getText() {
		return $this->query('xpath:./button')->one()->getText();
	}

	/**
	 * Select option by text.
	 *
	 * @param string $text    option text to be selected
	 *
	 * @return $this
	 */
	public function select($text) {
		$xpath = 'xpath:.//li[not(@optgroup) and text()='.CXPathHelper::escapeQuotes($text).']';

		if ($text === $this->getText()) {
			return $this;
		}

		for ($i = 0; $i < 5; $i++) {
			try {
				$this->waitUntilClickable()->click();
				$this->query($xpath)->one()->click();

				return $this;
			}
			catch (Exception $exception) {
				// Code is not missing here.
			}
		}

		throw new Exception('Failed to select dropdown option "'.$text.'".');
	}

	/**
	 * Alias for select.
	 * @see self::select
	 *
	 * @param string $text    option text to be selected
	 *
	 * @return $this
	 */
	public function fill($text) {
		return $this->select($text);
	}

	/**
	 * Alias for getText.
	 * @see self::getText
	 *
	 * @return string
	 */
	public function getValue() {
		return $this->getText();
	}
}
