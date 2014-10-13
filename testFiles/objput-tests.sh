#!/bin/bash
#OBJPUT
output="$(./objput 2>&1)"
if [ "$output" != "Usage objput -u username -g groupname objname" ]; then
	echo "failed 1"
fi

output="$(./objput -u user -g g1 newfile 2>&1)"
if [ "$output" != "Invalid user" ]; then
	echo failed 2
fi

output="$(./objput -u u1 -g group newfile 2>&1)"
if [ "$output" != "Invalid group" ]; then
	echo failed 3
fi

output="$(./objput -u u1 -g g1 {} 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

var="$(./objput -u u1 -g g1 newfile < testFiles/test.txt)"
if [ "$var" != "" ]; then
	echo failed 5
fi

output="$(./objput -u u1 -g g1 newfile < testFiles/test.txt 2>&1 )"
if [ "$output" != "Object already exists, overwritting" ]; then
	echo failed 6
fi

output="$(./objput -u u1 -g g1 u2+newfile < testFiles/test.txt 2>&1)"
if [ "$output" != "Cannot create file for another user" ]; then
	echo failed 7
fi

output="$(./objsetacl -u u1 -g g3 newfile < testFiles/testFileForOnly1.txt 2>&1)"
if [ "$var" != "" ]; then
	echo failed 8
fi

output="$(./objput -u u2 -g g4 u1+newfile < testFiles/test.txt 2>&1)"
if [ "$output" != "Permission denied" ]; then
	echo failed 9
fi

# OBJTESTACL
# ./objtestacl
# Usage objtestacl -u username -g groupname -a access objname

# ./objtestacl -u user -g g1 -a r newfile
# Invalid user

# ./objtestacl -u u1 -g group -a r newfile
# Invalid group

# ./objtestacl -u u1 -g g1 -a r {}
# Invalid characters

# ./objtestacl -u u1 -g g1 -a r u2+fileForOnly2
# denied

# ./objtestacl -u u1 -g g1 -a r fileForGroup1Only
# allowed

