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


/**
 * @var CView $this
 */

$widget = (new CWidget())
	->setTitle(_('Item prototypes'))
	->addItem(get_header_host_table('items', $data['hostid'], $data['parent_discoveryid']));

$form = (new CForm())
	->setName('item_prototype_form')
	->setAttribute('aria-labeledby', ZBX_STYLE_PAGE_TITLE)
	->addVar('group_itemid', $data['item_prototypeids'])
	->addVar('hostid', $data['hostid'])
	->addVar('parent_discoveryid', $data['parent_discoveryid'])
	->addVar('action', $data['action']);

$item_form_list = (new CFormList('item-form-list'))
	->addRow(
		(new CVisibilityBox('visible[type]', 'type', _('Original')))
			->setLabel(_('Type'))
			->setChecked(array_key_exists('type', $data['visible']))
			->setAttribute('autofocus', 'autofocus'),
		(new CSelect('type'))
			->setId('type')
			->setValue($data['type'])
			->addOptions(CSelect::createOptionsFromArray($data['itemTypes']))
	);

// Add interfaces if mass updating item prototypes on host level.
if ($data['display_interfaces']) {
	$item_form_list->addRow(
		(new CVisibilityBox('visible[interfaceid]', 'interfaceDiv', _('Original')))
			->setLabel(_('Host interface'))
			->setChecked(array_key_exists('interfaceid', $data['visible']))
			->setAttribute('data-multiple-interface-types', $data['multiple_interface_types']),
		(new CDiv([
			getInterfaceSelect($data['hosts']['interfaces'])
				->setId('interface-select')
				->setValue($data['interfaceid'])
				->addClass(ZBX_STYLE_ZSELECT_HOST_INTERFACE),
			(new CSpan(_('No interface found')))
				->addClass(ZBX_STYLE_RED)
				->setId('interface_not_defined')
				->addStyle('display: none;')
		]))->setId('interfaceDiv'),
		'interface_row'
	);
	$form->addVar('selectedInterfaceId', $data['interfaceid']);
}

$item_form_list
	// Append JMX endpoint to form list.
	->addRow(
		(new CVisibilityBox('visible[jmx_endpoint]', 'jmx_endpoint', _('Original')))
			->setLabel(_('JMX endpoint'))
			->setChecked(array_key_exists('jmx_endpoint', $data['visible'])),
		(new CTextBox('jmx_endpoint', $data['jmx_endpoint']))->setWidth(ZBX_TEXTAREA_STANDARD_WIDTH)
	)
	// Append "URL" field required by item prototype type "ITEM_TYPE_HTTPAGENT".
	->addRow(
		(new CVisibilityBox('visible[url]', 'url', _('Original')))
			->setLabel(_('URL'))
			->setChecked(array_key_exists('url', $data['visible'])),
		(new CTextBox('url', $data['url'], false, DB::getFieldLength('items', 'url')))
			->setWidth(ZBX_TEXTAREA_STANDARD_WIDTH)
	)
	// Append "Request body type" field (optional) for item prototype type "ITEM_TYPE_HTTPAGENT".
	->addRow(
		(new CVisibilityBox('visible[post_type]', 'post_type_container', _('Original')))
			->setLabel(_('Request body type'))
			->setChecked(array_key_exists('post_type', $data['visible'])),
		(new CDiv(
			(new CRadioButtonList('post_type', (int) $data['post_type']))
				->addValue(_('Raw data'), ZBX_POSTTYPE_RAW)
				->addValue(_('JSON data'), ZBX_POSTTYPE_JSON)
				->addValue(_('XML data'), ZBX_POSTTYPE_XML)
				->setModern(true)
		))->setId('post_type_container')
	)
	// Append "Request body" field (optional) for item prototype type "ITEM_TYPE_HTTPAGENT".
	->addRow(
		(new CVisibilityBox('visible[posts]', 'posts', _('Original')))
			->setLabel(_('Request body'))
			->setChecked(array_key_exists('posts', $data['visible'])),
		(new CTextArea('posts', $data['posts']))->setWidth(ZBX_TEXTAREA_STANDARD_WIDTH)
	);

// Append "Headers" field (optional) for item prototype type "ITEM_TYPE_HTTPAGENT".
$headers_data = [];

