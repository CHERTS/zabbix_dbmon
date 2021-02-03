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
?>

<script type="text/javascript">
	$(() => {
		const $status = $('#filter_status');

		$('#filter_state').on('change', (e) => {
			$status.prop('disabled', e.target.value != -1);
		}).trigger('change');
	});
</script>

<script type="text/x-jquery-tmpl" id="tmpl_expressions_list_row">
<?=
	(new CRow([
		(new CCol([
			(new CDiv())
				->addClass(ZBX_STYLE_DRAG_ICON)
				->addStyle('top: 0px;'),
			(new CSpan())->addClass('ui-icon ui-icon-arrowthick-2-n-s move '.ZBX_STYLE_TD_DRAG_ICON)
		]))->addClass(ZBX_STYLE_TD_DRAG_ICON),
		(new CDiv('#{expression}'))->setWidth(ZBX_TEXTAREA_MEDIUM_WIDTH),
		new CDiv('#{type_label}'),
		(new CCol([
			(new CVar('expressions[][value]', '#{expression}')),
			(new CVar('expressions[][type]', '#{type}')),
			(new CButton(null, _('Remove')))->addClass(ZBX_STYLE_BTN_LINK)
		]))->addClass(ZBX_STYLE_NOWRAP)
	]))
		->addClass('sortable form_row')
?>
</script>

<script type="text/x-jquery-tmpl" id="tmpl_expressions_part_list_row">
<?=
	(new CRow([
		(new CDiv('#{keyword}'))->setWidth(ZBX_TEXTAREA_MEDIUM_WIDTH),
		new CDiv('#{type_label}'),
		(new CCol([
			(new CVar('keys[][value]', '#{keyword}')),
			(new CVar('keys[][type]', '#{type_label}')),
			(new CButton(null, _('Remove')))->addClass(ZBX_STYLE_BTN_LINK)
		]))->addClass(ZBX_STYLE_NOWRAP)
	]))
		->addClass('form_row')
?>
</script>
