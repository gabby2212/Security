#!/bin/bash
# OBJGETACL
output="$(./objgetacl 2>&1)"
if [ "$output" != "Usage objgetacl -u username -g groupname objname" ]; then
	echo "failed 1"
fi

output="$(./objgetacl -u user -g g1 newfile 2>&1)"
if [ "$output" != "Invalid user" ]; then
	echo failed 2
fi

output="$(./objgetacl -u u1 -g group newfile 2>&1)"
if [ "$output" != "Invalid group" ]; then
	echo failed 3
fi

output="$(./objgetacl -u u1 -g g1 ! 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

output="$(./objgetacl -u u1 -g g1 foo 2>&1)"
if [ "$output" != "Invalid object" ]; then
	echo failed 5
fi

var="$(./objgetacl -u u2 -g g4 fileFor1and2)"
if [ "$var" != "rwxpv u1
rwxpv u2
rwxpv g3
rwxpv g4" ]; then
	echo failed 6
fi

output="$(./objgetacl -u u1 -g g3 u2+fileForOnly2 2>&1)"
if [ "$output" != "Permission denied" ]; then
	echo failed 7
fi