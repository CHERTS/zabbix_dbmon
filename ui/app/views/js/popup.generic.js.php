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


/**
 * @var CView $this
 */
?>

window.popup_generic = {
	init() {
		cookie.init();
		chkbxRange.init();
	},

	setPopupOpenerFieldValues(entries) {
		Object.entries(entries).forEach(([element_id, set_value]) => {
			const target_element = document.getElementById(element_id);

			if (target_element !== null) {
				target_element.value = set_value;
			}
		});
	},

	initGroupsFilter() {
		var overlay = overlays_stack.end();

		jQuery('.multiselect', overlay.$dialogue).each(function (i, ms) {
			jQuery(ms).on('change', {overlay: overlay}, function (e) {
				const groups = jQuery(this).multiSelect('getData').map((item) => item.id);
				const parameters = groups.length ? {groupid: groups[0]} : {filter_groupid_rst: 1, groupid: []};

				PopUp(e.data.overlay.action, {...e.data.overlay.options, ...parameters}, {
					dialogueid: e.data.overlay.dialogueid
				});
			});
		});
	},

	initHostsFilter() {
		var overlay = overlays_stack.end();

		jQuery('.multiselect', overlay.$dialogue).each(function (i, ms) {
			jQuery(ms).on('change', {overlay: overlay}, function (e) {
				const hosts = jQuery(this).multiSelect('getData').map((item) => item.id);
				const parameters = hosts.length ? {hostid: hosts[0]} : {filter_hostid_rst: 1, hostid: []};

				PopUp(e.data.overlay.action, {...e.data.overlay.options, ...parameters}, {
					dialogueid: e.data.overlay.dialogueid
				});
			});
		});
	},

	initHelpItems() {
		$('#itemtype').on('change', (e) => {
			reloadPopup(e.target.closest('form'));
		});
	},

	setEmpty(e, reset_fields) {
		e.preventDefault();

		this.setPopupOpenerFieldValues(reset_fields);
		overlayDialogueDestroy(jQuery(e.target).closest('[data-dialogueid]').attr('data-dialogueid'));
	},

	closePopup(e) {
		e.preventDefault();

		const $sender = jQuery(e.target).removeAttr('onclick');

		overlayDialogueDestroy($sender.closest('[data-dialogueid]').attr('data-dialogueid'));
	}
};
