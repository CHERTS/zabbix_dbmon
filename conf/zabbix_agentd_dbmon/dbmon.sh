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
# Database type
DB_TYPE=${PARAM2}

# Debug enable = 1, disabe = 0
DBS_ENABLE_DEBUG_MODE=1
# Logging
DBS_ENABLE_LOGGING=1
DBS_LOG_FILE="/var/log/zabbix/${SCRIPT_NAME%.*}.log"
DBS_MAX_LOG_FILE_SIZE_IN_KB=51200
# Config
DBS_CONFIG_FILE="${SCRIPT_DIR}/${SCRIPT_NAME%.*}.conf"

# Config file
if [ -f "${DBS_CONFIG_FILE}" ]; then
	source "${DBS_CONFIG_FILE}" >/dev/null 2>&1
fi

# A flag indicating that the initialization of all subsystems is complete. (DO NOT CHANGING THIS PARAMETR)
INIT_ALL_SUBSYS=0

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
	echo "ZBX_NOTSUPPORTED: Command 'printf' not found."
	exit 1
fi

if _command_exists date ; then
	DATE_BIN=$(which date)
else
	echo "ZBX_NOTSUPPORTED: Command 'date' not found."
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
	${PRINTF_BIN} 'ZBX_NOTSUPPORTED: %s\n' "${MSG}"
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
	_message "Database type '${PARAM2}' not supported."
	exit 1
fi

if _command_exists tr ; then
	TR_BIN=$(which tr)
else
	_message "Command 'tr' not found."
	exit 1
fi

if _command_exists grep ; then
	GREP_BIN=$(which grep)
else
	_message "Command 'grep' not found."
	exit 1
fi

