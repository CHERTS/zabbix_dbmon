#!/usr/bin/env bash

#
# Program: Automatic run backup via pg_probackup <pg_probackup.sh>
#
# Author: Mikhail Grigorev <sleuthhound at gmail dot com>
# 
# Current Version: 1.0.1
#
# License:
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#

# PostgreSQL version using for PgProBackup
PG_VERSION=13
# Main backup repo directory
PG_PROBACKUP_BACKUP_PATH=/backup
# Mode for incremental backup: DELTA, PAGE or PTRACK
PG_PROBACKUP_INCR_MODE=DELTA
# Enable wal mode STREAM
PG_PROBACKUP_STREAM=1
# Number of threads
PG_PROBACKUP_THREADS=4
# Enable write backup result in json file (using from zabbix-agent)
ZBX_WRITE_BACKUP_RESULT=1
# File path
ZBX_RESULT_DIR="/var/lib/zabbix"

if [ ${PG_PROBACKUP_STREAM} -eq 1 ]; then
	PG_PROBACKUP_STREAM_OPTS="--stream"
else
	PG_PROBACKUP_STREAM_OPTS=""
fi

PG_PROBACKUP_FULL_OPTS="-b FULL -j ${PG_PROBACKUP_THREADS} --delete-expired --delete-wal ${PG_PROBACKUP_STREAM_OPTS}"
PG_PROBACKUP_INCR_OPTS="-b ${PG_PROBACKUP_INCR_MODE} -j ${PG_PROBACKUP_THREADS} --delete-expired ${PG_PROBACKUP_STREAM_OPTS}"

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do
    DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
    SOURCE="$(readlink "$SOURCE")"
    [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
SCRIPT_DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

# trap ctrl-c and call ctrl_c()
trap ctrl_c INT

function ctrl_c() {
	echo "** Trapped CTRL-C"
}

_command_exists() {
	type "$1" &> /dev/null
}

if _command_exists jq ; then
	JQ_BIN=$(which jq)
else
	echo "ERROR: Command 'jq' not found."
	exit 1
fi

if _command_exists pg_probackup-${PG_VERSION} ; then
	PG_PROBACKUP_BIN=$(which pg_probackup-${PG_VERSION})
else
	echo "ERROR: Command 'pg_probackup-${PG_VERSION}' not found."
	exit 1
fi

_usage() {
	echo ""
	echo "Usage: $0 [ -m backup_mode -i instance]"
	echo ""
	echo "  -t backup_mode	: (Required) Backup mode (FULL or INCR)"
	echo "  -i instance		: (Required) Instance name"
	echo ""
	echo "  -h			: Print this screen"
	echo ""
	exit 1
}

while getopts "m:i:" option; do
	case "${option}" in
		m)
			PG_PROBACKUP_MODE=${OPTARG}
			;;
		i)
			PG_PROBACKUP_INCTANCE=${OPTARG}
			;;
		*)
			_usage
			;;
	esac
done

if [ -z "${PG_PROBACKUP_MODE}" ]; then
	echo "No options -m <backup_mode> found!"
	_usage
fi

if [ -z "${PG_PROBACKUP_INCTANCE}" ]; then
     echo "No options -i <instance> found!"
     _usage
fi

if [ ! -f "${PG_PROBACKUP_BIN}" ]; then
	echo "ERROR: Binary file '${PG_PROBACKUP_BIN}' not found."
	exit 1
fi

if [ ! -d "${PG_PROBACKUP_BACKUP_PATH}" ]; then
	echo "ERROR: Backup directory '${PG_PROBACKUP_BACKUP_PATH}' not found."
	exit 1
fi

if [ ${ZBX_WRITE_BACKUP_RESULT} -eq 1 ]; then
	if [ ! -d "${ZBX_RESULT_DIR}" ]; then
		echo "ERROR: Zabbix directory '${ZBX_RESULT_DIR}' not found."
		exit 1
	fi

	ZBX_FULL_BACKUP_RESULT_FILE="${ZBX_RESULT_DIR}/pg_full_backup_${PG_PROBACKUP_INCTANCE}.data"
	ZBX_INCR_BACKUP_RESULT_FILE="${ZBX_RESULT_DIR}/pg_incr_backup_${PG_PROBACKUP_INCTANCE}.data"
fi

_check_backup_result() {
	echo -n "Checking last backup status... "
	PG_LAST_BACKUP_MODE=$(${PG_PROBACKUP_BIN} show -B "${PG_PROBACKUP_BACKUP_PATH}" --instance "${PG_PROBACKUP_INCTANCE}" --format=json 2>/dev/null | ${JQ_BIN} -c '.[].backups[0]."backup-mode"' 2>/dev/null | sed 's/^[ \t\"]*//;s/[ \"]*$//')
	if [ -n "${PG_LAST_BACKUP_MODE}" ]; then
		PG_LAST_BACKUP_STATUS=$(${PG_PROBACKUP_BIN} show -B "${PG_PROBACKUP_BACKUP_PATH}" --instance "${PG_PROBACKUP_INCTANCE}" --format=json 2>/dev/null | ${JQ_BIN} -c '.[].backups[0]' 2>/dev/null)
		if [[ "${PG_LAST_BACKUP_MODE}" == 'FULL' ]]; then
			echo "OK, FULL"
			if [ ${ZBX_WRITE_BACKUP_RESULT} -eq 1 ]; then
				echo "${PG_LAST_BACKUP_STATUS}" 1>"${ZBX_FULL_BACKUP_RESULT_FILE}" 2>/dev/null
			fi
		else
			echo "OK, ${PG_LAST_BACKUP_MODE}"
			if [ ${ZBX_WRITE_BACKUP_RESULT} -eq 1 ]; then
				echo "${PG_LAST_BACKUP_STATUS}" 1>"${ZBX_INCR_BACKUP_RESULT_FILE}" 2>/dev/null
			fi
		fi
	else
		echo "UNKNOWN"
	fi
}

case "${PG_PROBACKUP_MODE}" in
	"FULL")
		echo -n "Running full backup... "
		${PG_PROBACKUP_BIN} backup -B "${PG_PROBACKUP_BACKUP_PATH}" --instance "${PG_PROBACKUP_INCTANCE}" ${PG_PROBACKUP_FULL_OPTS} >/dev/null 2>&1
		PG_EXIT_CODE=$?
		;;
	"INCR")
		echo -n "Running incremental backup... "
		${PG_PROBACKUP_BIN} backup -B "${PG_PROBACKUP_BACKUP_PATH}" --instance "${PG_PROBACKUP_INCTANCE}" ${PG_PROBACKUP_INCR_OPTS} >/dev/null 2>&1
		PG_EXIT_CODE=$?
		;;
	*)
		_usage
		;;
esac

if [ ${PG_EXIT_CODE} -eq 0 ]; then
	echo "OK"
	_check_backup_result
else
	echo "ERROR"
fi