if (is_array($data['headers']) && $data['headers']) {
	foreach ($data['headers'] as $pair) {
		$headers_data[] = ['name' => key($pair), 'value' => reset($pair)];
	}
}
else {
	$headers_data[] = ['name' => '', 'value' => ''];
}
$headers = (new CTag('script', true))->setAttribute('type', 'text/json');
$headers->items = [json_encode($headers_data)];

$item_form_list
	->addRow(
		(new CVisibilityBox('visible[headers]', 'headers_pairs', _('Original')))
			->setLabel(_('Headers'))
			->setChecked(array_key_exists('headers', $data['visible'])),
		(new CDiv([
			(new CTable())
				->addStyle('width: 100%;')
				->setHeader(['', _('Name'), '', _('Value'), ''])
				->addRow((new CRow)->setAttribute('data-insert-point', 'append'))
				->setFooter(new CRow(
					(new CCol(
						(new CButton(null, _('Add')))
							->addClass(ZBX_STYLE_BTN_LINK)
							->setAttribute('data-row-action', 'add_row')
					))->setColSpan(5)
				)),
			(new CTag('script', true))
				->setAttribute('type', 'text/x-jquery-tmpl')
				->addItem(new CRow([
					(new CCol((new CDiv)->addClass(ZBX_STYLE_DRAG_ICON)))->addClass(ZBX_STYLE_TD_DRAG_ICON),
					(new CTextBox('headers[name][#{index}]', '#{name}'))->setWidth(ZBX_TEXTAREA_HTTP_PAIR_NAME_WIDTH),
					'&rArr;',
					(new CTextBox('headers[value][#{index}]', '#{value}', false, 2000))
						->setWidth(ZBX_TEXTAREA_HTTP_PAIR_VALUE_WIDTH),
					(new CButton(null, _('Remove')))
						->addClass(ZBX_STYLE_BTN_LINK)
						->setAttribute('data-row-action', 'remove_row')
				])),
			$headers
		]))
			->addClass(ZBX_STYLE_TABLE_FORMS_SEPARATOR)
			->setId('headers_pairs')
			->setAttribute('data-sortable-pairs-table', '1')
			->addStyle('min-width: '.ZBX_TEXTAREA_BIG_WIDTH.'px;')
	)
	// Append value type to form list.
	->addRow(
		(new CVisibilityBox('visible[value_type]', 'value_type', _('Original')))
			->setLabel(_('Type of information'))
			->setChecked(array_key_exists('value_type', $data['visible'])),
		(new CSelect('value_type'))
			->setId('value_type')
			->setValue($data['value_type'])
			->addOptions(CSelect::createOptionsFromArray([
				ITEM_VALUE_TYPE_UINT64 => _('Numeric (unsigned)'),
				ITEM_VALUE_TYPE_FLOAT => _('Numeric (float)'),
				ITEM_VALUE_TYPE_STR => _('Character'),
				ITEM_VALUE_TYPE_LOG => _('Log'),
				ITEM_VALUE_TYPE_TEXT => _('Text')
			]))
	)
	// Append units to form list.
	->addRow(
		(new CVisibilityBox('visible[units]', 'units', _('Original')))
			->setLabel(_('Units'))
			->setChecked(array_key_exists('units', $data['visible'])),
		(new CTextBox('units', $data['units']))->setWidth(ZBX_TEXTAREA_STANDARD_WIDTH)
	)
	// Append authtype to form list.
	->addRow(
		(new CVisibilityBox('visible[authtype]', 'authtype', _('Original')))
			->setLabel(_('Authentication method'))
			->setChecked(array_key_exists('authtype', $data['visible'])),
		(new CSelect('authtype'))
			->setId('authtype')
			->setValue($data['authtype'])
			->addOptions(CSelect::createOptionsFromArray([
				ITEM_AUTHTYPE_PASSWORD => _('Password'),
				ITEM_AUTHTYPE_PUBLICKEY => _('Public key')
			]))
	)
	// Append username to form list.
	->addRow(
		(new CVisibilityBox('visible[username]', 'username', _('Original')))
			->setLabel(_('User name'))
			->setChecked(array_key_exists('username', $data['visible'])),
		(new CTextBox('username', $data['username']))
			->setWidth(ZBX_TEXTAREA_SMALL_WIDTH)
			->disableAutocomplete()
	)
	// Append publickey to form list.
	->addRow(
		(new CVisibilityBox('visible[publickey]', 'publickey', _('Original')))
			->setLabel(_('Public key file'))
			->setChecked(array_key_exists('publickey', $data['visible'])),
		(new CTextBox('publickey', $data['publickey']))->setWidth(ZBX_TEXTAREA_SMALL_WIDTH)
	)
	// Append privatekey to form list.
	->addRow(
		(new CVisibilityBox('visible[privatekey]', 'privatekey', _('Original')))
			->setLabel(_('Private key file'))
			->setChecked(array_key_exists('privatekey', $data['visible'])),
		(new CTextBox('privatekey', $data['privatekey']))->setWidth(ZBX_TEXTAREA_SMALL_WIDTH)
	)
	// Append password to form list.
	->addRow(
		(new CVisibilityBox('visible[password]', 'password', _('Original')))
			->setLabel(_('Password'))
			->setChecked(array_key_exists('password', $data['visible'])),
		(new CTextBox('password', $data['password']))
			->setWidth(ZBX_TEXTAREA_SMALL_WIDTH)
			->disableAutocomplete()
	);

