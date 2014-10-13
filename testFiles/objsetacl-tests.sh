#!/bin/bash
# OBJSETACL
output="$(./objsetacl 2>&1)"
if [ "$output" != "Usage objsetacl -u username -g groupname objname" ]; then
	echo "failed 1"
fi

output="$(./objsetacl -u user -g g1 existingFile 2>&1)"
if [ "$output" != "Invalid user" ]; then
	echo failed 2
fi

output="$(./objsetacl -u u1 -g group existingFile 2>&1)"
if [ "$output" != "Invalid group" ]; then
	echo failed 3
fi

output="$(./objsetacl -u u1 -g g1 .\* 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

var="$(./objput -u u1 -g g1 existingFile < testFiles/test.txt)"
if [ "$var" != "" ]; then
	echo failed 5
fi

var="$(./objsetacl -u u2 -g g4 u1+existingFile < testFiles/testFileForOnly2.txt)"
if [ "$var" != "" ]; then
	echo failed 6
fi

output="$(./objsetacl -u u1 -g g4 existingFile < testFiles/testFileFor1and2.txt 2>&1 )"
if [ "$output" != "User not in group" ]; then
	echo failed 7
fi

output="$(./objput -u u1 -g g1 u2+newfile < testFiles/test.txt 2>&1)"
if [ "$output" != "Cannot create file for another user" ]; then
	echo failed 8
fi

output="$(./objsetacl -u u1 -g g3 newfile < testFiles/testFileForOnly1.txt 2>&1)"
if [ "$var" != "" ]; then
	echo failed 9
fi

output="$(./objsetacl -u u1 -g g3 existingFile < testFiles/testFileFor1and2.txt 2>&1)"
if [ "$output" != "Permission denied" ]; then
	echo failed 10
fi

var="$(./objput -u u2 -g g4 fileForOnly2 < testFiles/test.txt)"
if [ "$var" != "" ]; then
	echo failed 11
fi

var="$(./objsetacl -u u2 -g g4 fileForOnly2 < testFiles/testFileForOnly2.txt)"
if [ "$var" != "" ]; then
	echo failed 12
fi

output="$(./objsetacl -u u1 -g g1 u2+fileForOnly2 < testFiles/testFileForOnly2.txt 2>&1)"
if [ "$output" != "Permission denied" ]; then
	echo failed 13
fi

var="$(./objput -u u2 -g g4 fileFor1and2 < testFiles/test.txt)"
if [ "$var" != "" ]; then
	echo failed 14
fi

var="$(./objsetacl -u u2 -g g4 fileFor1and2 < testFiles/testFileFor1and2.txt
)"
if [ "$var" != "" ]; then
	echo failed 15
fi

var="$(./objsetacl -u u1 -g g1 u2+fileFor1and2 < testFiles/testFileFor1and2.txt
)"
if [ "$var" != "" ]; then
	echo failed 16
fi

var="$(./objput -u u1 -g g3 fileForGroup1Only < testFiles/test.txt)"
if [ "$var" != "" ]; then
	echo failed 17
fi

var="$(./objsetacl -u u1 -g g1 fileForGroup1Only < testFiles/testFileForGroup1.txt
)"
if [ "$var" != "" ]; then
	echo failed 18
fi

output="$(./objsetacl -u u2 -g g4 u1+fileForGroup1Only < testFiles/testFileForGroup1.txt  2>&1)"
if [ "$output" != "Permission denied" ]; then
	echo failed 19
fi

var="$(./objsetacl -u u2 -g g1 u1+fileForGroup1Only < testFiles/testFileForGroup1.txt
)"
if [ "$var" != "" ]; then
	echo failed 20
fi