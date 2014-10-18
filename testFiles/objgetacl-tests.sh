#!/bin/bash
# OBJGETACL
output="$(./objgetacl 2>&1)"
if [ "$output" != "Usage objgetacl objname" ]; then
	echo "failed 1"
fi

# output="$(./objgetacl newfile 2>&1)"
# if [ "$output" != "Invalid user" ]; then
# 	echo failed 2
# fi

# output="$(./objgetacl newfile 2>&1)"
# if [ "$output" != "Invalid group" ]; then
# 	echo failed 3
# fi

output="$(./objgetacl ! 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

output="$(./objgetacl foo 2>&1)"
if [ "$output" != "Invalid object" ]; then
	echo failed 5
fi

# var="$(./objgetacl fileFor1and2)"
# if [ "$var" != "rwxpv u1
# rwxpv u2
# rwxpv g3
# rwxpv g4" ]; then
# 	echo failed 6
# fi

# output="$(./objgetacl u2+fileForOnly2 2>&1)"
# if [ "$output" != "Permission denied" ]; then
# 	echo failed 7
# fi