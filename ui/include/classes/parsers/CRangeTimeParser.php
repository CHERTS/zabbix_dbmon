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
 * A parser for time used in the time selector.
 */
class CRangeTimeParser extends CParser {

	const ZBX_TIME_UNKNOWN = 0;
	const ZBX_TIME_ABSOLUTE = 1;
	const ZBX_TIME_RELATIVE = 2;

	/**
	 * @var int $time_type
	 */
	private $time_type;

	/**
	 * @var CRelativeTimeParser
	 */
	private $relative_time_parser;

	/**
	 * @var CAbsoluteTimeParser
	 */
	private $absolute_time_parser;

	public function __construct() {
		$this->relative_time_parser = new CRelativeTimeParser();
		$this->absolute_time_parser = new CAbsoluteTimeParser();
	}

	/**
	 * Parse the given period.
	 *
	 * @param string $source  Source string that needs to be parsed.
	 * @param int    $pos     Position offset.
	 */
	public function parse($source, $pos = 0) {
		$this->length = 0;
		$this->match = '';
		$this->time_type = self::ZBX_TIME_UNKNOWN;

		$p = $pos;

		if ($this->relative_time_parser->parse($source, $p) != self::PARSE_FAIL) {
			$p += $this->relative_time_parser->getLength();
			$this->time_type = self::ZBX_TIME_RELATIVE;
		}
		elseif ($this->absolute_time_parser->parse($source, $p) != self::PARSE_FAIL) {
			$p += $this->absolute_time_parser->getLength();
			$this->time_type = self::ZBX_TIME_ABSOLUTE;
		}
		else {
			return self::PARSE_FAIL;
		}

		$this->length = $p - $pos;
		$this->match = substr($source, $pos, $this->length);

		return isset($source[$p]) ? self::PARSE_SUCCESS_CONT : self::PARSE_SUCCESS;
	}

	public function getTimeType() {
		return $this->time_type;
	}

	/**
	 * Get DateTime object with its value set to either start or end of the period derived from the date/time specified.
	 *
	 * @param                   $is_start
	 * @param DateTimeZone|null $timezone
	 *
	 * @return DateTime|null
	 */
	public function getDateTime($is_start, DateTimeZone $timezone = null): ?DateTime {
		switch ($this->time_type) {
			case self::ZBX_TIME_ABSOLUTE:
				return $this->absolute_time_parser->getDateTime($is_start, $timezone);

			case self::ZBX_TIME_RELATIVE:
				return $this->relative_time_parser->getDateTime($is_start, $timezone);

			default:
				return null;
		}
	}
}
