<?php
/*
** Zabbix
** Copyright (C) 2001-2020 Zabbix SIA
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
 * Global popup menu element.
 */
class CPopupMenuElement extends CElement {

	/**
	 * @inheritdoc
	 */
	public static function find() {
		return (new CElementQuery('xpath://ul[contains(@class, "menu-popup-top")]'))->asPopupMenu();
	}

	/**
	 * Get collection of popup menu titles.
	 *
	 * @return array
	 */
	public function getTitles() {
		return $this->query('xpath:.//h3')->all();
	}

	/**
	 * Check if titles exists.
	 *
	 * @param string|array $titles    titles to be searched for
	 *
	 * @return boolean
	 */
	public function hasTitles($titles) {
		if (!is_array($titles)) {
			$titles = [$titles];
		}

		return count(array_diff($titles, $this->getTitles()->asText())) === 0;
	}

	/**
	 * Get collection of menu items.
	 *
	 * @return CElementCollection
	 */
	public function getItems() {
		return $this->query('xpath:./li/a')->all();
	}

	/**
	 * Check if items exists.
	 *
	 * @param string|array $items    items to be searched for
	 *
	 * @return boolean
	 */
	public function hasItems($items) {
		if (!is_array($items)) {
			$items = [$items];
		}

		return count(array_diff($this->getItems()->asText(), $items)) === 0;
	}


	/**
	 * Select item from popup menu.
	 *
	 * @param string|array $items    text of menu item(s)
	 *
	 * @return $this
	 */
	public function select($items) {
		if (!is_array($items)) {
			$items = [$items];
		}

		$name = array_shift($items);
		$element = $this->query('xpath', './li/a[text()='.CXPathHelper::escapeQuotes($name).']')->one(false);
		if (!$element->isValid()) {
			throw new Exception('Failed to find menu item by name: "'.$name.'".');
		}

		if ($items) {
			$element->hover();
			$element->parents()->query('class:menu-popup')->asPopupMenu()
					->waitUntilPresent()->one()->select($items);
		}
		else {
			$element->click();
		}

		return $this;
	}

	/**
	 * Alias for select.
	 * @see self::select
	 *
	 * @param string $items    items text to be selected
	 *
	 * @return $this
	 */
	public function fill($items) {
		return $this->select($items);
	}
}
