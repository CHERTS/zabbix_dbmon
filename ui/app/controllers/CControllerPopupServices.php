<?php declare(strict_types = 0);
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


class CControllerPopupServices extends CController {

	protected function init(): void {
		$this->disableSIDvalidation();
	}

	protected function checkInput(): bool {
		$fields = [
			'title' =>				'string|required',
			'filter_name' =>		'string',
			'exclude_serviceids' =>	'array_db services.serviceid',
			'multiple' =>			'in 0,1'
		];

		$ret = $this->validateInput($fields);

		if (!$ret) {
			$this->setResponse(
				(new CControllerResponseData([
					'main_block' => json_encode(['errors' => getMessages()->toString()])
				]))->disableView()
			);
		}

		return $ret;
	}

	protected function checkPermissions(): bool {
		return true;
	}

	/**
	 * @throws APIException
	 */
	protected function doAction(): void {
		$exclude_serviceids = $this->getInput('exclude_serviceids', []);

		$limit = CSettingsHelper::get(CSettingsHelper::SEARCH_LIMIT);

		$services = API::Service()->get([
			'output' => ['serviceid', 'name'],
			'selectTags' => ['tag', 'value'],
			'selectProblemTags' => ['tag', 'value'],
			'search' => ['name' => $this->hasInput('filter_name') ? $this->getInput('filter_name') : null],
			'limit' => $limit + count($exclude_serviceids),
			'preservekeys' => true
		]);

		$services = array_diff_key($services, array_flip($exclude_serviceids));
		$services = array_slice($services, 0, $limit);

		$tags = [];
		$problem_tags = [];

		foreach ($services as $service) {
			$tags[] = [
				'serviceid' => $service['serviceid'],
				'tags' => $service['tags']
			];

			$problem_tags[] = [
				'serviceid' => $service['serviceid'],
				'tags' => $service['problem_tags']
			];
		}

		$problem_tags_html = [];

		foreach (makeTags($problem_tags, true, 'serviceid') as $serviceid => $service_tags) {
			$problem_tags_html[$serviceid] = implode('', $service_tags);
		}

		$data = [
			'title' => $this->getInput('title'),
			'filter' => [
				'name' => $this->getInput('filter_name', '')
			],
			'exclude_serviceids' => $exclude_serviceids,
			'is_multiple' => $this->getInput('multiple', 1) == 1,
			'services' => $services,
			'tags' => makeTags($tags, true, 'serviceid'),
			'problem_tags' => makeTags($problem_tags, true, 'serviceid'),
			'problem_tags_html' => $problem_tags_html,
			'user' => [
				'debug_mode' => $this->getDebugMode()
			]
		];

		$this->setResponse(new CControllerResponseData($data));
	}
}
