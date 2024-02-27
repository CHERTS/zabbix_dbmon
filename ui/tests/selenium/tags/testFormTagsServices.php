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

require_once dirname(__FILE__).'/../common/testFormTags.php';
require_once dirname(__FILE__).'/../../include/helpers/CDataHelper.php';

/**
 * @dataSource EntitiesTags
 *
 * @backup services
 */
class testFormTagsServices extends testFormTags {

	public $update_name = 'Service with tags for updating';
	public $clone_name = 'Service with tags for cloning';
	public $remove_name = 'Service for removing tags';
	public $link = 'zabbix.php?action=service.list.edit';
	public $saved_link = 'zabbix.php?action=host.edit&hostid=';

	/**
	 * Test creating of Service with tags.
	 *
	 * @dataProvider getCreateData
	 */
	public function testFormTagsServices_Create($data) {
		$this->checkTagsCreate($data, 'service');
	}

	/**
	 * Test update of Service with tags.
	 *
	 * @dataProvider getUpdateData
	 */
	public function testFormTagsServices_Update($data) {
		$this->checkTagsUpdate($data, 'service');
	}

	/**
	 * Test cloning of Service with tags.
	 */
	public function testFormTagsServices_Clone() {
		$this->executeCloning('service', 'Clone');
	}

	/**
	 * Test removing tags from Service.
	 */
	public function testFormTagsServices_RemoveTags() {
		$this->clearTags('service');
	}
}
