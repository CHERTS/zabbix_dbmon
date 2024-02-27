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

<script>
	const view = {
		init({host, dashboard, widget_defaults, time_period, web_layout_mode}) {
			if (dashboard.pages.length > 1
					|| (dashboard.pages.length === 1 && dashboard.pages[0].widgets.length !== 0)) {
				timeControl.refreshPage = false;

				ZABBIX.Dashboard = new CDashboard(document.querySelector('.<?= ZBX_STYLE_DASHBOARD ?>'), {
					containers: {
						grid: document.querySelector('.<?= ZBX_STYLE_DASHBOARD_GRID ?>'),
						navigation: document.querySelector('.<?= ZBX_STYLE_DASHBOARD_NAVIGATION ?>'),
						navigation_tabs: document.querySelector('.<?= ZBX_STYLE_DASHBOARD_NAVIGATION_TABS ?>')
					},
					buttons: web_layout_mode == <?= ZBX_LAYOUT_KIOSKMODE ?>
						? {
							previous_page: document.querySelector('.<?= ZBX_STYLE_BTN_DASHBOARD_KIOSKMODE_PREVIOUS_PAGE?>'),
							next_page: document.querySelector('.<?= ZBX_STYLE_BTN_DASHBOARD_KIOSKMODE_NEXT_PAGE ?>'),
							slideshow: document.querySelector('.<?= ZBX_STYLE_BTN_DASHBOARD_KIOSKMODE_TOGGLE_SLIDESHOW ?>')
						}
						: {
							previous_page: document.querySelector('.<?= ZBX_STYLE_DASHBOARD_PREVIOUS_PAGE ?>'),
							next_page: document.querySelector('.<?= ZBX_STYLE_DASHBOARD_NEXT_PAGE ?>'),
							slideshow: document.querySelector('.<?= ZBX_STYLE_DASHBOARD_TOGGLE_SLIDESHOW ?>')
						},
					data: {
						dashboardid: dashboard.dashboardid,
						name: dashboard.name,
						userid: null,
						templateid: dashboard.templateid,
						display_period: dashboard.display_period,
						auto_start: dashboard.auto_start
					},
					max_dashboard_pages: <?= DASHBOARD_MAX_PAGES ?>,
					cell_width: 100 / <?= DASHBOARD_MAX_COLUMNS ?>,
					cell_height: 70,
					max_columns: <?= DASHBOARD_MAX_COLUMNS ?>,
					max_rows: <?= DASHBOARD_MAX_ROWS ?>,
					widget_min_rows: <?= DASHBOARD_WIDGET_MIN_ROWS ?>,
					widget_max_rows: <?= DASHBOARD_WIDGET_MAX_ROWS ?>,
					widget_defaults: widget_defaults,
					is_editable: false,
					is_edit_mode: false,
					can_edit_dashboards: false,
					is_kiosk_mode: web_layout_mode == <?= ZBX_LAYOUT_KIOSKMODE ?>,
					time_period: time_period,
					dynamic_hostid: host.hostid,
				});

				for (const page of dashboard.pages) {
					for (const widget of page.widgets) {
						widget.fields = (typeof widget.fields === 'object') ? widget.fields : {};
						widget.configuration = (typeof widget.configuration === 'object') ? widget.configuration : {};
					}

					ZABBIX.Dashboard.addDashboardPage(page);
				}

				ZABBIX.Dashboard.activate();
			}

			if (web_layout_mode == <?= ZBX_LAYOUT_NORMAL ?>) {
				document.getElementById('dashboardid').addEventListener('change', this.events.dashboardChange);
			}

			jqBlink.blink();
		},

		events: {
			dashboardChange(e) {
				e.target.closest('form').submit();
			}
		}
	}
</script>
