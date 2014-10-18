#!/bin/bash
output="$(./objget 2>&1)"
if [ "$output" != "Usage objget objname" ]; then
	echo "failed 1"
fi

output="$(./objget @# 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

var="$(./objget newfile)"
if [ "$var" != "this is a test file" ]; then
	echo failed 5
fi

output="$(./objget existingFile 2>&1 )"
if [ "$output" != "Invalid object" ]; then
	echo failed 6
fi

# output="$(./objput -u u1 -g g1 u2+newfile < testFiles/test.txt 2>&1)"
# if [ "$output" != "Cannot create file for another user" ]; then
# 	echo failed 7
# fi

# var="$(./objsetacl -u u1 -g g3 newfile < testFiles/testFileForOnly1.txt)"
# if [ "$var" != "" ]; then
# 	echo failed 8
# fi

# output="$(./objget -u u2 -g g4 u1+newfile 2>&1)"
# if [ "$output" != "Permission denied" ]; then
# 	echo failed 9
# fi