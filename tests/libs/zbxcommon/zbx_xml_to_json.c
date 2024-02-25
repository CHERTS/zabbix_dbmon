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

#include "zbxmocktest.h"
#include "zbxmockutil.h"

void	zbx_mock_test_entry(void **state)
{
	char	*xml, *expected_json, *json = NULL, *error = NULL;
	int	actual_result, expected_result;

	ZBX_UNUSED(state);

	xml = (char *)zbx_mock_get_parameter_string("in.xml");
	expected_result = zbx_mock_str_to_return_code(zbx_mock_get_parameter_string("out.return"));
	expected_json = (char *)zbx_mock_get_parameter_string("out.json");
	actual_result = zbx_xml_to_json(xml, &json, &error);

	if (actual_result != expected_result || ( NULL != json && 0 != strcmp(expected_json, json)))
	{
#ifdef HAVE_LIBXML2
		fail_msg("Actual: %d \"%s\" != expected: %d \"%s\"", actual_result, json, expected_result,
				expected_json);
#else
		skip();
#endif
	}
	zbx_free(json);
	zbx_free(error);
}
