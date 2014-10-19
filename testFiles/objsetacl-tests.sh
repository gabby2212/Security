#!/bin/bash
# OBJSETACL
output="$(su u1 -c "./objsetacl" 2>&1)"
if [ "$output" != "Usage objsetacl objname" ]; then
	echo "failed 1"
fi

output="$(su foo "./objsetacl existingFile" 2>&1)"
if [ "$output" != "No passwd entry for user 'foo'" ]; then	
	echo failed 2
fi

output="$(su u1:foo -c "./objsetacl existingFile" 2>&1)"
if [ "$output" != "No passwd entry for user 'u1:foo'" ]; then	
	echo failed 3
fi

output="$(su u1 -c "./objsetacl .\*" 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

var="$(su u1 -c "./objput existingFile < testFiles/test.txt")"
if [ "$var" != "" ]; then
	echo failed 5
fi

var="$(su u2 -c "./objsetacl u1+existingFile < testFiles/testFileForOnly2.txt")"
if [ "$var" != "" ]; then
	echo failed 6
fi

output="$(su u1 -c "./objput u2+newfile < testFiles/test.txt" 2>&1)"
if [ "$output" != "Cannot create file for another user" ]; then
	echo failed 7
fi

output="$(su u1 -c "./objsetacl newfile < testFiles/testFileForOnly1.txt" 2>&1)"
if [ "$var" != "" ]; then
	echo failed 8
fi

output="$(su u1 -c "./objsetacl existingFile < testFiles/testFileFor1and2.txt" 2>&1)"
if [ "$output" != "Permission denied" ]; then
	echo failed 9
fi

var="$(su u2 -c "./objput fileForOnly2 < testFiles/test.txt")"
if [ "$var" != "" ]; then
	echo failed 10
fi

var="$(su u2 -c "./objsetacl fileForOnly2 < testFiles/testFileForOnly2.txt")"
if [ "$var" != "" ]; then
	echo failed 11
fi

output="$(su u1 -c "./objsetacl u2+fileForOnly2 < testFiles/testFileForOnly2.txt" 2>&1)"
if [ "$output" != "Permission denied" ]; then
	echo failed 12
fi

var="$(su u2 -c "./objput fileFor1and2 < testFiles/test.txt")"
if [ "$var" != "" ]; then
	echo failed 13
fi

var="$(su u2 -c "./objsetacl fileFor1and2 < testFiles/testFileFor1and2.txt")"
if [ "$var" != "" ]; then
	echo failed 14
fi

var="$(su u1 -c "./objsetacl u2+fileFor1and2 < testFiles/testFileFor1and2.txt"
)"
if [ "$var" != "" ]; then
	echo failed 15
fi

var="$(su u1 -c "./objput fileForGroup1Only < testFiles/test.txt")"
if [ "$var" != "" ]; then
	echo failed 16
fi

var="$(su u1 -c "./objsetacl fileForGroup1Only < testFiles/testFileForGroup1.txt"
)"
if [ "$var" != "" ]; then
	echo failed 17
fi

var="$(su u2 -c "./objsetacl u1+fileForGroup1Only < testFiles/testFileForGroup1.txt"
)"
if [ "$var" != "" ]; then
	echo failed 19
fi
