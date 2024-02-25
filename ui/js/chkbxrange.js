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


/*
 * Automatic checkbox range selection
 */
var chkbxRange = {
	startbox:			null,	// start checkbox obj
	chkboxes:			{},		// ckbx list
	prefix:				null,	// prefix for session storage variable name
	pageGoName:			null,	// which checkboxes should be counted by Go button and saved to session storage
	footerButtons:		{},		// action buttons at the bottom of page
	sessionStorageName:	null,

	init: function() {
		var path = new Curl();
		var filename = basename(path.getPath(), '.php');
		this.sessionStorageName = 'cb_' + filename + (this.prefix ? '_' + this.prefix : '');
		// Erase old checkboxes.
		this.chkboxes = {};
		this.startbox = null;

		this.resetOtherPage();

		// initialize checkboxes
		var chkboxes = jQuery('.list-table tbody input[type=checkbox]:not(:disabled)');
		if (chkboxes.length > 0) {
			for (var i = 0; i < chkboxes.length; i++) {
				this.implement(chkboxes[i]);
			}
		}

		// load selected checkboxes from session storage or cache
		if (this.pageGoName != null) {
			const selected_ids = this.getSelectedIds();

			// check if checkboxes should be selected from session storage
			if (!jQuery.isEmptyObject(selected_ids)) {
				var objectIds = jQuery.map(selected_ids, function(id) { return id });
			}
			// no checkboxes selected, check browser cache if checkboxes are still checked and update state
			else {
				var checkedFromCache = jQuery('main .list-table tbody input[type=checkbox]:checked:not(:disabled)');
				var objectIds = jQuery.map(checkedFromCache, jQuery.proxy(function(checkbox) {
					return this.getObjectIdFromName(checkbox.name);
				}, this));
			}

			this.checkObjects(this.pageGoName, objectIds, true);
			this.update(this.pageGoName);
		}

		this.footerButtons = jQuery('#action_buttons button:not(.no-chkbxrange)');
		var thisChkbxRange = this;
		this.footerButtons.each(function() {
			addListener(this, 'click', thisChkbxRange.submitFooterButton.bindAsEventListener(thisChkbxRange), false);
		});
	},

	implement: function(obj) {
		// skip the "select all" checkbox
		if (obj.name.indexOf('all_') > -1) {
			return;
		}

		var objName = this.getObjectFromName(obj.name);

		if (typeof(this.chkboxes[objName]) === 'undefined') {
			this.chkboxes[objName] = [];
		}
		this.chkboxes[objName].push(obj);

		addListener(obj, 'click', this.handleClick.bindAsEventListener(this), false);

		if (objName == this.pageGoName) {
			var objId = jQuery(obj).val();
			if (isset(objId, this.getSelectedIds())) {
				obj.checked = true;
			}
		}
	},

	getSelectedIds() {
		const session_selected_ids = sessionStorage.getItem(this.sessionStorageName);

		return session_selected_ids === null ? {} : JSON.parse(session_selected_ids);
	},

	/**
	 * Handles a click on one of the checkboxes.
	 *
	 * @param e
	 */
	handleClick: function(e) {
		e = e || window.event;
		var checkbox = e.target;

		PageRefresh.restart();

		var object = this.getObjectFromName(checkbox.name);
		var objectId = this.getObjectIdFromName(checkbox.name);

		// range selection
		if ((e.ctrlKey || e.shiftKey) && this.startbox != null) {
			this.checkObjectRange(object, this.startbox, checkbox, this.startbox.checked);
		}
		// an individual checkbox
		else {
			this.checkObjects(object, [objectId], checkbox.checked);
		}

		this.update(object);

		this.startbox = checkbox;
	},

	/**
	 * Extracts the name of an object from the name of a checkbox.
	 *
	 * @param {string} name
	 *
	 * @returns {string}
	 */
	getObjectFromName: function(name) {
		return name.split('[')[0];
	},

	/**
	 * Extracts the ID of an object from the name of a checkbox.
	 *
	 * @param {string} name
	 *
	 * @returns {string}
	 */
	getObjectIdFromName: function(name) {
		var id = name.split('[')[1];
		id = id.substring(0, id.lastIndexOf(']'));

		return id;
	},

	/**
	 * Returns the checkboxes in an object group.
	 *
	 * @param string object
	 *
	 * @returns {Array}
	 */
	getObjectCheckboxes: function(object) {
		return this.chkboxes[object] || [];
	},

	/**
	 * Toggle all checkboxes of the given objects.
	 *
	 * Checks all of the checkboxes that belong to these objects and highlights the table row.
	 *
	 * @param {string}  object
	 * @param {Array}   objectIds     array of objects IDs as integers
	 * @param {bool}    checked
	 */
	checkObjects: function(object, objectIds, checked) {
		const selected_ids = this.getSelectedIds();

		jQuery.each(this.getObjectCheckboxes(object), jQuery.proxy(function(i, checkbox) {
			var objectId = this.getObjectIdFromName(checkbox.name);

			if (objectIds.indexOf(objectId) > -1) {
				checkbox.checked = checked;

				jQuery(checkbox).closest('tr').toggleClass('row-selected', checked);
				// Remove class attribute if it's empty
				jQuery(checkbox).closest('tr').filter('*[class=""]').removeAttr('class');

				if (checked) {
					selected_ids[objectId] = objectId;
				}
				else {
					delete selected_ids[objectId];
				}
			}
		}, this));

		this.saveSessionStorage(object, selected_ids);
	},

	/**
	 * Toggle all objects between the two checkboxes.
	 *
	 * @param {string} object
	 * @param {object} startCheckbox
	 * @param {object} endCheckbox
	 * @param {bool} checked
	 */
	checkObjectRange: function(object, startCheckbox, endCheckbox, checked) {
		var checkboxes = this.getObjectCheckboxes(object);

		var startCheckboxIndex = checkboxes.indexOf(startCheckbox);
		var endCheckboxIndex = checkboxes.indexOf(endCheckbox);
		var start = Math.min(startCheckboxIndex, endCheckboxIndex);
		var end = Math.max(startCheckboxIndex, endCheckboxIndex);

		var objectIds = [];
		for (var i = start; i <= end; i++) {
			objectIds.push(this.getObjectIdFromName(checkboxes[i].name));
		}
		this.checkObjects(object, objectIds, checked);
	},

	/**
	 * Toggle all of the checkboxes belonging to the given object group.
	 *
	 * @param {string} object
	 *
	 * @param {boolean} checked
	 */
	checkObjectAll: function(object, checked) {
		// main checkbox exists and is clickable, but other checkboxes may not exist and object may be empty
		var objectIds = jQuery.map(this.getObjectCheckboxes(object), jQuery.proxy(function(checkbox) {
			return this.getObjectIdFromName(checkbox.name);
		}, this));

		this.checkObjects(object, objectIds, checked);
	},

	/**
	 * Update the general state after toggling a checkbox.
	 *
	 * @param {string} object
	 */
	update: function(object) {
		// update main checkbox state
		this.updateMainCheckbox(object);

		if (this.pageGoName == object) {
			this.updateGoButton();
		}
	},

	/**
	 * Update the state of the "Go" controls.
	 */
	updateGoButton: function() {
		var count = 0;
		jQuery.each(this.getSelectedIds(), function() {
			count++;
		});

		var selectedCountSpan = jQuery('#selected_count');
		selectedCountSpan.text(count + ' ' + selectedCountSpan.text().split(' ')[1]);

		jQuery('#action_buttons button').each((_, val) => {
			const $val = jQuery(val);

			if (!$val.data('disabled')) {
				$val.prop('disabled', count == 0);
			}
		});
	},

	/**
	 * Select main checkbox if all other checkboxes are selected.
	 *
	 * @param {string} object
	 */
	updateMainCheckbox: function(object) {
		const checkbox_list = this.getObjectCheckboxes(object);
		const $main_checkbox = $(checkbox_list)
			.parents('table')
			.find('thead input[type=checkbox]');

		if ($main_checkbox.length == 0) {
			return;
		}

		const count_available = checkbox_list.length;

		if (count_available > 0) {
			const checked = [];

			jQuery.each(checkbox_list, (i, checkbox) => {
				if (checkbox.checked) {
					checked.push(checkbox);
				}
			});

			$main_checkbox[0].checked = (checked.length == count_available);
		}
	},

	/**
	 * Save the state of the checkboxes belonging to the given object group in SessionStorage.
	 *
	 * @param {string} object
	 * @param {Object} selected_ids  key/value pairs of selected ids.
	 */
	saveSessionStorage: function(object, selected_ids) {
		if (Object.keys(selected_ids).length > 0) {
			if (this.pageGoName == object) {
				sessionStorage.setItem(this.sessionStorageName, JSON.stringify(selected_ids));
			}
		}
		else {
			sessionStorage.removeItem(this.sessionStorageName);
		}
	},

	clearSelectedOnFilterChange: function() {
		sessionStorage.removeItem(this.sessionStorageName);
	},

	/**
	 * Reset all selections on other pages.
	 */
	resetOtherPage: function() {
		var key_;

		for (var i = 0; i < sessionStorage.length; i++) {
			key_ = sessionStorage.key(i);

			if (key_.substring(0, 3) === 'cb_' && key_ != this.sessionStorageName) {
				sessionStorage.removeItem(key_);
			}
		}
	},

	submitFooterButton: function(e) {
		e = e || window.event;

		var footerButton = jQuery(e.target),
			form = footerButton.closest('form'),
			confirmText = footerButton.attr('confirm');
		if (confirmText && !confirm(confirmText)) {
			e.preventDefault();
			e.stopPropagation();

			return false;
		}

		const selected_ids = this.getSelectedIds();
		for (let key in selected_ids) {
			if (selected_ids.hasOwnProperty(key) && selected_ids[key] !== null) {
				create_var(form.attr('name'), this.pageGoName + '[' + key + ']', key, false);
			}
		}
		return true;
	}
};
