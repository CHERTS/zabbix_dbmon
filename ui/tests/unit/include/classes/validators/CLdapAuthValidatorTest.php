<?php declare(strict_types=1);
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

/**
 * @incomplete
 */
use PHPUnit\Framework\TestCase;

class CLdapAuthValidatorTest extends CValidatorTest {

	protected function setUp(): void {
		$this->markTestIncomplete('This test is not yet written');
	}

	public function dataProviderValidParam() {
		return [
			[[
				'conf' => []
			]]
		];
	}

	public function dataProviderValidValues() {
		return [[]];
	}

	public function dataProviderInvalidValues() {
		return [[]];
	}

	public function dataProviderInvalidValuesWithObjects() {
		return [[]];
	}

	protected function createValidator(array $params = []) {
		return new CLdapAuthValidator($params);
	}
}