// Create preprocessing form list.
$preprocessing_form_list = (new CFormList('preprocessing-form-list'))
	// Append item pre-processing to form list.
	->addRow(
		(new CVisibilityBox('visible[preprocessing]', 'preprocessing_div', _('Original')))
			->setLabel(_('Preprocessing steps'))
			->setChecked(array_key_exists('preprocessing', $data['visible'])),
		(new CDiv(getItemPreprocessing($form, $data['preprocessing'], false, $data['preprocessing_types'])))
			->addClass(ZBX_STYLE_TABLE_FORMS_SEPARATOR)
			->addStyle('min-width: '.ZBX_TEXTAREA_STANDARD_WIDTH.'px;')
			->setId('preprocessing_div')
	);

// Prepare Update interval for form list.
$update_interval = (new CTable())
	->setId('update_interval')
	->addRow([_('Delay'),
		(new CDiv((new CTextBox('delay', $data['delay']))->setWidth(ZBX_TEXTAREA_SMALL_WIDTH)))
	]);

$custom_intervals = (new CTable())
	->setId('custom_intervals')
	->setHeader([
		new CColHeader(_('Type')),
		new CColHeader(_('Interval')),
		new CColHeader(_('Period')),
		(new CColHeader(_('Action')))->setWidth(50)
	])
	->addClass(ZBX_STYLE_TABLE_FORMS_SEPARATOR)
	->addStyle('min-width: '.ZBX_TEXTAREA_STANDARD_WIDTH.'px;');

foreach ($data['delay_flex'] as $i => $delay_flex) {
	$type_input = (new CRadioButtonList('delay_flex['.$i.'][type]', (int) $delay_flex['type']))
		->addValue(_('Flexible'), ITEM_DELAY_FLEXIBLE)
		->addValue(_('Scheduling'), ITEM_DELAY_SCHEDULING)
		->setModern(true);

	if ($delay_flex['type'] == ITEM_DELAY_FLEXIBLE) {
		$delay_input = (new CTextBox('delay_flex['.$i.'][delay]', $delay_flex['delay']))
			->setAttribute('placeholder', ZBX_ITEM_FLEXIBLE_DELAY_DEFAULT);
		$period_input = (new CTextBox('delay_flex['.$i.'][period]', $delay_flex['period']))
			->setAttribute('placeholder', ZBX_DEFAULT_INTERVAL);
		$schedule_input = (new CTextBox('delay_flex['.$i.'][schedule]'))
			->setAttribute('placeholder', ZBX_ITEM_SCHEDULING_DEFAULT)
			->addStyle('display: none;');
	}
	else {
		$delay_input = (new CTextBox('delay_flex['.$i.'][delay]'))
			->setAttribute('placeholder', ZBX_ITEM_FLEXIBLE_DELAY_DEFAULT)
			->addStyle('display: none;');
		$period_input = (new CTextBox('delay_flex['.$i.'][period]'))
			->setAttribute('placeholder', ZBX_DEFAULT_INTERVAL)
			->addStyle('display: none;');
		$schedule_input = (new CTextBox('delay_flex['.$i.'][schedule]', $delay_flex['schedule']))
			->setAttribute('placeholder', ZBX_ITEM_SCHEDULING_DEFAULT);
	}

	$button = (new CButton('delay_flex['.$i.'][remove]', _('Remove')))
		->addClass(ZBX_STYLE_BTN_LINK)
		->addClass('element-table-remove');

	$custom_intervals->addRow([$type_input, [$delay_input, $schedule_input], $period_input, $button], 'form_row');
}

