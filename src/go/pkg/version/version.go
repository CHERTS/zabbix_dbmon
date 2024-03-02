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

// Package version provides zabbix release version
package version

import (
	"fmt"
	"strings"
)

const (
	ZABBIX_REVDATE          = "26 February 2024"
	ZABBIX_VERSION_MAJOR    = 6
	ZABBIX_VERSION_MINOR    = 0
	ZABBIX_VERSION_PATCH    = 27
	ZABBIX_VERSION_RC       = ""
	ZABBIX_VERSION_RC_NUM   = "{ZABBIX_RC_NUM}"
	ZABBIX_VERSION_REVISION = "{ZABBIX_REVISION}"
	copyrightMessage        = "Copyright (C) 2024 Zabbix SIA\n" +
		"License GPLv2+: GNU GPL version 2 or later <https://www.gnu.org/licenses/>.\n" +
		"This is free software: you are free to change and redistribute it according to\n" +
		"the license. There is NO WARRANTY, to the extent permitted by law."
)

var (
	titleMessage  string = "{undefined}"
	compileDate   string = "{undefined}"
	compileTime   string = "{undefined}"
	compileOs     string = "{undefined}"
	compileArch   string = "{undefined}"
	compileMode   string
	extraLicenses []string
)

func RevDate() string {
	return ZABBIX_REVDATE
}

func Major() int {
	return ZABBIX_VERSION_MAJOR
}

func Minor() int {
	return ZABBIX_VERSION_MINOR
}

func Patch() int {
	return ZABBIX_VERSION_PATCH
}

func RC() string {
	return ZABBIX_VERSION_RC
}

func LongStr() string {
	var ver string = fmt.Sprintf("%d.%d.%d", Major(), Minor(), Patch())
	if len(RC()) != 0 {
		ver += " " + RC()
	}
	return ver
}

func Long() string {
	var ver string = fmt.Sprintf("%d.%d.%d", Major(), Minor(), Patch())
	if len(RC()) != 0 {
		ver += RC()
	}
	return ver
}

func Short() string {
	return fmt.Sprintf("%d.%d", Major(), Minor())
}

func Revision() string {
	return ZABBIX_VERSION_REVISION
}

func CopyrightMessage() string {
	msg := copyrightMessage

	for _, license := range extraLicenses {
		msg += license
	}

	return msg
}

func CompileDate() string {
	return compileDate
}

func CompileTime() string {
	return compileTime
}

func CompileOs() string {
	return compileOs
}

func CompileArch() string {
	return compileArch
}

func CompileMode() string {
	return compileMode
}

func TitleMessage() string {
	var title string = titleMessage
	if "windows" == compileOs {
		if -1 < strings.Index(compileArch, "64") {
			title += " Win64"
		} else {
			title += " Win32"
		}
	}

	if len(compileMode) != 0 {
		title += fmt.Sprintf(" (%s)", compileMode)
	}

	return title
}

func Display(additionalMessages []string) {
	fmt.Printf("%s (Zabbix) %s\n", TitleMessage(), Long())
	fmt.Printf("Revision %s %s, compilation time: %s %s\n", Revision(), RevDate(), CompileDate(), CompileTime())

	for _, msg := range additionalMessages {
		fmt.Println(msg)
	}

	fmt.Println()
	fmt.Println(CopyrightMessage())
}

func Init(title string, extra ...string) {
	titleMessage = title
	extraLicenses = append(extraLicenses, extra...)
}

func init() {
	extraLicenses = make([]string, 0)
}
