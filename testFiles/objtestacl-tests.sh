#!/bin/bash
# OBJTESTACL
output="$(./objtestacl 2>&1)"
if [ "$output" != "Usage objtestacl -a access objname" ]; then
	echo "failed 1"
fi

output="$(./objtestacl -a r \;. 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

# var="$(./objtestacl -a r u2+fileForOnly2)"
# if [ "$var" != "denied" ]; then
# 	echo failed 5
# fi

# var="$(./objtestacl -a r fileForGroup1Only)"
# if [ "$var" != "allowed" ]; then
# 	echo failed 6
# fi