# Checking the availability of necessary utilities
COMMAND_EXIST_ARRAY=(DU SED PS TAIL AWK CUT RM CAT WC DIRNAME TOUCH HEAD)
for ((i=0; i<${#COMMAND_EXIST_ARRAY[@]}; i++)); do
	__CMDVAR=${COMMAND_EXIST_ARRAY[$i]}
	CMD_FIND=$(${ECHO_BIN} "${__CMDVAR}" | ${TR_BIN} '[:upper:]' '[:lower:]' 2>/dev/null)
	if _command_exists ${CMD_FIND} ; then
		eval $__CMDVAR'_BIN'="'$(which ${CMD_FIND})'"
		hash "${CMD_FIND}" >/dev/null 2>&1
	else
		_message "Command '${CMD_FIND}' not found."
		exit 1
	fi
done

_randhash(){ < /dev/urandom tr -dc "A-Za-z0-9" | ${HEAD_BIN} -c${1:-5}; ${ECHO_BIN}; }

RANDOM_UNIQUE_HASH=$(_randhash)

# Check PARAM1
case "${PARAM1}" in
	listener_info)
		if [ -n "${PARAM3}" ]; then
			LISTENER_NAME="${PARAM3}"
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
	_message "Database type '${DB_TYPE}' not supported."
	exit 1
fi

if [ ${DBS_ENABLE_LOGGING} -eq 1 ]; then
	LOG_DIR=$(dirname "${DBS_LOG_FILE}")
	if [ ! -d "${LOG_DIR}" ]; then
		DBS_ENABLE_LOGGING=0
		DBS_ENABLE_DEBUG_MODE=0
	fi
fi

_logging() {
	local MSG=${1}
	if [ ${DBS_ENABLE_LOGGING} -eq 1 ]; then
		if [ -n "${RANDOM_UNIQUE_HASH}" ]; then
			${PRINTF_BIN} "%s | %s: %s\n" "$(${DATE_BIN} "+%d.%m.%Y %H:%M:%S")" "${RANDOM_UNIQUE_HASH}" "${MSG}" 1>>${DBS_LOG_FILE} 2>&1
		else
			${PRINTF_BIN} "%s: %s\n" "$(${DATE_BIN} "+%d.%m.%Y %H:%M:%S")" "${MSG}" 1>>${DBS_LOG_FILE} 2>&1
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
			fi
		fi
	else
		_debug_logging "Func: ${FUNCNAME[0]}: Incorrect file size."
	fi
}

DBS_SCRIPT_PID=$$

# Start
_logging "============================= MAIN STARTED ============================="
_logging "Func: Main: Debug mode: ${DBS_ENABLE_DEBUG_MODE}"
_logging "Func: Main: Script ${SCRIPT_NAME} PID: ${DBS_SCRIPT_PID}"
_logging "Func: Main: Script params: 1:${PARAM1}|2:${PARAM2}|3:${PARAM3}|4:${PARAM4}|5:${PARAM5}|6:${PARAM6}|7:${PARAM7}|8:${PARAM8}|9:${PARAM9}"

# Init all subsys done
INIT_ALL_SUBSYS=1

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
	else
		${ECHO_BIN} "{\"data\":[]}"
	fi
	IFS=${OLD_IFS}
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
	local RESULT_STR=""
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
			RESULT_STR="{\"SERVICE_STATUS\":${SET_SERVICE_STATUS},\"SERVICE_INSTANCE_NUMBER\":${NUM_OF_INST_IN_SERVICE},\"SERVICE_INSTANCE_LIST\":\"${INSTANCE_LIST_RESULT}\"}"
			_debug_logging "Func: ${FUNCNAME[0]}: RESULT = ${RESULT_STR}"
			${ECHO_BIN} ${RESULT_STR}
		else
			_message "Binary file '${LISTENER_CTRL_BIN}' not found."
		fi
	else
		_message "Binary file '${LISTENER_PATH}' not found."
	fi
}

# Function discovery Oracle listener for all running instance on server
# RETURN: Zabbix JSON discovery
_oracle_listener_discovery() {
	local OLD_IFS=""
	local PS_FIND=""
	local PS_FIND_NUM=0
	local LSNRLIST=""
	local ORA_LSNR_USER=""
	local ORA_LSNR_PATH=""
	local ORA_LSNR_NAME=""
	local ORA_LSNR_BIN_NAME=""
	local LISTENER_DISCOVERY_RESULT=""
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
		if [ ${PS_FIND_NUM} -ne 0 ]; then
			LISTENER_DISCOVERY_RESULT='{"data":['${LSNRLIST#,}']}'
			_debug_logging "Func: ${FUNCNAME[0]}: LISTENER_LIST = ${LISTENER_DISCOVERY_RESULT}"
			${ECHO_BIN} "${LISTENER_DISCOVERY_RESULT}"
		else
			${ECHO_BIN} "{\"data\":[]}"
		fi
	else
		${ECHO_BIN} "{\"data\":[]}"
	fi
	IFS=$OLD_IFS
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
	local RESULT_STR=""
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
					LSNCTRL_CHECK_ERROR=$(${LISTENER_CTRL_BIN} status ${LISTENER_NAME} 2>/dev/null | ${GREP_BIN} -iE '^ERROR|ERROR:|ORA-[0-9]{1,}|PLS-|SP2-|IMP-|TNS-[0-9]{1,}' 2>/dev/null)
					if [ -z "${LSNCTRL_CHECK_ERROR}" ]; then
						if [[ "${PLATFORM}" = "aix" ]]; then
							LSNCTRL_VERSION=$(${LISTENER_CTRL_BIN} status ${LISTENER_NAME} 2>/dev/null | ${GREP_BIN} -iE "^Version" 2>/dev/null | ${SED_BIN} -n "s/.*Version \([^ ]*\).*/\1/p" 2>/dev/null)
						else
							LSNCTRL_VERSION=$(${LISTENER_CTRL_BIN} status ${LISTENER_NAME} 2>/dev/null | ${GREP_BIN} -iE "^Version" 2>/dev/null | ${SED_BIN} -r "s/.*Version ([^ ]+).*/\1/" 2>/dev/null)
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
			fi
			RESULT_STR="{\"LSNR_STATUS\":${LSNCTRL_STATUS},\"LSNR_VERSION\":\"${LSNCTRL_VERSION}\",\"LSNR_ERROR\":\"${LSNCTRL_ERROR}\"}"
			_debug_logging "Func: ${FUNCNAME[0]}: RESULT = ${RESULT_STR}"
			${ECHO_BIN} ${RESULT_STR}
		else
			_message "Binary file '${LISTENER_CTRL_BIN}' not found."
		fi
	else
		_message "Binary file '${LISTENER_PATH}' not found."
	fi
}

############################# END ORACLE #############################

################################ MAIN ###############################

RES=""
EXIT_CODE=0

if [[ "$PARAM1" = "listener_discovery" ]]; then
	_logrotate
	if [[ "${DB_TYPE}" = "oracle" ]]; then
		_oracle_listener_discovery
	fi
	RCODE="0"
elif [[ "$PARAM1" = "service_discovery" ]]; then
	if [[ "${DB_TYPE}" = "oracle" ]]; then
		_oracle_service_discovery
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

# End
_logging "============================= MAIN END ================================="

exit ${RCODE}
