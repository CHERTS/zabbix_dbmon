#!/usr/bin/env bash

#
# Program: Oracle database monitoring script for Zabbix <dbmon.sh>
#
# Author: Mikhail Grigorev <sleuthhound at gmail dot com>
# 
# Current Version: 1.0.0
#
# License:
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#

# trap ctrl-c and call ctrl_c()
trap ctrl_c INT

function ctrl_c() {
	echo "** Trapped CTRL-C"
}

# Script parameters:
# 1 - ITEM
# 2 - DBType (oracle)
# 3 - Oracle listener name or Oracle service name
# 4 - Oracle service name
# 5 - Oracle listener path
# 6 - Not use
# 7 - Config file or config directory
# 8 - Not use
# 9 - Not use
for ((i=1;i<=$#;i++)); do
	__CMDVAR=$i
	eval 'PARAM'$__CMDVAR="$(echo \${$i})"
done

# Script name and dir
SCRIPT_DIR=$(dirname $0)
SCRIPT_NAME=$(basename $0)
# Settings
HOSTNAME=$(hostname)
# Database type
DB_TYPE=${PARAM2}

# Zabbix Agent config file
ZBX_AGENTD_CONFIG_FILE=/etc/zabbix/zabbix_agentd_dbmon.conf
ZBX_SERVER_DEFAULT_PORT=10051
# Zabbix home dir and cache file
ZBX_HOME_DIR=~zabbix
ZBX_TMP_DIR=/tmp
ZBX_CUSTOM_BIN_DIR=/usr/sbin

# Debug enable = 1, disabe = 0
DBS_ENABLE_DEBUG_MODE=1
# Logging
DBS_ENABLE_LOGGING=1
DBS_LOG_FILE="${ZBX_HOME_DIR}/${SCRIPT_NAME%.*}.log"
DBS_MAX_LOG_FILE_SIZE_IN_KB=51200
# Config
DBS_CONFIG_FILE="${ZBX_HOME_DIR}/${SCRIPT_NAME%.*}.conf"
# Maximum number of running script
DBS_ZBX_MON_PROCESSES_NUMBER_MAX=50

# Config file
if [ -f "${DBS_CONFIG_FILE}" ]; then
	source "${DBS_CONFIG_FILE}" >/dev/null 2>&1
fi

# A flag indicating that the initialization of all subsystems is complete. (DO NOT CHANGING THIS PARAMETR)
INIT_ALL_SUBSYS=0

# Log file is writeble (DO NOT CHANGING THIS PARAMETR)
LOGFILE_IS_WRITEBLE=0

PLATFORM="unknown"
UNAME_INFO=$(uname -s)
case "${UNAME_INFO}" in
	Linux)
		PLATFORM='linux'
		# Local settings
		export LANG=en_US.UTF-8
		export LANGUAGE=en_US.UTF-8
		export LC_COLLATE=C
		export LC_CTYPE=en_US.UTF-8
		;;
	AIX|Darwin)
		PLATFORM='aix'
		if [[ "${DB_TYPE}" = "oracle" ]]; then
			# Local settings
			export LANG=C
			export NLS_LANG=AMERICAN_AMERICA.CL8MSWIN1251
			export NLS_DATE_FORMAT="YYYY-MM-DD HH24:MI:SS"
		fi
		;;
	HP-UX)
		PLATFORM='hp-ux'
		echo "Sorry, while the HP-UX platform is not supported, but we are working on support right now."
		exit 1
		;;
	Solaris|SunOS)
		PLATFORM='sunos'
		echo "Sorry, while the SunOS platform is not supported, but we are working on support right now."
		exit 1
		;;
	*)
		echo "This OS is not supported, please, contact the developer by sleuthhound@programs74.ru"
		exit 1
		;;
esac

_command_exists() {
	type "${1}" &> /dev/null
}

if _command_exists printf ; then
	PRINTF_BIN=$(which printf)
	if [ ! -f "${PRINTF_BIN}" ]; then
		PRINTF_BIN="printf"
	fi
else
	if [ ${DBS_ENABLE_DEBUG_MODE} -eq 1 ]; then
		echo "ERROR: Command 'printf' not found." 
	else
		echo "ZBX_NOTSUPPORTED: Command 'printf' not found."
	fi
	exit 1
fi

if _command_exists date ; then
	DATE_BIN=$(which date)
else
	if [ ${DBS_ENABLE_DEBUG_MODE} -eq 1 ]; then
		echo "ERROR: Command 'date' not found."
	else
		echo "ZBX_NOTSUPPORTED: Command 'date' not found."
	fi
	exit 1
fi

if [[ "${PLATFORM}" = "aix" ]]; then
	ECHO_BIN="echo"
else
	if _command_exists echo ; then
		ECHO_BIN=$(which echo)
		if [ ! -f "${ECHO_BIN}" ]; then
			ECHO_BIN="echo"
		fi
	else
		${PRINTF_BIN} "ZBX_NOTSUPPORTED: %s" "Command \"echo\" not found."
		exit 1
	fi
fi

_message() {
	local MSG="$1"
	local FLAG1="${2}"
	if [ -n "${FLAG1}" ]; then
		${PRINTF_BIN} 'ZBX_NOTSUPPORTED: %s\n' "${MSG}"
	else
		MSG=$(${ECHO_BIN} "${MSG//\"/}")
		${PRINTF_BIN} 'ZBX_NOTSUPPORTED: %s\n' "${MSG}"
	fi
	if [ ${INIT_ALL_SUBSYS} -eq 1 ]; then
		_logging "${MSG}"
	fi
}

if [ -z "${PARAM1}" ]; then
	_message "The first parameter (items) is required (Ex: listener_discovery, service_discovery, listener_info, service_info)"
	exit 1
fi

if [ -z "${PARAM2}" ]; then
	_message "The second parameter (database type) is required (Ex: oracle)"
	exit 1
fi

