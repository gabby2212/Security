#!/bin/bash
# OBJTESTACL
output="$(./objtestacl 2>&1)"
if [ "$output" != "Usage objtestacl -u username -g groupname -a access objname" ]; then
	echo "failed 1"
fi

output="$(./objtestacl -u user -g g1 -a r newfile 2>&1)"
if [ "$output" != "Invalid user" ]; then
	echo failed 2
fi

output="$(./objtestacl -u u1 -g group -a r newfile 2>&1)"
if [ "$output" != "Invalid group" ]; then
	echo failed 3
fi

output="$(./objtestacl -u u1 -g g1 -a r \;. 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

var="$(./objtestacl -u u1 -g g1 -a r u2+fileForOnly2)"
if [ "$var" != "denied" ]; then
	echo failed 5
fi

var="$(./objtestacl -u u1 -g g1 -a r fileForGroup1Only)"
if [ "$var" != "allowed" ]; then
	echo failed 6
fi