$custom_intervals->addRow([(new CButton('interval_add', _('Add')))
	->addClass(ZBX_STYLE_BTN_LINK)
	->addClass('element-table-add')]);

$update_interval->addRow(
	(new CRow([
		(new CCol(_('Custom intervals')))->addStyle('vertical-align: top;'),
		new CCol($custom_intervals)
	]))
);

// Append update interval to form list.
$item_form_list
	->addRow(
		(new CVisibilityBox('visible[delay]', 'update_interval_div', _('Original')))
			->setLabel(_('Update interval'))
			->setChecked(array_key_exists('delay', $data['visible'])),
		(new CDiv($update_interval))->setId('update_interval_div')
	)
	// Append history to form list.
	->addRow(
		(new CVisibilityBox('visible[history]', 'history_div', _('Original')))
			->setLabel(_('History storage period'))
			->setChecked(array_key_exists('history', $data['visible'])),
		(new CDiv([
			(new CRadioButtonList('history_mode', (int) $data['history_mode']))
				->addValue(_('Do not keep history'), ITEM_STORAGE_OFF)
				->addValue(_('Storage period'), ITEM_STORAGE_CUSTOM)
				->setModern(true),
			(new CDiv())->addClass(ZBX_STYLE_FORM_INPUT_MARGIN),
			(new CTextBox('history', $data['history']))
				->setWidth(ZBX_TEXTAREA_TINY_WIDTH)
				->setAriaRequired()
		]))
			->addClass('wrap-multiple-controls')
			->setId('history_div')
	)
	// Append trends to form list.
	->addRow(
		(new CVisibilityBox('visible[trends]', 'trends_div', _('Original')))
			->setLabel(_('Trend storage period'))
			->setChecked(array_key_exists('trends', $data['visible'])),
		(new CDiv([
			(new CRadioButtonList('trends_mode', (int) $data['trends_mode']))
				->addValue(_('Do not keep trends'), ITEM_STORAGE_OFF)
				->addValue(_('Storage period'), ITEM_STORAGE_CUSTOM)
				->setModern(true),
			(new CDiv())->addClass(ZBX_STYLE_FORM_INPUT_MARGIN),
			(new CTextBox('trends', $data['trends']))
				->setWidth(ZBX_TEXTAREA_TINY_WIDTH)
				->setAriaRequired()
		]))
			->addClass('wrap-multiple-controls')
			->setId('trends_div')
	);

$item_form_list
	// Append status (create enabled) to form list.
	->addRow(
		(new CVisibilityBox('visible[status]', 'status', _('Original')))
			->setLabel(_('Create enabled'))
			->setChecked(array_key_exists('status', $data['visible'])),
		(new CSelect('status'))
			->setId('status')
			->setValue($data['status'])
			->addOption(new CSelectOption(ITEM_STATUS_ACTIVE, item_status2str(ITEM_STATUS_ACTIVE)))
			->addOption(new CSelectOption(ITEM_STATUS_DISABLED, item_status2str(ITEM_STATUS_DISABLED)))
	)
	->addRow(
		(new CVisibilityBox('visible[discover]', 'discover', _('Original')))
			->setLabel(_('Discover'))
			->setChecked(array_key_exists('discover', $data['visible'])),
		(new CRadioButtonList('discover', (int) $data['discover']))
			->addValue(_('Yes'), ZBX_PROTOTYPE_DISCOVER)
			->addValue(_('No'), ZBX_PROTOTYPE_NO_DISCOVER)
			->setModern(true)
	)
	// Append logtime to form list.
	->addRow(
		(new CVisibilityBox('visible[logtimefmt]', 'logtimefmt', _('Original')))
			->setLabel(_('Log time format'))
			->setChecked(array_key_exists('logtimefmt', $data['visible'])),
		(new CTextBox('logtimefmt', $data['logtimefmt']))->setWidth(ZBX_TEXTAREA_STANDARD_WIDTH)
	);

