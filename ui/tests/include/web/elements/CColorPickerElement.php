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

require_once 'vendor/autoload.php';

require_once dirname(__FILE__).'/../CElement.php';

/**
 * Color picker element.
 */
class CColorPickerElement extends CElement {

	/**
	 * Get input field of color pick form.
	 *
	 * @return type
	 */
	public function getInput() {
		return $this->query('xpath:.//input')->one();
	}

	/**
	 * Overwrite input value.
	 *
	 * @inheritdoc
	 *
	 * @param string $color		color code
	 */
	public function overwrite($color) {
		$this->query('xpath:./button['.CXPathHelper::fromClass('color-picker-preview').']')->one()->click();
		$overlay = (new CElementQuery('id:color_picker'))->waitUntilVisible()->asOverlayDialog()->one();

		if ($color === null) {
			$overlay->query('button:Use default')->one()->click();
		}
		else {
			$overlay->query('xpath:.//div[@class="color-picker-input"]/input')->one()->overwrite($color);
		}

		$overlay->query('class:overlay-close-btn')->one()->click()->waitUntilNotVisible();

		return $this;
	}

	/**
	 * @inheritdoc
	 */
	public function isEnabled($enabled = true) {
		return $this->getInput()->isEnabled($enabled);
	}

	/**
	 * @inheritdoc
	 */
	public function getValue() {
		return $this->getInput()->getValue();
	}

	/**
	 * Alias for getValue.
	 * @see self::getValue
	 *
	 * @return string
	 */
	public function getText() {
		return $this->getValue();
	}

	/**
	 * Close color pick overlay dialog.
	 *
	 * @return $this
	 */
	public function close() {
		$this->query('class:overlay-close-btn')->one()->click()->waitUntilNotVisible();
	}

	/**
	 * Add/rewrite color code and submit it.
	 *
	 * @param string $color		color code
	 */
	public function fill($color) {
		$this->overwrite($color);
	}
}
