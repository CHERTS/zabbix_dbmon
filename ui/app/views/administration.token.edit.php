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


/**
 * @var CView $this
 */

$this->addJsFile('class.calendar.js');

$this->includeJsFile('administration.token.edit.js.php');

$widget = (new CWidget())
	->setTitle(_('API tokens'))
	->setTitleSubmenu(getAdministrationGeneralSubmenu());

$token_form = (new CForm())
	->setId('token_form')
	->setName('token')
	->setAttribute('aria-labelledby', ZBX_STYLE_PAGE_TITLE)
	->addVar('action_src', 'token.edit')
	->addVar('action_dst', ($data['tokenid'] == 0) ? 'token.view' : 'token.list')
	->addVar('action', ($data['tokenid'] == 0) ? 'token.create' : 'token.update')
	->addVar('tokenid', $data['tokenid']);

// Enable form submitting on Enter.
$token_form->addItem((new CSubmitButton(null))->addClass(ZBX_STYLE_FORM_SUBMIT_HIDDEN));

$token_from_list = (new CFormList())
	->addRow((new CLabel(_('Name'), 'name'))->setAsteriskMark(),
		(new CTextBox('name', $data['name'], false, DB::getFieldLength('token', 'name')))
			->setWidth(ZBX_TEXTAREA_STANDARD_WIDTH)
			->setAttribute('autofocus', 'autofocus')
			->setAriaRequired()
	)
	->addRow((new CLabel(_('User'), 'userid_ms'))->setAsteriskMark(), [
		(new CMultiSelect([
			'disabled' => ($data['tokenid'] != 0),
			'multiple' => false,
			'name' => 'userid',
			'object_name' => 'users',
			'data' => $data['ms_user'],
			'placeholder' => '',
			'popup' => [
				'parameters' => [
					'srctbl' => 'users',
					'srcfld1' => 'userid',
					'srcfld2' => 'fullname',
					'dstfrm' => $token_form->getName(),
					'dstfld1' => 'userid'
				]
			]
		]))->setWidth(ZBX_TEXTAREA_STANDARD_WIDTH)
	])
	->addRow(new CLabel(_('Description'), 'description'),
		(new CTextArea('description', $data['description']))
			->addClass(ZBX_STYLE_MONOSPACE_FONT)
			->setWidth(ZBX_TEXTAREA_STANDARD_WIDTH)
			->setMaxLength(DB::getFieldLength('token', 'description'))
			->setAriaRequired()
	)
	->addRow(_('Set expiration date and time'),
		(new CCheckBox('expires_state', '1'))
			->setChecked($data['expires_state'])
			->setUncheckedValue('0')
	)
	->addRow((new CLabel(_('Expires at'), 'expires_at'))->setAsteriskMark(),
		(new CDateSelector('expires_at', $data['expires_at']))
			->setDateFormat(ZBX_FULL_DATE_TIME)
			->setPlaceholder(_('YYYY-MM-DD hh:mm:ss'))
			->setAriaRequired(),
		'expires-at-row'
	)
	->addRow(_('Enabled'),
		(new CCheckBox('status', ZBX_AUTH_TOKEN_ENABLED))
			->setChecked($data['status'] == ZBX_AUTH_TOKEN_ENABLED)
			->setUncheckedValue(ZBX_AUTH_TOKEN_DISABLED)
	);


$token_view = (new CTabView())->addTab('token', '', $token_from_list);
$cancel_button = (new CRedirectButton(_('Cancel'), (new CUrl('zabbix.php'))
	->setArgument('action', 'token.list')
	->setArgument('page', CPagerHelper::loadPage('token.list', null))
))->setId('cancel');

if ($data['tokenid'] == 0) {
	$add_button = (new CSubmitButton(_('Add')))->setId('add');
	$token_view->setFooter(makeFormFooter($add_button, [$cancel_button]));
}
else {
	$update_button = (new CSubmitButton(_('Update')))->setId('update');
	$regenerate_button = (new CSimpleButton(_('Regenerate')))
		->setAttribute('data-confirmation', _('Regenerate selected API token? Previously generated token will become invalid.'))
		->setId('regenerate');
	$delete_button = (new CRedirectButton(_('Delete'), (new CUrl('zabbix.php'))
			->setArgumentSID()
			->setArgument('action', 'token.delete')
			->setArgument('action_src', 'token.list')
			->setArgument('tokenids', (array) $data['tokenid']),
		_('Delete selected API token?')
	))->setId('delete');

	$token_view->setFooter(makeFormFooter(
		$update_button,
		[
			$regenerate_button,
			$delete_button,
			$cancel_button
		]
	));
}

$token_form->addItem($token_view);
$widget
	->addItem($token_form)
	->show();
