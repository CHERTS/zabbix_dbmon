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

require_once dirname(__FILE__).'/common/testFormTags.php';

/**
 * @backup hosts
 */
class testFormTagsHost extends testFormTags {

	public $update_name = 'Host with tags for updating';
	public $clone_name = 'Host with tags for cloning';
	public $link = 'hosts.php';
	public $saved_link = 'hosts.php?form=update&hostid=';

	/**
	 * Test creating of Host with tags.
	 *
	 * @dataProvider getCreateData
	 */
	public function testFormTagsHost_Create($data) {
		$this->checkTagsCreate($data, 'host');
	}

	/**
	 * Test update of Host with tags.
	 *
	 * @dataProvider getUpdateData
	 */
	public function testFormTagsHost_Update($data) {
		$this->checkTagsUpdate($data, 'host');
	}

	/**
	 * Test cloning of Host with tags.
	 */
	public function testFormTagsHost_Clone() {
		$this->executeCloning('host', 'Clone');
	}

	/**
	 * Test full cloning of Host with tags.
	 */
	public function testFormTagsHost_FullClone() {
		$this->executeCloning('host', 'Full clone');
	}
}
