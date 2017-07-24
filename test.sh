#!/bin/bash
set -e

progname=${1:-./lcs}

function run_test {
	# expected=${1:?}; shift

	result=$($progname $*)

	if [[ $result = ${expected:?} ]]; then
		printf "PASS: '%s %s'=%s\n" "$progname" "$*" "$result" >&2
		return 0
	else
		printf "FAIL: '%s %s'=%s (expected %s)\n" "$progname" "$*" "$result" "$expected" >&2
		return 1
	fi
}

expected='10.0.0.0/24'            run_test    10.0.0.1 10.0.0.255
expected='10.0.0.0/24'            run_test -c 10.0.0.1 10.0.0.255
expected='24'                     run_test -C 10.0.0.1 10.0.0.255
expected='10.0.0.0/255.255.255.0' run_test -m 10.0.0.1 10.0.0.255
expected='255.255.255.0'          run_test -M 10.0.0.1 10.0.0.255
expected='11.24.0.0/16'           run_test 11.24.2.41 11.24.249.61
expected='0.0.0.0/1'              run_test 93.232.14.55 50.56.247.112
