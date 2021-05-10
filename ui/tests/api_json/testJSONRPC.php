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


require_once dirname(__FILE__).'/../include/CAPITest.php';

class testJSONRPC extends CAPITest {

	public static function json_rpc_data() {
		return [
			// rpc call with invalid JSON
			[
				'request' => '{"jsonrpc": "2.0", "method": "foobar, "params": "bar", "baz]',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32700,
						'message' => 'Parse error',
						'data' => 'Invalid JSON. An error occurred on the server while parsing the JSON text.'
					],
					'id' => null
				]
			],
			// rpc call Batch, invalid JSON
			[
				'request' =>
					'['.
						'{"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},'.
						'{"jsonrpc": "2.0", "method"'.
					']',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32700,
						'message' => 'Parse error',
						'data' => 'Invalid JSON. An error occurred on the server while parsing the JSON text.'
					],
					'id' => null
				]
			],
			// rpc call with an empty Array
			[
				'request' => '[]',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32600,
						'message' => 'Invalid Request.',
						'data' => 'The received JSON is not a valid JSON-RPC Request.'
					],
					'id' => null
				]
			],
			// rpc call with an invalid scalar data
			[
				'request' => '12345',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32600,
						'message' => 'Invalid Request.',
						'data' => 'The received JSON is not a valid JSON-RPC Request.'
					],
					'id' => null
				]
			],
			// rpc call with invalid "jsonrpc"
			[
				'request' => '{"jsonrpc": null}',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32600,
						'message' => 'Invalid Request.',
						'data' => 'Invalid parameter "/jsonrpc": a character string is expected.'
					],
					'id' => null
				]
			],
			// rpc call with invalid version "jsonrpc"
			[
				'request' => '{"jsonrpc": "1.0"}',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32600,
						'message' => 'Invalid Request.',
						'data' => 'Invalid parameter "/jsonrpc": value must be one of 2.0.'
					],
					'id' => null
				]
			],
			// rpc call with invalid "method"
			[
				'request' => '{"jsonrpc": "2.0", "method": 1}',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32600,
						'message' => 'Invalid Request.',
						'data' => 'Invalid parameter "/method": a character string is expected.'
					],
					'id' => null
				]
			],
			// rpc call with invalid "params"
			[
				'request' => '{"jsonrpc": "2.0", "method": "host.get", "params": "abc"}',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32600,
						'message' => 'Invalid Request.',
						'data' => 'Invalid parameter "/params": an array or object is expected.'
					],
					'id' => null
				]
			],
			// rpc call with invalid "auth"
			[
				'request' => '{"jsonrpc": "2.0", "method": "host.get", "params": {}, "auth": 12345, "id": 1}',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32600,
						'message' => 'Invalid Request.',
						'data' => 'Invalid parameter "/auth": a character string is expected.'
					],
					'id' => 1
				]
			],
			// rpc call with invalid "id"
			[
				'request' => '{"jsonrpc": "2.0", "method": "host.get", "params": {}, "auth": null, "id": true}',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32600,
						'message' => 'Invalid Request.',
						'data' => 'Invalid parameter "/id": a string, number or null value is expected.'
					],
					'id' => null
				]
			],
			// rpc call with invalid batch (but not empty)
			[
				'request' => '[1]',
				'result' => [
					[
						'jsonrpc' => '2.0',
						'error' => [
							'code' => -32600,
							'message' => 'Invalid Request.',
							'data' => 'Invalid parameter "/": an array is expected.'
						],
						'id' => null
					]
				]
			],
			// rpc call with invalid batch
			[
				'request' => '[1, 2, 3]',
				'result' => [
					[
						'jsonrpc' => '2.0',
						'error' => [
							'code' => -32600,
							'message' => 'Invalid Request.',
							'data' => 'Invalid parameter "/": an array is expected.'
						],
						'id' => null
					],
					[
						'jsonrpc' => '2.0',
						'error' => [
							'code' => -32600,
							'message' => 'Invalid Request.',
							'data' => 'Invalid parameter "/": an array is expected.'
						],
						'id' => null
					],
					[
						'jsonrpc' => '2.0',
						'error' => [
							'code' => -32600,
							'message' => 'Invalid Request.',
							'data' => 'Invalid parameter "/": an array is expected.'
						],
						'id' => null
					]
				]
			],
			// rpc call with empty "method"
			[
				'request' => '{"jsonrpc": "2.0", "method": "", "params": {}, "id": null}',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32601,
						'message' => 'Method not found.',
						'data' => 'Incorrect API "".'
					],
					'id' => null
				]
			],
			// rpc call of non-existent API class
			[
				'request' => '{"jsonrpc": "2.0", "method": "foo", "params": {}, "id": 5}',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32601,
						'message' => 'Method not found.',
						'data' => 'Incorrect API "foo".'
					],
					'id' => 5
				]
			],
			// rpc call of non-existent method
			[
				'request' => '{"jsonrpc": "2.0", "method": "apiinfo.get", "params": {}, "id": 5}',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32601,
						'message' => 'Method not found.',
						'data' => 'Incorrect method "apiinfo.get".'
					],
					'id' => 5
				]
			],
			// a notification
			[
				'request' => '{"jsonrpc": "2.0", "method": "apiinfo.version", "params": {}}',
				'result' => ''
			],
			// a notification with non-existent method
			[
				'request' => '{"jsonrpc": "2.0", "method": "foobar", "params": {}}',
				'result' => ''
			],
			// rpc call batch (all notifications)
			[
				'request' => '['.
					'{"jsonrpc": "2.0", "method": "apiinfo.version", "params": {}},'.
					'{"jsonrpc": "2.0", "method": "apiinfo.version", "params": {}}'.
				']',
				'result' => ''
			],
			// rpc call with extra parameters (deprecated, will be unsupported since version 5.4)
			[
				'request' => '{"jsonrpc": "2.0", "method": "apiinfo.version", "params": {}, "id": 1, "foo": "bar"}',
				'result' => [
					'jsonrpc' => '2.0',
					'result' => ZABBIX_API_VERSION,
					'id' => 1
				]
			],
			// rpc call batch
			[
				'request' => '['.
					'{"jsonrpc": "2.0", "method": "apiinfo.version", "params": {}, "id": 1},'.
					'{"jsonrpc": "2.0", "method": "apiinfo.version", "params": {}},'.
					'{"jsonrpc": "2.0", "method": "foo", "params": {}},'.
					'{"method": "foo", "params": {}, "id": 2},'.
					'{"jsonrpc": "2.0", "method": "foo", "id": 3},'.
					'"abc",'.
					'{"jsonrpc": "2.0", "method": "apiinfo.version", "params": {}, "id": 4}'.
				']',
				'result' => [
					[
						'jsonrpc' => '2.0',
						'result' => ZABBIX_API_VERSION,
						'id' => 1
					],
					[
						'jsonrpc' => '2.0',
						'error' => [
							'code' => -32600,
							'message' => 'Invalid Request.',
							'data' => 'Invalid parameter "/": the parameter "jsonrpc" is missing.'
						],
						'id' => 2
					],
					[
						'jsonrpc' => '2.0',
						'error' => [
							'code' => -32600,
							'message' => 'Invalid Request.',
							'data' => 'Invalid parameter "/": the parameter "params" is missing.'
						],
						'id' => 3
					],
					[
						'jsonrpc' => '2.0',
						'error' => [
							'code' => -32600,
							'message' => 'Invalid Request.',
							'data' => 'Invalid parameter "/": an array is expected.'
						],
						'id' => null
					],
					[
						'jsonrpc' => '2.0',
						'result' => ZABBIX_API_VERSION,
						'id' => 4
					]
				]
			],
			// rpc call with not required auth
			[
				'request' => '{"jsonrpc": "2.0", "method": "apiinfo.version", "params": {}, "auth": "token", "id": 5}',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32602,
						'message' => 'Invalid params.',
						'data' => 'The "apiinfo.version" method must be called without the "auth" parameter.'
					],
					'id' => 5
				]
			],
			// rpc call without required auth
			[
				'request' => '{"jsonrpc": "2.0", "method": "user.get", "params": {}, "auth": null, "id": 5}',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32602,
						'message' => 'Invalid params.',
						'data' => 'Not authorised.'
					],
					'id' => 5
				]
			],
			// rpc call without required auth
			[
				'request' => '{"jsonrpc": "2.0", "method": "user.get", "params": {}, "id": 5}',
				'result' => [
					'jsonrpc' => '2.0',
					'error' => [
						'code' => -32602,
						'message' => 'Invalid params.',
						'data' => 'Not authorised.'
					],
					'id' => 5
				]
			]
		];
	}

	/**
	 * @dataProvider json_rpc_data
	 */
	public function testJSONRPC_Calls($request, $expected_result) {
		$this->assertSame($expected_result, $this->callRaw($request));
	}
}
