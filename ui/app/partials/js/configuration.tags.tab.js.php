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
 * @var CPartial $this
 */
?>

<script type="text/x-jquery-tmpl" id="tag-row-tmpl">
	<?= renderTagTableRow('#{rowNum}', '', '', ['add_post_js' => false]) ?>
</script>

<script type="text/javascript">
	jQuery(function() {
		let tags_initialized = false;

		$('#tabs').on('tabscreate tabsactivate', function(event, ui) {
			const $panel = (event.type === 'tabscreate') ? ui.panel : ui.newPanel;

			if ($panel.attr('id') === 'tags-tab') {
				if (tags_initialized) {
					return;
				}

				const $table = $('#tags-table');

				$table
					.dynamicRows({template: '#tag-row-tmpl'})
					.on('afteradd.dynamicRows', function() {
						$('.<?= ZBX_STYLE_TEXTAREA_FLEXIBLE ?>', $table).textareaFlexible();
					})
					.find('.<?= ZBX_STYLE_TEXTAREA_FLEXIBLE ?>')
					.textareaFlexible();

				tags_initialized = true;
			}
		});
	});
</script>