DB_TYPE_ARRAY=(oracle)
for ((i=0; i<${#DB_TYPE_ARRAY[@]}; i++)); do
	if [[ "${PARAM2}" = "${DB_TYPE_ARRAY[$i]}" ]]; then
		DB_TYPE=${DB_TYPE_ARRAY[$i]}
	fi
done
if [ -z "${DB_TYPE}" ]; then
	_message "Database type ${PARAM2} not supported."
	exit 1
fi

if _command_exists tr ; then
	TR_BIN=$(which tr)
else
	_message "Command tr not found."
	exit 1
fi

if _command_exists grep ; then
	GREP_BIN=$(which grep)
else
	_message "Command grep not found."
	exit 1
fi

if _command_exists zabbix_agentd ; then
	ZABBIX_AGENTD_BIN=$(which zabbix_agentd)
else
	SBIN_SEARCH=$(${ECHO_BIN} "$PATH" | ${GREP_BIN} -c "${ZBX_CUSTOM_BIN_DIR}" 2>/dev/null)
	if [ ${SBIN_SEARCH} -eq 0 ]; then
		export PATH="$PATH:${ZBX_CUSTOM_BIN_DIR}"
		if _command_exists zabbix_agentd ; then
			ZABBIX_AGENTD_BIN=$(which zabbix_agentd)
		else
			_message "Command zabbix_agentd not found in directory ${ZBX_CUSTOM_BIN_DIR}"
			exit 1
		fi
	else
		_message "Command zabbix_agentd not found."
		exit 1
	fi
fi

# Checking the availability of necessary utilities
if [[ "${PLATFORM}" = "aix" ]]; then
	COMMAND_EXIST_ARRAY=(FILE SU DU SED ID GROUPS WHOAMI PS TAIL ISTAT LSATTR AWK CUT EXPR RM XARGS CAT WC DIRNAME TOUCH HEAD NAWK LS MV FIND EGREP)
else
	COMMAND_EXIST_ARRAY=(FILE SU DU SED ID GROUPS WHOAMI PS TAIL STAT AWK CUT EXPR RM XARGS CAT WC DIRNAME TOUCH MD5SUM HEAD LS MV FIND READLINK EGREP)
fi
for ((i=0; i<${#COMMAND_EXIST_ARRAY[@]}; i++)); do
	__CMDVAR=${COMMAND_EXIST_ARRAY[$i]}
	CMD_FIND=$(${ECHO_BIN} "${__CMDVAR}" | ${TR_BIN} '[:upper:]' '[:lower:]' 2>/dev/null)
	if _command_exists ${CMD_FIND} ; then
		eval $__CMDVAR'_BIN'="'$(which ${CMD_FIND})'"
		hash "${CMD_FIND}" >/dev/null 2>&1
	else
		_message "Command ${CMD_FIND} not found."
		exit 1
	fi
done

_ppid() {
	local PS_PID=${1:-$$}
	local IS_NUM_REGEXP='^[0-9]+$'
	local PS_PPID=0
	if [[ "${PLATFORM}" = "aix" ]]; then
		PS_PPID=$(${PS_BIN} -p ${PS_PID} -o ppid=parent | ${SED_BIN} 's/^[ \t]*//;s/[ ]*$//' | ${TAIL_BIN} -1 2>/dev/null)
	else
		PS_PPID=$(${PS_BIN} -p ${PS_PID} -o ppid= | ${SED_BIN} 's/^[ \t]*//;s/[ \t]*$//')
	fi
	if [[ ${PS_PPID} =~ ${IS_NUM_REGEXP} ]] ; then
		${ECHO_BIN} "${PS_PPID}"
	fi
}

_get_ps_cmd() {
	local PS_PID=${1:-$$}
	local PS_CMD=""
	local IS_NUM_REGEXP='^[0-9]+$'
	if [[ ${PS_PID} =~ ${IS_NUM_REGEXP} ]] ; then
		if [[ "${PLATFORM}" = "aix" ]]; then
			PS_CMD=$(${PS_BIN} -o args -p ${PS_PID} | ${SED_BIN} 's/^[ \t]*//;s/[ ]*$//' | ${TAIL_BIN} -1 2>/dev/null)
		else
			PS_CMD=$(${PS_BIN} -o cmd -p ${PS_PID} --no-headers | ${SED_BIN} 's/^[ \t]*//;s/[ \t]*$//' 2>/dev/null)
		fi
		if [ $? -eq 0 ]; then
			${ECHO_BIN} "${PS_CMD}"
		fi
	fi
}

# Return:
# 0 - directory
# 1 - correct symlink
# 2 - file
# 3 - not valid data
_check_file_or_directory() {
	PASSED=$1
	if [ -d "${PASSED}" ] ; then
		echo 0
	else
		if [ -L "${PASSED}" ] && [ -e "${PASSED}" ]; then
			echo 1
		elif [ -f "${PASSED}" ]; then
			echo 2
		else
			echo 3
		fi
	fi
}

_oracle_check_ora_home_symlink() {
	local DBS_DONT_ERROR=${1:-"0"}
	if [ -n "${ORACLE_HOME}" ]; then
		if [ -L "${ORACLE_HOME}" ]; then
			if [ -e "${ORACLE_HOME}" ]; then
				if [[ "${PLATFORM}" = "aix" ]]; then
					ORACLE_HOME_CHECK_LINK=$(${FIND_BIN} "${ORACLE_HOME}" -type l -exec ${LS_BIN} -l {} \; | ${AWK_BIN} -F'->' '{print $2}' | ${SED_BIN} 's/^[ \t]*//;s/[ ]*$//' | ${SED_BIN} 's/\/$//' 2>/dev/null)
				else
					ORACLE_HOME_CHECK_LINK=$(${READLINK_BIN} "${ORACLE_HOME}" 2>/dev/null)
				fi
				if [ -n "${ORACLE_HOME_CHECK_LINK}" ]; then
					DBS_ORACLE_HOME=${ORACLE_HOME_CHECK_LINK}
				else
					DBS_ORACLE_HOME=${ORACLE_HOME}
				fi
			else
				DBS_ORACLE_HOME=${ORACLE_HOME}
			fi
		elif [ -e "${ORACLE_HOME}" ]; then
			DBS_ORACLE_HOME=${ORACLE_HOME}
		else
			if [ "${DBS_DONT_ERROR}" -eq 0 ]; then
				_message "Directory ORACLE_HOME=${ORACLE_HOME} not found or access denied."
				exit 1
			else
				DBS_ORACLE_HOME=${ORACLE_HOME}
			fi
		fi
	fi
}

# Checking the availability of config file
ORA_CONFIG_DIR=""
if [ -n "${PARAM7}" ]; then
	CHECK_CONFIG_STATUS=$(_check_file_or_directory "${PARAM7}")
	if [ ${CHECK_CONFIG_STATUS} -eq 2 ]; then
		source "${PARAM7}" >/dev/null 2>&1
		if [[ "${DB_TYPE}" = "oracle" ]]; then
			_oracle_check_ora_home_symlink
			if [ -n "${ORACLE_SID}" ]; then
				DBS_ORACLE_SID=${ORACLE_SID}
			fi
		fi
	elif [ ${CHECK_CONFIG_STATUS} -eq 0 ]; then
		if [[ "${DB_TYPE}" = "oracle" ]]; then
			ORA_CONFIG_DIR="${PARAM7}"
		fi
	else
		_message "The seventh parameter is incorrect."
		exit 1
	fi
fi

CURRENT_USER=$(${WHOAMI_BIN})

_randhash(){ < /dev/urandom tr -dc "A-Za-z0-9" | ${HEAD_BIN} -c${1:-5}; ${ECHO_BIN}; }

ZBX_HOSTNAME=$(${ZABBIX_AGENTD_BIN} -c "${ZBX_AGENTD_CONFIG_FILE}" -t agent.hostname 2>/dev/null | ${CUT_BIN} -d'|' -f2- 2>/dev/null | ${SED_BIN} -e 's/]$//')
if [ -n "${ZBX_HOSTNAME}" ]; then
	if [[ "${HOSTNAME}" != "${ZBX_HOSTNAME}" ]]; then
		HOSTNAME="${ZBX_HOSTNAME}"
	fi
fi

ZBX_LOCK_FILE_NAME=$(${PRINTF_BIN} "%s-%s-%s_dbmon.lock" "${HOSTNAME}" "${DB_TYPE}" "${PARAM1}")
RANDOM_UNIQUE_HASH=$(_randhash)

# Check PARAM1
case "${PARAM1}" in
	listener_info)
		if [ -n "${PARAM3}" ]; then
			LISTENER_NAME="${PARAM3}"
			ZBX_LOCK_FILE_NAME="${ZBX_LOCK_FILE_NAME%.*}-${LISTENER_NAME}.lock"
		else
			_message "The third parameter (listener name) is required (Ex: LISTENER)"
			exit 1
		fi
		if [ -n "${PARAM5}" ]; then
			LISTENER_PATH="${PARAM5}"
		else
			_message "You must specify the fifth parameter (listener path) (Ex: /u01/app/oracle/dbhome_1/bin/tnslsnr)"
			exit 1
		fi
		;;
	service_info)
		if [ -n "${PARAM3}" ]; then
			SERVICE_NAME="${PARAM3}"
		else
			_message "The third parameter (service name) is required (Ex: LOGSB)"
			exit 1
		fi
		if [ -n "${PARAM4}" ]; then
			LISTENER_NAME="${PARAM4}"
			ZBX_LOCK_FILE_NAME="${ZBX_LOCK_FILE_NAME%.*}-${LISTENER_NAME}-${SERVICE_NAME}.metrics"
		else
			_message "You must specify the fourth parameter (listener name) (Ex: LISTENER)"
			exit 1
		fi
		if [ -n "${PARAM5}" ]; then
			LISTENER_PATH="${PARAM5}"
		else
			_message "You must specify the fifth parameter (listener path) (Ex: /u01/app/oracle/dbhome_1/bin/tnslsnr)"
			exit 1
		fi
		;;
esac

if [[ "${DB_TYPE}" != "oracle" ]]; then
	_message "Database type ${DB_TYPE} not supported."
	exit 1
fi

if [ ! -d "${ZBX_HOME_DIR}" ]; then
	ZBX_HOME_DIR="${ZBX_TMP_DIR}"
else
	ZBX_HOME_DIR="$(${ECHO_BIN} "${ZBX_HOME_DIR}" | ${SED_BIN} 's/\/$//')"
fi

# If changed ZBX_HOME_DIR then change the destination for the file log, collect and config.
DBS_LOG_FILE="${ZBX_HOME_DIR}/${SCRIPT_NAME%.*}.log"
DBS_CONFIG_FILE="${ZBX_HOME_DIR}/${SCRIPT_NAME%.*}.conf"

if [ ! -d "${ZBX_HOME_DIR}" ]; then
	_message "Zabbix home directory not found."
	exit 1
fi

if [[ "${PLATFORM}" = "aix" ]]; then
	STAT_BIN=${ISTAT_BIN}
fi

_can_user_read_from_file() {
	if [[ $# -lt 2 || ! -r $2 ]]; then
		${ECHO_BIN} 0
		return
	fi
	if [[ ${1} = "root" ]]; then
		${ECHO_BIN} 1
		return
	fi
}

_can_user_write_to_file() {
	if [[ $# -lt 2 || ! -r $2 ]]; then
		${ECHO_BIN} 0
		return
	fi

	if [[ ${1} = "root" ]]; then
		${ECHO_BIN} 1
		return
	fi

	local USER_ID=$(${ID_BIN} -u ${1} 2>/dev/null)
	local FILE_OWNER_ID=0
	local FILE_GROUP_NAME=0
	local FILE_ACCESS=0

	if [[ "${PLATFORM}" = "aix" ]]; then
		FILE_OWNER_ID=$(${STAT_BIN} ${2} | ${GREP_BIN} "Owner:" | ${AWK_BIN} -F' ' {'print $2'} | ${AWK_BIN} -F'(' {'print $1'})
		FILE_GROUP_NAME=$(${STAT_BIN} ${2} | ${GREP_BIN} "Group:" | ${AWK_BIN} -F' ' {'print $4'} | ${AWK_BIN} -F'(' {'print $2'} | ${CUT_BIN} -d')' -f1)
		local CHECK_STATUS=$(_check_file_or_directory "${2}")
		local FIRST_BIT=""
		if [ ${CHECK_STATUS} -eq 2 ]; then
			FIRST_BIT="-"
		elif [ ${CHECK_STATUS} -eq 0 ]; then
			FIRST_BIT="d"
		fi
		local FILE_ACCESS_RWX=$(${STAT_BIN} ${2} | ${GREP_BIN} "Protection:" | ${AWK_BIN} -F' ' {'print $2'})
		FILE_ACCESS=$(${ECHO_BIN} "${FIRST_BIT}${FILE_ACCESS_RWX}" | ${AWK_BIN} '{k=0;for(i=0;i<=8;i++)k+=((substr($1,i+2,1)~/[rwx]/)*2^(8-i));if(k)printf("%0o",k)}')
	else
		FILE_OWNER_ID=$(${STAT_BIN} -c "%u" ${2})
		FILE_GROUP_NAME=$(${STAT_BIN} -c "%G" ${2})
		FILE_ACCESS=$(${STAT_BIN} -c "%a" ${2})
	fi

	if [[ "${USER_ID}" == "${FILE_OWNER_ID}" ]]; then
		${ECHO_BIN} 1
		return
	fi

	local FILE_GROUP_ACCESS=${FILE_ACCESS:1:1}
	local FILE_OTHER_ACCESS=${FILE_ACCESS:2:2}
	local USER_GROUP_LIST=$(${GROUPS_BIN} ${1} 2>/dev/null)

	if [ ${FILE_OTHER_ACCESS} -eq 7 ]; then
		${ECHO_BIN} 1
		return
	fi

	if [ ${FILE_GROUP_ACCESS} -ge 6 ]; then
		for USR in ${USER_GROUP_LIST-nop}; do
			if [[ "${USR}" == "${FILE_GROUP_NAME}" ]]; then
				${ECHO_BIN} 1
				return
			fi
		done
	fi

	echo 0
}

_check_logfile_writeble() {
	local LOG_DIR=""
	local CURRENT_USER=""
	local NEW_CURRENT_USER=""
	local CHECK_LOGDIR_ACCESS=0
	local CHECK_LOGFILE_ACCESS=0

	LOGFILE_IS_WRITEBLE=0
	CURRENT_USER=$(${WHOAMI_BIN})
	if [[ "${PLATFORM}" = "mingw" ]]; then
		NEW_CURRENT_USER=$(${ECHO_BIN} "${CURRENT_USER}" | ${AWK_BIN} -F'\' '{print $2}')
		if [ -n "${NEW_CURRENT_USER}" ]; then
			CURRENT_USER=${NEW_CURRENT_USER}
		fi
	fi
	LOG_DIR=$(${DIRNAME_BIN} ${DBS_LOG_FILE})
	CHECK_LOGDIR_ACCESS=$(_can_user_write_to_file "${CURRENT_USER}" "${LOG_DIR}")
	if [[ "${CHECK_LOGDIR_ACCESS}" = "1" ]]; then
		if [ -f "${DBS_LOG_FILE}" ]; then
			CHECK_LOGFILE_ACCESS=$(_can_user_write_to_file "${CURRENT_USER}" "${DBS_LOG_FILE}")
			if [[ "${CHECK_LOGFILE_ACCESS}" = "1" ]]; then
				LOGFILE_IS_WRITEBLE=1
			else
				DBS_LOG_FILE=${ZBX_TMP_DIR}/${SCRIPT_NAME%.*}.log
				if [ -f "${DBS_LOG_FILE}" ]; then
					CHECK_LOGFILE_ACCESS=$(_can_user_write_to_file "${CURRENT_USER}" "${DBS_LOG_FILE}")
					if [[ "${CHECK_LOGFILE_ACCESS}" = "1" ]]; then
						LOGFILE_IS_WRITEBLE=1
					fi
				else
					${TOUCH_BIN} "${DBS_LOG_FILE}" >/dev/null 2>&1
					if [ $? -eq 0 ]; then
						LOGFILE_IS_WRITEBLE=1
					fi
				fi
			fi
		else
			${TOUCH_BIN} "${DBS_LOG_FILE}" >/dev/null 2>&1
			if [ $? -eq 0 ]; then
				LOGFILE_IS_WRITEBLE=1
			fi
		fi
	fi
}

_logging() {
	local MSG=${1}
	if [ ${DBS_ENABLE_LOGGING} -eq 1 ]; then
		if [ ${LOGFILE_IS_WRITEBLE} -eq 1 ]; then
			if [ -n "${RANDOM_UNIQUE_HASH}" ]; then
				${PRINTF_BIN} "%s | %s: %s\n" "$(${DATE_BIN} "+%d.%m.%Y %H:%M:%S")" "${RANDOM_UNIQUE_HASH}" "${MSG}" 1>>${DBS_LOG_FILE} 2>&1
			else
				${PRINTF_BIN} "%s: %s\n" "$(${DATE_BIN} "+%d.%m.%Y %H:%M:%S")" "${MSG}" 1>>${DBS_LOG_FILE} 2>&1
			fi
		fi
	fi
}

_debug_logging() {
	local MSG=${1}
	if [ ${DBS_ENABLE_DEBUG_MODE} -eq 1 ]; then
		_logging "${MSG}"
	fi
}

_logrotate() {
	local FILENAME=${1:-"${DBS_LOG_FILE}"}
	local FILESIZE_IN_KB=0
	local DATE_TIME=0
	local IS_NUM_REGEXP='^[0-9]+$'
	if [ ! -f "${FILENAME}" ]; then
		return 1
	fi
	FILESIZE_IN_KB=$(${DU_BIN} -k "${FILENAME}" 2>/dev/null | ${TR_BIN} -s '\t' ' ' 2>/dev/null | ${CUT_BIN} -d' ' -f1)
	if [[ ${FILESIZE_IN_KB} =~ ${IS_NUM_REGEXP} ]] ; then
		_debug_logging "Func: ${FUNCNAME[0]}: Size of log file '${FILENAME}' is ${FILESIZE_IN_KB} KB"
		_debug_logging "Func: ${FUNCNAME[0]}: Max size of log file '${FILENAME}' is ${DBS_MAX_LOG_FILE_SIZE_IN_KB} KB"
		if [ ${FILESIZE_IN_KB} -gt ${DBS_MAX_LOG_FILE_SIZE_IN_KB} ]; then
			_debug_logging "Func: ${FUNCNAME[0]}: Delete big log file ${FILENAME}"
			${RM_BIN} -f "${FILENAME}" >/dev/null 2>&1
			if [ $? -eq 0 ]; then
				${TOUCH_BIN} "${FILENAME}" >/dev/null 2>&1
				if [ $? -eq 0 ]; then
					LOGFILE_IS_WRITEBLE=1
				else
					LOGFILE_IS_WRITEBLE=0
				fi
			fi
		fi
	else
		_debug_logging "Func: ${FUNCNAME[0]}: Incorrect file size."
	fi
}

# Checking write access for a log file
if [ ${DBS_ENABLE_LOGGING} -eq 1 ]; then
	_check_logfile_writeble
fi

# Checking write access to the ${ZBX_HOME_DIR} directory
ZBX_CHECK_HOME_ACCESS=$(_can_user_write_to_file ${CURRENT_USER} "${ZBX_HOME_DIR}")
if [[ ${ZBX_CHECK_HOME_ACCESS} = "0" ]]; then
	_message "Home directory ${ZBX_HOME_DIR} not writeble for user ${CURRENT_USER}"
	exit 1
fi

DBS_SCRIPT_PID=$$

# Debug mode
if [ ${DBS_ENABLE_DEBUG_MODE} -eq 1 ]; then
	_logging "============================= DEBUG STARTED ============================="
	_logging "Func: Main: Debug mode is enabled."
	_logging "Func: Main: Script ${SCRIPT_NAME} PID: ${DBS_SCRIPT_PID}"
fi

ZBX_AGENTD_ACTIVE_PPID=$(_ppid)
_debug_logging "Func: Main: ZBX_AGENTD_ACTIVE_PPID=${ZBX_AGENTD_ACTIVE_PPID}"
if [ -n "${ZBX_AGENTD_ACTIVE_PPID}" ]; then
	ZBX_AGENTD_PID=$(_ppid ${ZBX_AGENTD_ACTIVE_PPID})
	_debug_logging "Func: Main: ZBX_AGENTD_PID=${ZBX_AGENTD_PID}"
fi
if [ -n "${ZBX_AGENTD_PID}" ]; then
	ZBX_AGENTD_CMD=$(_get_ps_cmd "${ZBX_AGENTD_PID}")
	_debug_logging "Func: Main: ZBX_AGENTD_CMD=${ZBX_AGENTD_CMD}"
	if [ -n "${ZBX_AGENTD_CMD}" ]; then
		ZBX_AGENTD_BIN=$(${ECHO_BIN} "${ZBX_AGENTD_CMD}" | ${CUT_BIN} -d' ' -f1 2>/dev/null)
		ZBX_AGENTD_CONF_FILE=$(${ECHO_BIN} "${ZBX_AGENTD_CMD}" | ${CUT_BIN} -d' ' -f3 2>/dev/null)
		_debug_logging "Func: Main: Detected ZBX_AGENTD_BIN=${ZBX_AGENTD_BIN}"
		_debug_logging "Func: Main: Standart ZABBIX_AGENTD_BIN=${ZABBIX_AGENTD_BIN}"
		_debug_logging "Func: Main: Detect ZBX_AGENTD_CONF_FILE=${ZBX_AGENTD_CONF_FILE}"
		_debug_logging "Func: Main: Standart ZBX_AGENTD_CONFIG_FILE=${ZBX_AGENTD_CONFIG_FILE}"
		if [ -f "${ZBX_AGENTD_BIN}" ]; then
			if [[ "${ZBX_AGENTD_BIN}" != "${ZABBIX_AGENTD_BIN}" ]]; then
				ZBX_FILE_TYPE=0
				if [[ "${PLATFORM}" = "aix" ]]; then
					ZBX_FILE_TYPE=$(${FILE_BIN} "${ZBX_AGENTD_BIN}" 2>/dev/null | ${GREP_BIN} -c XCOFF)
				else
					ZBX_FILE_TYPE=$(${FILE_BIN} "${ZBX_AGENTD_BIN}" 2>/dev/null | ${GREP_BIN} -c ELF)
				fi
				if [ ${ZBX_FILE_TYPE} -eq 1 ]; then
					ZABBIX_AGENTD_BIN=${ZBX_AGENTD_BIN}
				fi
			fi
		fi
		if [ -f "${ZBX_AGENTD_CONF_FILE}" ]; then
			if [[ "${ZBX_AGENTD_CONF_FILE}" != "${ZBX_AGENTD_CONFIG_FILE}" ]]; then
				ZBX_AGENTD_CONFIG_FILE=${ZBX_AGENTD_CONF_FILE}
				_debug_logging "Func: Main: New ZBX_AGENTD_CONFIG_FILE=${ZBX_AGENTD_CONFIG_FILE}"
				ZBX_HOSTNAME=$(${ZABBIX_AGENTD_BIN} -c "${ZBX_AGENTD_CONFIG_FILE}" -t agent.hostname 2>/dev/null | ${CUT_BIN} -d'|' -f2- | ${SED_BIN} -e 's/]$//')
				if [ -n "${ZBX_HOSTNAME}" ]; then
					if [[ "${HOSTNAME}" != "${ZBX_HOSTNAME}" ]]; then
						HOSTNAME="${ZBX_HOSTNAME}"
					fi
				fi
				_debug_logging "Func: Main: New HOSTNAME=${HOSTNAME}"
			fi
		fi
	fi
fi

ZBX_LOCK_FILE_NAME=$(${PRINTF_BIN} "%s-%s-%s_dbmon.lock" "${HOSTNAME}" "${DB_TYPE}" "${PARAM1}")

case "${PARAM1}" in
	listener_info)
		ZBX_LOCK_FILE_NAME="${ZBX_LOCK_FILE_NAME%.*}-${LISTENER_NAME}.metrics"
		;;
	service_info)
		ZBX_LOCK_FILE_NAME="${ZBX_LOCK_FILE_NAME%.*}-${LISTENER_NAME}-${SERVICE_NAME}.metrics"
		;;
esac

# Debug mode
if [ ${DBS_ENABLE_DEBUG_MODE} -eq 1 ]; then
	# Add debug prefix to cache and metrics file
	ZBX_LOCK_FILE_NAME="${ZBX_LOCK_FILE_NAME%.*}-debug.lock"
	if [ -z "${ZBX_AGENTD_PID}" ]; then
		_logging "Func: Main: Zabbix agentd active checks PID: <NotFound>"
	fi
	_logging "Func: Main: Script params: 1:${PARAM1}|2:${PARAM2}|3:${PARAM3}|4:${PARAM4}|5:${PARAM5}|6:${PARAM6}|7:${PARAM7}|8:${PARAM8}|9:${PARAM9}|10:${PARAM10}|11:${PARAM11}"
	_logging "Func: Main: ZBX_LOCK_FILE_NAME=${ZBX_LOCK_FILE_NAME}"

	# Debug config file params
	if [ -n "${PARAM7}" ]; then
		if [ -f "${PARAM7}" ]; then
			_logging "Including config file '${PARAM7}'"
		fi
		if [[ "${DB_TYPE}" = "oracle" ]]; then
			if [ -n "${ORACLE_HOME}" ]; then
				_logging "\$ORACLE_HOME variable found, value = ${ORACLE_HOME}"
				_logging "\$DBS_ORACLE_HOME variable found, value = ${DBS_ORACLE_HOME}"
			fi
			if [ -n "${ORACLE_SID}" ]; then
				_logging "\$ORACLE_SID variable found, value = ${ORACLE_SID}"
			fi
		fi
	fi
fi

# Init all subsys done
INIT_ALL_SUBSYS=1

_ps() {
	if [[ "$#" -eq 3 ]]; then
		local __RESULTVAR=$1
		local __FULLRESULTVAR=$2
		local REGEXP_FILTER=$3
	else
		return 1
	fi
	local PROCESS_NUM=0
	local PROCESS=""
	local OLD_IFS=$IFS
	IFS=$'\n'
	if [[ "${PLATFORM}" = "aix" ]]; then
		PROCESS=($(${PS_BIN} -eo pid,user,args | ${GREP_BIN} -v "${SCRIPT_NAME}" | ${GREP_BIN} -E "${REGEXP_FILTER}" | ${SED_BIN} 's/^[ \t]*//;s/[ ]*$//' 2>/dev/null))
	else
		PROCESS=($(${PS_BIN} -eo pid,user,cmd | ${GREP_BIN} -v "${SCRIPT_NAME}" | ${GREP_BIN} -E "${REGEXP_FILTER}" | ${SED_BIN} 's/^[ \t]*//;s/[ \t]*$//' 2>/dev/null))
	fi
	EXIT_CODE=$?
	#_debug_logging "Func: ${FUNCNAME[0]}: $(declare -p PROCESS | sed -e 's/^declare -a [^=]*=//')"
	#declare -p PROCESS >&2
	#declare -p PROCESS | sed -e 's/^declare -a [^=]*=//'
	local PROCESS_ARR="$(declare -p PROCESS)"
	local PROCESS_ARR_RES=$(${ECHO_BIN} "${PROCESS_ARR#*=}")
	if [ ${EXIT_CODE} -eq 0 ]; then
		PROCESS_NUM=${#PROCESS[*]}
	fi
	if [[ "$__RESULTVAR" ]]; then
		eval $__RESULTVAR="'${PROCESS_NUM}'"
	fi
	if [[ "$__FULLRESULTVAR" ]]; then
		eval $__FULLRESULTVAR="'${PROCESS_ARR_RES}'"
	fi
	IFS=${OLD_IFS}
	return ${EXIT_CODE}
}

############################# ORACLE #############################

# Function discovery Oracle listener services
# RETURN: Zabbix JSON discovery
_oracle_service_discovery() {
	local PS_FIND=""
	local PS_FIND_NUM=0
	local EXIT_CODE=0
	local ORA_LSNR_USER=""
	local ORA_LSNR_PATH=""
	local ORA_LSNR_NAME=""
	local LISTENER_CTRL_BIN=""
	local SERVICELIST=""
	local SERVICE_LIST_NUM=0
	local SERVICE_DISCOVERY_RESULT=""
	local LSNCTRL_SERVICE_LIST=""
	local ORA_BIN_DIR=""
	local ORA_HOME=""
	local LSNCTRL_CHECK_ERROR=""
	local TIME_NOW=""
	local OLD_IFS=""
	OLD_IFS=$IFS
	IFS=$'\n'
	if [[ "${PLATFORM}" = "aix" ]]; then
		PS_FIND=($(${PS_BIN} -eo user,args | ${GREP_BIN} -v "${SCRIPT_NAME}" | ${GREP_BIN} -v "zabbix_get" | ${GREP_BIN} [t]nslsnr | ${SED_BIN} 's/^[ \t]*//;s/[ ]*$//' 2>/dev/null))
	else
		PS_FIND=($(${PS_BIN} -eo user,cmd | ${GREP_BIN} -v "${SCRIPT_NAME}" | ${GREP_BIN} -v "zabbix_get" | ${GREP_BIN} [t]nslsnr | ${SED_BIN} 's/^[ \t]*//;s/[ \t]*$//' 2>/dev/null))
	fi
	if [ $? -eq 0 ]; then
		PS_FIND_NUM=${#PS_FIND[*]}
		_debug_logging "Func: ${FUNCNAME[0]}: Found ${PS_FIND_NUM} tnslsnr process"
		_debug_logging "Func: ${FUNCNAME[0]}: PS_FIND=$(declare -p PS_FIND | ${SED_BIN} -e 's/^declare -a [^=]*=//')"
		for ((i=0; i<${#PS_FIND[@]}; i++)); do
			if [ -n "${PS_FIND[$i]}" ]; then
				local ORA_LSNR_USER=$(${ECHO_BIN} "${PS_FIND[$i]}" | ${AWK_BIN} -F' ' '{print $1}')
				local ORA_LSNR_PATH=$(${ECHO_BIN} "${PS_FIND[$i]}" | ${AWK_BIN} -F' ' '{print $2}')
				local ORA_LSNR_NAME=$(${ECHO_BIN} "${PS_FIND[$i]}" | ${AWK_BIN} -F' ' '{print $3}')
				if [ -f "${ORA_LSNR_PATH}"  ]; then
					LISTENER_CTRL_BIN="$(${DIRNAME_BIN} "${ORA_LSNR_PATH}")/lsnrctl"
					ORA_BIN_DIR=$(${DIRNAME_BIN} "${ORA_LSNR_PATH}")
					ORA_HOME=${ORA_BIN_DIR%/*}
					export ORACLE_HOME=${ORA_HOME}
					if [ -f "${LISTENER_CTRL_BIN}" ]; then
						_debug_logging "Func: ${FUNCNAME[0]}: ORA_LSNR_NAME=${ORA_LSNR_NAME}, ORA_LSNR_PATH=${ORA_LSNR_PATH}, ORA_LSNR_USER=${ORA_LSNR_USER}"
						_debug_logging "Func: ${FUNCNAME[0]}: Run: ${LISTENER_CTRL_BIN} status ${ORA_LSNR_NAME} | ${GREP_BIN} -iE '^ERROR|ERROR:|ORA-[0-9]{1,}|PLS-|SP2-|IMP-|TNS-[0-9]{1,}'"
						LSNCTRL_CHECK_ERROR=$(${LISTENER_CTRL_BIN} status ${ORA_LSNR_NAME} | ${GREP_BIN} -iE '^ERROR|ERROR:|ORA-[0-9]{1,}|PLS-|SP2-|IMP-|TNS-[0-9]{1,}')
						_debug_logging "Func: ${FUNCNAME[0]}: LSNCTRL_CHECK_ERROR=${LSNCTRL_CHECK_ERROR}"
						if [ -z "${LSNCTRL_CHECK_ERROR}" ]; then
							_debug_logging "Func: ${FUNCNAME[0]}: Run: ${LISTENER_CTRL_BIN} status ${ORA_LSNR_NAME} | ${GREP_BIN} -iE \"^service(*.)\" | ${GREP_BIN} -vi \"summary\" | ${AWK_BIN} -F' ' '{print \$2}' 2>/dev/null | ${TR_BIN} -d \\\""
							LSNCTRL_SERVICE_LIST=( $(${LISTENER_CTRL_BIN} status ${ORA_LSNR_NAME} | ${GREP_BIN} -iE "^service(.*)" | ${GREP_BIN} -vi "summary" | ${AWK_BIN} -F' ' '{print $2}' 2>/dev/null | ${TR_BIN} -d \" 2>/dev/null) )
							SERVICE_LIST_NUM=${#LSNCTRL_SERVICE_LIST[*]}
							_debug_logging "Func: ${FUNCNAME[0]}: Found ${SERVICE_LIST_NUM} service."
							for ((j=0; j<${#LSNCTRL_SERVICE_LIST[@]}; j++)); do
								if [ -n "${LSNCTRL_SERVICE_LIST[$j]}" ]; then
									SERVICELIST="${SERVICELIST},"'{"{#SERVICE_NAME}":"'${LSNCTRL_SERVICE_LIST[$j]}'","{#HOSTNAME}":"'${HOSTNAME}'","{#LSNR_NAME}":"'${ORA_LSNR_NAME}'","{#LSNR_PATH}":"'${LISTENER_CTRL_BIN}'"}'
								fi
							done
						fi
					else
						_debug_logging "Func: ${FUNCNAME[0]}: ERROR: Binary file ${LISTENER_CTRL_BIN} not found."
					fi
				else
					_debug_logging "Func: ${FUNCNAME[0]}: ERROR: Binary file ${ORA_LSNR_PATH} not found."
				fi
			fi
		done
		if [ ${PS_FIND_NUM} -ne 0 ]; then
			SERVICE_DISCOVERY_RESULT='{"data":['${SERVICELIST#,}']}'
			_debug_logging "Func: ${FUNCNAME[0]}: SERVICE_LIST = ${SERVICE_DISCOVERY_RESULT}"
			${ECHO_BIN} "${SERVICE_DISCOVERY_RESULT}"
		else
			${ECHO_BIN} "{\"data\":[]}"
		fi
		IFS=${OLD_IFS}
	else
		${ECHO_BIN} "{\"data\":[]}"
	fi
}

# Function send Oracle service info to Zabbix server
# PARAM1: Service name
# PARAM2: Listener name
# PARAM3: Listener binary file path
_oracle_service_info() {
	local SERVICE_NAME=$1
	local LISTENER_NAME=$2
	local LISTENER_PATH=$3
	local EXIT_CODE=0
	local LISTENER_CTRL_BIN=""
	local INSTANCE_NAME=""
	local INSTANCE_STATUS=""
	local INSTANCE_STATUS_INT=0
	local NUM_OF_INST_IN_SERVICE=0
	local SET_SERVICE_STATUS=0
	local INSTANCE_IN_SERVICE_LIST=("")
	if [ -f "${LISTENER_PATH}"  ]; then
		LISTENER_CTRL_BIN="$(${DIRNAME_BIN} "${LISTENER_PATH}")/lsnrctl"
		local ORA_BIN_DIR=$(${DIRNAME_BIN} "${LISTENER_PATH}")
		local ORA_HOME=${ORA_BIN_DIR%/*}
		export ORACLE_HOME=${ORA_HOME}
		if [ -f "${LISTENER_CTRL_BIN}" ]; then
			local TIME_NOW=$(${DATE_BIN} +%s)
			_debug_logging "Func: ${FUNCNAME[0]}: Run: ${LISTENER_CTRL_BIN} status \"${LISTENER_NAME}\" | ${GREP_BIN} 'Service \"${SERVICE_NAME}\"'"
			local SERVICE_STATUS=$(${LISTENER_CTRL_BIN} status "${LISTENER_NAME}" | ${GREP_BIN} "Service \"${SERVICE_NAME}\"")
			if [ -n "${SERVICE_STATUS}" ]; then
				if [[ "${PLATFORM}" = "aix" ]]; then
					NUM_OF_INST_IN_SERVICE=$(${LISTENER_CTRL_BIN} status "${LISTENER_NAME}" | ${GREP_BIN} "Service \"${SERVICE_NAME}\"" | ${SED_BIN} -n "s/.*has \([^ ]*\) instance.*/\1/p")
				else
					NUM_OF_INST_IN_SERVICE=$(${LISTENER_CTRL_BIN} status "${LISTENER_NAME}" | ${GREP_BIN} "Service \"${SERVICE_NAME}\"" | ${SED_BIN} -r "s/.*has ([^ ]+) instance.*/\1/")
				fi
				_debug_logging "Func: ${FUNCNAME[0]}: NUM_OF_INST_IN_SERVICE=${NUM_OF_INST_IN_SERVICE}"
				if [ -z "${NUM_OF_INST_IN_SERVICE}" ]; then
					NUM_OF_INST_IN_SERVICE=0
				fi
				SET_SERVICE_STATUS=1
				local OLD_IFS=$IFS
				IFS=$'\n'
				if [[ "${PLATFORM}" = "aix" ]]; then
					INSTANCE_IN_SERVICE_LIST=( $(${LISTENER_CTRL_BIN} status "${LISTENER_NAME}" | ${NAWK_BIN} "\$0~s{for(c=NR-b;c<=NR+a;c++)r[c]=1}{q[NR]=\$0}END{for(c=1;c<=NR;c++)if(r[c])print q[c]}" b=0 a=${NUM_OF_INST_IN_SERVICE} s="Service \"${SERVICE_NAME}\"" | ${GREP_BIN} "Instance" | ${SED_BIN} 's/^[ \t]*//;s/[ ]*$//' | ${SED_BIN} "s/\"//g") )
				else
					INSTANCE_IN_SERVICE_LIST=( $(${LISTENER_CTRL_BIN} status "${LISTENER_NAME}" | ${GREP_BIN} "Service \"${SERVICE_NAME}\"" -A ${NUM_OF_INST_IN_SERVICE} | ${TAIL_BIN} -n ${NUM_OF_INST_IN_SERVICE} | ${SED_BIN} "s/^[ \t]*//;s/[ ]*$//" | ${SED_BIN} "s/\"//g") )
				fi
				_debug_logging "Func: ${FUNCNAME[0]}: INSTANCE_IN_SERVICE_LIST=$(declare -p INSTANCE_IN_SERVICE_LIST | ${SED_BIN} -e 's/^declare -a [^=]*=//')"
				for ((i=0; i<${#INSTANCE_IN_SERVICE_LIST[@]}; i++)); do
					if [ -n "${INSTANCE_IN_SERVICE_LIST[$i]}" ]; then
						if [[ "${PLATFORM}" = "aix" ]]; then
							INSTANCE_NAME=$(${ECHO_BIN} "${INSTANCE_IN_SERVICE_LIST[$i]}" | ${SED_BIN} -n "s/Instance \([^,]*\).*/\1/p")
							INSTANCE_STATUS=$(${ECHO_BIN} "${INSTANCE_IN_SERVICE_LIST[$i]}" | ${SED_BIN} -n "s/.*status \([^,]*\).*/\1/p")
						else
							INSTANCE_NAME=$(${ECHO_BIN} "${INSTANCE_IN_SERVICE_LIST[$i]}" | ${SED_BIN} -r "s/Instance ([^,]+).*/\1/")
							INSTANCE_STATUS=$(${ECHO_BIN} "${INSTANCE_IN_SERVICE_LIST[$i]}" | ${SED_BIN} -r "s/.*status ([^,]+).*/\1/")
						fi
						case ${INSTANCE_STATUS} in
							UNKNOWN) 
								INSTANCE_STATUS_INT=0
								;;
							READY)
								INSTANCE_STATUS_INT=1
								;;
							*)
								INSTANCE_STATUS_INT=2
						esac
						_debug_logging "Func: ${FUNCNAME[0]}: INSTANCE_NAME=${INSTANCE_NAME}|INSTANCE_STATUS=${INSTANCE_STATUS}"
						#INSTANCELIST="${INSTANCELIST},"'{"{#INSTANCE_NAME}":"'${INSTANCE_NAME}'","{#INSTANCE_STATUS}":"'${INSTANCE_STATUS_INT}'"}'
						INSTANCELIST="${INSTANCELIST},${INSTANCE_NAME}:${INSTANCE_STATUS}"
					fi
				done
				IFS=$OLD_IFS
				#INSTANCE_LIST_RESULT='['${INSTANCELIST#,}']'
				INSTANCE_LIST_RESULT="${INSTANCELIST#,}"
			else
				SET_SERVICE_STATUS=0
			fi
			${ECHO_BIN} "{\"SERVICE_STATUS\":${SET_SERVICE_STATUS},\"SERVICE_INSTANCE_NUMBER\":${NUM_OF_INST_IN_SERVICE},\"SERVICE_INSTANCE_LIST\":\"${INSTANCE_LIST_RESULT}\"}"
		else
			_message "Binary file ${LISTENER_CTRL_BIN} not found."
		fi
	else
		_message "Binary file ${LISTENER_PATH} not found."
	fi
}

# Function send Oracle listener info to Zabbix server
# PARAM1: Listener name
# PARAM2: Listener binary file path
_oracle_listener_info() {
	local LISTENER_NAME=$1
	local LISTENER_PATH=$2
	local EXIT_CODE=0
	local LISTENER_CTRL_BIN=""
	local PS_FIND=()
	local PS_FIND_NUM=0
	local OLD_IFS=""
	local LSNCTRL_CHECK_ERROR=""
	local LSNCTRL_ERROR=""
	local LSNCTRL_STATUS=0
	local LSNCTRL_VERSION="0"
	if [ -f "${LISTENER_PATH}"  ]; then
		LISTENER_CTRL_BIN="$(${DIRNAME_BIN} "${LISTENER_PATH}")/lsnrctl"
		local ORA_BIN_DIR=$(${DIRNAME_BIN} "${LISTENER_PATH}")
		local ORA_HOME=${ORA_BIN_DIR%/*}
		export ORACLE_HOME=${ORA_HOME}
		if [ -f "${LISTENER_CTRL_BIN}" ]; then
			OLD_IFS=$IFS
			IFS=$'\n'
			if [[ "${PLATFORM}" = "aix" ]]; then
				PS_FIND=($(${PS_BIN} -eo user,args | ${GREP_BIN} -v "${SCRIPT_NAME}" 2>/dev/null | ${GREP_BIN} -v "zabbix_get" 2>/dev/null | ${GREP_BIN} [t]nslsnr 2>/dev/null | ${GREP_BIN} "${LISTENER_NAME}" 2>/dev/null | ${SED_BIN} 's/^[ \t]*//;s/[ ]*$//' 2>/dev/null))
			else
				PS_FIND=($(${PS_BIN} -eo user,cmd | ${GREP_BIN} -v "${SCRIPT_NAME}" 2>/dev/null | ${GREP_BIN} -v "zabbix_get" 2>/dev/null | ${GREP_BIN} [t]nslsnr 2>/dev/null | ${GREP_BIN} "${LISTENER_NAME}" 2>/dev/null | ${SED_BIN} 's/^[ \t]*//;s/[ \t]*$//' 2>/dev/null))
			fi
			if [ $? -eq 0 ]; then
				PS_FIND_NUM=${#PS_FIND[*]}
				_debug_logging "Func: ${FUNCNAME[0]}: Found ${PS_FIND_NUM} tnslsnr process"
				_debug_logging "Func: ${FUNCNAME[0]}: PS_FIND=$(declare -p PS_FIND | ${SED_BIN} -e 's/^declare -a [^=]*=//')"
				if [ ${PS_FIND_NUM} -gt 0 ]; then
					_debug_logging "Func: ${FUNCNAME[0]}: Run: ${LISTENER_CTRL_BIN} status ${LISTENER_NAME} | ${GREP_BIN} -iE '^ERROR|ERROR:|ORA-[0-9]{1,}|PLS-|SP2-|IMP-|TNS-[0-9]{1,}'"
					LSNCTRL_CHECK_ERROR=$(${LISTENER_CTRL_BIN} status ${LISTENER_NAME} | ${GREP_BIN} -iE '^ERROR|ERROR:|ORA-[0-9]{1,}|PLS-|SP2-|IMP-|TNS-[0-9]{1,}' 2>/dev/null)
					if [ -z "${LSNCTRL_CHECK_ERROR}" ]; then
						if [[ "${PLATFORM}" = "aix" ]]; then
							LSNCTRL_VERSION=$(${LISTENER_CTRL_BIN} status ${LISTENER_NAME} | ${GREP_BIN} -iE "^Version" 2>/dev/null | ${SED_BIN} -n "s/.*Version \([^ ]*\).*/\1/p")
						else
							LSNCTRL_VERSION=$(${LISTENER_CTRL_BIN} status ${LISTENER_NAME} | ${GREP_BIN} -iE "^Version" 2>/dev/null | ${SED_BIN} -r "s/.*Version ([^ ]+).*/\1/")
						fi
						LSNCTRL_STATUS=1
					else
						LSNCTRL_STATUS=2
						LSNCTRL_ERROR=$(${ECHO_BIN} "${LSNCTRL_CHECK_ERROR}" | ${HEAD_BIN} -1 2>/dev/null)
						_debug_logging "Func: ${FUNCNAME[0]}: LSNCTRL_ERROR=${LSNCTRL_ERROR}"
					fi
				else
					LSNCTRL_STATUS=0
				fi
				IFS=${OLD_IFS}
				${ECHO_BIN} "{\"LSNR_STATUS\":${LSNCTRL_STATUS},\"LSNR_VERSION\":\"${LSNCTRL_VERSION}\",\"LSNR_ERROR\":\"${LSNCTRL_ERROR}\"}"
			fi
		else
			_message "Binary file ${LISTENER_CTRL_BIN} not found."
		fi
	else
		_message "Binary file ${LISTENER_PATH} not found."
	fi
}

# Function discovery Oracle listener for all running instance on server
# RETURN: Zabbix JSON discovery
_oracle_listener_discovery() {
	if [[ "$#" -eq 1 ]]; then
		local __RESULTVAR=$1
	fi
	local RESULT=""
	local OLD_IFS=""
	local PS_FIND=""
	local PS_FIND_NUM=0
	local LSNRLIST=""
	local ORA_LSNR_USER=""
	local ORA_LSNR_PATH=""
	local ORA_LSNR_NAME=""
	local ORA_LSNR_BIN_NAME=""
	OLD_IFS=$IFS
	IFS=$'\n'
	if [[ "${PLATFORM}" = "aix" ]]; then
		PS_FIND=($(${PS_BIN} -eo user,args | ${GREP_BIN} -v "${SCRIPT_NAME}" | ${GREP_BIN} "[t]nslsnr" | ${SED_BIN} 's/^[ \t]*//;s/[ ]*$//' 2>/dev/null))
	else
		PS_FIND=($(${PS_BIN} -eo user,cmd | ${GREP_BIN} -v "${SCRIPT_NAME}" | ${GREP_BIN} "[t]nslsnr" | ${SED_BIN} 's/^[ \t]*//;s/[ \t]*$//' 2>/dev/null))
	fi
	if [ $? -eq 0 ]; then
		PS_FIND_NUM=${#PS_FIND[*]}
		_debug_logging "Func: ${FUNCNAME[0]}: Found ${PS_FIND_NUM} listener."
		_debug_logging "Func: ${FUNCNAME[0]}: PS_FIND=$(declare -p PS_FIND | ${SED_BIN} -e 's/^declare -a [^=]*=//')"
		for ((i=0; i<${#PS_FIND[@]}; i++)); do
			_debug_logging "Func: ${FUNCNAME[0]}: ARR[$i]: ${PS_FIND[$i]}"
			if [ -n "${PS_FIND[$i]}" ]; then
				ORA_LSNR_USER=$(${ECHO_BIN} "${PS_FIND[$i]}" | ${AWK_BIN} -F' ' '{print $1}')
				ORA_LSNR_PATH=$(${ECHO_BIN} "${PS_FIND[$i]}" | ${AWK_BIN} -F' ' '{print $2}')
				ORA_LSNR_NAME=$(${ECHO_BIN} "${PS_FIND[$i]}" | ${AWK_BIN} -F' ' '{print $3}')
				if [ -f "${ORA_LSNR_PATH}" ]; then
					ORA_LSNR_BIN_NAME=$(basename "${ORA_LSNR_PATH}")
					if [[ "${ORA_LSNR_BIN_NAME}" = "tnslsnr" ]]; then
						if [ -n "${ORA_LSNR_NAME}" ]; then
							_debug_logging "Func: ${FUNCNAME[0]}: LSNR_USER:${ORA_LSNR_USER}|LSNR_NAME:${ORA_LSNR_NAME}|LSNR_PATH:${ORA_LSNR_PATH}"
							LSNRLIST="${LSNRLIST},"'{"{#HOSTNAME}":"'${HOSTNAME}'","{#LSNR_NAME}":"'${ORA_LSNR_NAME}'","{#LSNR_PATH}":"'${ORA_LSNR_PATH}'","{#LSNR_USER}":"'${ORA_LSNR_USER}'"}'
						fi
					fi
				fi
			fi
		done
	fi
	IFS=$OLD_IFS
	if [[ ${PS_FIND_NUM} -ne 0 ]]; then
		RESULT='{"data":['${LSNRLIST#,}']}'
	else
		RESULT='{"data":[]}'
	fi
	if [[ "$__RESULTVAR" ]]; then
		eval $__RESULTVAR="'${RESULT}'"
	else
		${ECHO_BIN} "${RESULT}"
	fi
}

############################# END ORACLE #############################

################################ MAIN ###############################

RES=""
EXIT_CODE=0

# Check running process
DBS_ZBX_MON_PROCESSES=()
DBS_ZBX_MON_PROCESSES_NUMBER=0
OLD_IFS=$IFS
IFS=$'\n'
if [[ "${PLATFORM}" = "aix" ]]; then
	DBS_ZBX_MON_PROCESSES=($(${PS_BIN} -eo pid,user,args | ${GREP_BIN} -v "${DBS_SCRIPT_PID}" | ${GREP_BIN} "${SCRIPT_NAME}" | ${GREP_BIN} "${PARAM2}" | ${GREP_BIN} -v "${PARAM1}" | ${SED_BIN} 's/^[ \t]*//;s/[ ]*$//' 2>/dev/null))
else
	DBS_ZBX_MON_PROCESSES=($(${PS_BIN} -eo pid,user,cmd | ${GREP_BIN} -v "${DBS_SCRIPT_PID}" | ${GREP_BIN} "${SCRIPT_NAME}" | ${GREP_BIN} "${PARAM2}" | ${GREP_BIN} -v "${PARAM1}" | ${SED_BIN} 's/^[ \t]*//;s/[ \t]*$//' 2>/dev/null))
fi
EXIT_CODE=$?
if [ ${EXIT_CODE} -eq 0 ]; then
	DBS_ZBX_MON_PROCESSES_NUMBER=${#DBS_ZBX_MON_PROCESSES[*]}
fi
IFS=${OLD_IFS}
_debug_logging "Func: Main: dbs_zbx_mon_processes_number=${DBS_ZBX_MON_PROCESSES_NUMBER}"
if [ ${DBS_ZBX_MON_PROCESSES_NUMBER} -gt ${DBS_ZBX_MON_PROCESSES_NUMBER_MAX} ]; then
	_debug_logging "Func: Main: Found critical number of running script (over ${DBS_ZBX_MON_PROCESSES_NUMBER_MAX})."
	_debug_logging "Func: Main: dbs_zbx_mon_processes_list=$(declare -p DBS_ZBX_MON_PROCESSES | ${SED_BIN} -e 's/^declare -a [^=]*=//')"
	_debug_logging "Func: Main: Script execution was interrupted..."
	_debug_logging "============================= DEBUG END ================================="
	exit 2
fi

# Check lock file
if [ -f "${ZBX_HOME_DIR}/${ZBX_LOCK_FILE_NAME}" ]; then
	_debug_logging "Func: Main: Lock file ${ZBX_HOME_DIR}/${ZBX_LOCK_FILE_NAME} found."
	DBS_LOCK_PID=$(${CAT_BIN} "${ZBX_HOME_DIR}/${ZBX_LOCK_FILE_NAME}")
	if [ -n "${DBS_LOCK_PID}" ]; then
		DBS_IS_NUM_REGEXP='^[0-9]+$'
     	if [[ ${DBS_LOCK_PID} =~ ${DBS_IS_NUM_REGEXP} ]] ; then
			DBS_CMD=$(_get_ps_cmd "${DBS_LOCK_PID}")
			if [ -n "${DBS_CMD}" ]; then
				_debug_logging "Func: Main: SCRIPT_NAME = ${SCRIPT_NAME} | DBS_LOCK_PID = ${DBS_LOCK_PID} | DBS_CMD = ${DBS_CMD}"
				DBS_IS_BACKGROUND=$(${ECHO_BIN} "${DBS_CMD}" | ${GREP_BIN} -c "background")
				if [ ${DBS_IS_BACKGROUND} -eq 0 ]; then
					DBS_LOCK_SCRIPT_NAME=$(${ECHO_BIN} "${DBS_CMD}" | ${AWK_BIN} -F' ' '{print $1}')
				else
					DBS_LOCK_SCRIPT_NAME=$(${ECHO_BIN} "${DBS_CMD}" | ${AWK_BIN} -F' ' '{print $2}')
				fi
				DBS_LOCK_SCRIPT_NAME=$(basename ${DBS_LOCK_SCRIPT_NAME})
				_debug_logging "Func: Main: DBS_LOCK_SCRIPT_NAME = ${DBS_LOCK_SCRIPT_NAME}"
				if [ "${SCRIPT_NAME}" = "${DBS_LOCK_SCRIPT_NAME}" ]; then
					_debug_logging "Func: Main: Script execution was interrupted..."
					_debug_logging "============================= DEBUG END ================================="
					exit 2
				fi
			else
				_debug_logging "Func: Main: Empty process '${DBS_LOCK_PID}' cmd string, lock file ${ZBX_HOME_DIR}/${ZBX_LOCK_FILE_NAME} has been deleted."
				${RM_BIN} -f "${ZBX_HOME_DIR}/${ZBX_LOCK_FILE_NAME}" 2>/dev/null
			fi
		else
			${RM_BIN} -f "${ZBX_HOME_DIR}/${ZBX_LOCK_FILE_NAME}" 2>/dev/null
		fi
	else
		_debug_logging "Func: Main: Empty lock file ${ZBX_HOME_DIR}/${ZBX_LOCK_FILE_NAME} has been deleted."
		${RM_BIN} -f "${ZBX_HOME_DIR}/${ZBX_LOCK_FILE_NAME}" 2>/dev/null
	fi
else
	_debug_logging "Func: Main: Create lock file ${ZBX_HOME_DIR}/${ZBX_LOCK_FILE_NAME}, PID: ${DBS_SCRIPT_PID}"
	${PRINTF_BIN} "%s" "${DBS_SCRIPT_PID}" > "${ZBX_HOME_DIR}/${ZBX_LOCK_FILE_NAME}" 2>/dev/null
fi

if [[ "$PARAM1" = "listener_discovery" ]]; then
	_logrotate
	if [[ "${DB_TYPE}" = "oracle" ]]; then
		RES=$(_oracle_listener_discovery)
		EXIT_CODE=$?
		if [ ${EXIT_CODE} -ne 0 ]; then
			_debug_logging "Func: Main: An error occurred while searching for Oracle listener."
		fi
	fi
	if [ ${EXIT_CODE} -eq 0 ]; then
		${ECHO_BIN} "${RES}"
	fi
	RCODE="0"
elif [[ "$PARAM1" = "service_discovery" ]]; then
	if [[ "${DB_TYPE}" = "oracle" ]]; then
		RES=$(_oracle_service_discovery)
		EXIT_CODE=$?
		if [ ${EXIT_CODE} -ne 0 ]; then
			_debug_logging "Func: Main: An error occurred while searching for Oracle listener service."
		fi
	fi
	if [ ${EXIT_CODE} -eq 0 ]; then
		${ECHO_BIN} "${RES}"
	fi
	RCODE="0"
elif [[ "$PARAM1" = "service_info" ]]; then
	if [[ "${DB_TYPE}" = "oracle" ]]; then
		_oracle_service_info "${SERVICE_NAME}" "${LISTENER_NAME}" "${LISTENER_PATH}"
	fi
	RCODE="0"
elif [[ "$PARAM1" = "listener_info" ]]; then
	if [[ "${DB_TYPE}" = "oracle" ]]; then
		_oracle_listener_info "${LISTENER_NAME}" "${LISTENER_PATH}"
	fi
	RCODE="0"
else
	_message "Incorrect set first script params."
	RCODE="1"
fi

# Check lock file
if [ -f "${ZBX_HOME_DIR}/${ZBX_LOCK_FILE_NAME}" ]; then
	_debug_logging "Func: Main: Delete lock file ${ZBX_HOME_DIR}/${ZBX_LOCK_FILE_NAME}"
	${RM_BIN} -f "${ZBX_HOME_DIR}/${ZBX_LOCK_FILE_NAME}" 2>/dev/null
fi

# Debug mode
_debug_logging "============================= DEBUG END ================================="

exit ${RCODE}