// append valuemap to form list
$valuemaps_select = (new CSelect('valuemapid'))
	->setId('valuemapid')
	->setValue($data['valuemapid'])
	->addOption(new CSelectOption(0, _('As is')));

foreach ($data['valuemaps'] as $valuemap) {
	$valuemaps_select->addOption(new CSelectOption($valuemap['valuemapid'], $valuemap['name']));
}

$item_form_list
	->addRow(
		(new CVisibilityBox('visible[valuemapid]', 'valuemap', _('Original')))
			->setLabel(_('Show value'))
			->setChecked(array_key_exists('valuemapid', $data['visible'])),
		(new CDiv([$valuemaps_select, ' ',
			(new CLink(_('show value mappings'), (new CUrl('zabbix.php'))
				->setArgument('action', 'valuemap.list')
				->getUrl()
			))->setAttribute('target', '_blank')
		]))->setId('valuemap')
	)
	->addRow(
		(new CVisibilityBox('visible[allow_traps]', 'allow_traps', _('Original')))
			->setLabel(_('Enable trapping'))
			->setChecked(array_key_exists('allow_traps', $data['visible'])),
		(new CCheckBox('allow_traps', HTTPCHECK_ALLOW_TRAPS_ON))
			->setChecked($data['allow_traps'] == HTTPCHECK_ALLOW_TRAPS_ON)
	)
	->addRow(
		(new CVisibilityBox('visible[trapper_hosts]', 'trapper_hosts', _('Original')))
			->setLabel(_('Allowed hosts'))
			->setChecked(array_key_exists('trapper_hosts', $data['visible'])),
		(new CTextBox('trapper_hosts', $data['trapper_hosts']))->setWidth(ZBX_TEXTAREA_STANDARD_WIDTH)
	)
	->addRow(
		(new CVisibilityBox('visible[applications]', 'applications_div', _('Original')))
			->setLabel(_('Applications'))
			->setChecked(array_key_exists('applications', $data['visible'])),
		(new CDiv([
			(new CRadioButtonList('massupdate_app_action', (int) $data['massupdate_app_action']))
				->addValue(_('Add'), ZBX_ACTION_ADD)
				->addValue(_('Replace'), ZBX_ACTION_REPLACE)
				->addValue(_('Remove'), ZBX_ACTION_REMOVE)
				->setModern(true),
			(new CMultiSelect([
				'name' => 'applications[]',
				'object_name' => 'applications',
				'add_new' => !($data['massupdate_app_action'] == ZBX_ACTION_REMOVE),
				'data' => $data['applications'],
				'popup' => [
					'parameters' => [
						'srctbl' => 'applications',
						'srcfld1' => 'applicationid',
						'dstfrm' => $form->getName(),
						'dstfld1' => 'applications_',
						'hostid' => $data['hostid'],
						'noempty' => true
					]
				]
			]))->setWidth(ZBX_TEXTAREA_STANDARD_WIDTH)
		]))
			->addClass(ZBX_STYLE_TABLE_FORMS_SEPARATOR)
			->setId('applications_div')
);

// Append master item select.
$master_itemname = ($data['master_itemid'] != 0)
	? $data['master_hostname'].NAME_DELIMITER.$data['master_itemname']
	: '';

$master_item = [
	(new CTextBox('master_itemname', $master_itemname, true))
		->setWidth(ZBX_TEXTAREA_STANDARD_WIDTH)
		->setAriaRequired(),
	(new CVar('master_itemid', $data['master_itemid'], 'master_itemid'))
];

$master_item[] = (new CDiv())->addClass(ZBX_STYLE_FORM_INPUT_MARGIN);
$master_item[] = (new CButton('button', _('Select')))
	->addClass(ZBX_STYLE_BTN_GREY)
	->onClick('return PopUp("popup.generic",'.
		json_encode([
			'srctbl' => 'items',
			'srcfld1' => 'itemid',
			'srcfld2' => 'name',
			'dstfrm' => $form->getName(),
			'dstfld1' => 'master_itemid',
			'dstfld2' => 'master_itemname',
			'only_hostid' => $data['hostid'],
			'with_webitems' => 1,
			'normal_only' => 1
		]).', null, this);'
	);
$master_item[] = (new CDiv())->addClass(ZBX_STYLE_FORM_INPUT_MARGIN);
$master_item[] = (new CButton('button', _('Select prototype')))
	->addClass(ZBX_STYLE_BTN_GREY)
	->onClick('return PopUp("popup.generic",'.
		json_encode([
			'srctbl' => 'item_prototypes',
			'srcfld1' => 'itemid',
			'srcfld2' => 'name',
			'dstfrm' => $form->getName(),
			'dstfld1' => 'master_itemid',
			'dstfld2' => 'master_itemname',
			'parent_discoveryid' => $data['parent_discoveryid']
		]).', null, this);'
	);

$item_form_list
	->addRow(
		(new CVisibilityBox('visible[applicationPrototypes]', 'application_prototypes_div', _('Original')))
			->setLabel(_('Application prototypes'))
			->setChecked(array_key_exists('applicationPrototypes', $data['visible'])),
		(new CDiv([
			(new CRadioButtonList('massupdate_app_prot_action', (int) $data['massupdate_app_prot_action']))
				->addValue(_('Add'), ZBX_ACTION_ADD)
				->addValue(_('Replace'), ZBX_ACTION_REPLACE)
				->addValue(_('Remove'), ZBX_ACTION_REMOVE)
				->setModern(true),
			(new CMultiSelect([
				'name' => 'application_prototypes[]',
				'object_name' => 'application_prototypes',
				'add_new' => !($data['massupdate_app_prot_action'] == ZBX_ACTION_REMOVE),
				'data' => $data['application_prototypes'],
				'popup' => [
					'parameters' => [
						'srctbl' => 'application_prototypes',
						'srcfld1' => 'application_prototypeid',
						'dstfrm' => $form->getName(),
						'dstfld1' => 'application_prototypes_',
						'hostid' => $data['hostid'],
						'parent_discoveryid' => $data['parent_discoveryid'],
						'noempty' => true
					]
				]
			]))->setWidth(ZBX_TEXTAREA_STANDARD_WIDTH)
		]))
			->addClass(ZBX_STYLE_TABLE_FORMS_SEPARATOR)
			->setId('application_prototypes_div')
	)
	// Append master item select.
	->addRow(
		(new CVisibilityBox('visible[master_itemid]', 'master_item', _('Original')))
			->setLabel(_('Master item'))
			->setChecked(array_key_exists('master_itemid', $data['visible'])),
		(new CDiv([
			(new CVar('master_itemname')),
			$master_item
		]))->setId('master_item')
	)
	// Append description to form list.
	->addRow(
		(new CVisibilityBox('visible[description]', 'description', _('Original')))
			->setLabel(_('Description'))
			->setChecked(array_key_exists('description', $data['visible'])),
		(new CTextArea('description', $data['description']))
			->setWidth(ZBX_TEXTAREA_STANDARD_WIDTH)
			->setMaxlength(DB::getFieldLength('items', 'description'))
	);

$tabs = (new CTabView())
	->addTab('item_tab', _('Item prototype'), $item_form_list)
	->addTab('preprocessing_tab', _('Preprocessing'), $preprocessing_form_list)
	// Append buttons to form.
	->setFooter(makeFormFooter(
		new CSubmit('massupdate', _('Update')),
		[new CButtonCancel(url_params(['hostid', 'parent_discoveryid']))]
	));

if (!hasRequest('massupdate')) {
	$tabs->setSelected(0);
}

$form->addItem($tabs);

$widget->addItem($form);

$interface_ids_by_types = [];
if ($data['display_interfaces']) {
	foreach ($data['hosts']['interfaces'] as $interface) {
		$interface_ids_by_types[$interface['type']][] = $interface['interfaceid'];
	}
}

require_once dirname(__FILE__).'/js/configuration.item.massupdate.js.php';

$widget->show();
