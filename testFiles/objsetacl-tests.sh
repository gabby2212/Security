#!/bin/bash
# OBJSETACL
output="$(su gabi -c "./objsetacl" 2>&1)"
if [ "$output" != "Usage objsetacl objname" ]; then
	echo "failed 1"
fi

output="$(su foo "./objsetacl existingFile" 2>&1)"
if [ "$output" != "No passwd entry for user 'foo'" ]; then	
	echo failed 2
fi

output="$(su gabi:foo -c "./objsetacl existingFile" 2>&1)"
if [ "$output" != "No passwd entry for user 'gabi:foo'" ]; then	
	echo failed 3
fi

output="$(su gabi -c "./objsetacl .\*" 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

var="$(su gabi -c "./objput existingFile < testFiles/test.txt")"
if [ "$var" != "" ]; then
	echo failed 5
fi

var="$(su fsManager -c "./objsetacl gabi+existingFile < testFiles/testFileForOnly2.txt")"
if [ "$var" != "" ]; then
	echo failed 6
fi

output="$(su gabi -c "./objput fsManager+newfile < testFiles/test.txt" 2>&1)"
if [ "$output" != "Cannot create file for another user" ]; then
	echo failed 7
fi

output="$(su gabi -c "./objsetacl newfile < testFiles/testFileForOnly1.txt" 2>&1)"
if [ "$var" != "" ]; then
	echo failed 8
fi

output="$(su gabi -c "./objsetacl existingFile < testFiles/testFileFor1and2.txt" 2>&1)"
if [ "$output" != "Permission denied" ]; then
	echo failed 9
fi

var="$(su fsManager -c "./objput fileForOnly2 < testFiles/test.txt")"
if [ "$var" != "" ]; then
	echo failed 10
fi

var="$(su fsManager -c "./objsetacl fileForOnly2 < testFiles/testFileForOnly2.txt")"
if [ "$var" != "" ]; then
	echo failed 11
fi

output="$(su gabi -c "./objsetacl fsManager+fileForOnly2 < testFiles/testFileForOnly2.txt" 2>&1)"
if [ "$output" != "Permission denied" ]; then
	echo failed 12
fi

var="$(su fsManager -c "./objput fileFor1and2 < testFiles/test.txt")"
if [ "$var" != "" ]; then
	echo failed 13
fi

var="$(su fsManager -c "./objsetacl fileFor1and2 < testFiles/testFileFor1and2.txt")"
if [ "$var" != "" ]; then
	echo failed 14
fi

var="$(su gabi -c "./objsetacl fsManager+fileFor1and2 < testFiles/testFileFor1and2.txt"
)"
if [ "$var" != "" ]; then
	echo failed 15
fi

var="$(su gabi -c "./objput fileForGroup1Only < testFiles/test.txt")"
if [ "$var" != "" ]; then
	echo failed 16
fi

var="$(su gabi -c "./objsetacl fileForGroup1Only < testFiles/testFileForGroup1.txt"
)"
if [ "$var" != "" ]; then
	echo failed 17
fi

var="$(su fsManager -c "./objsetacl gabi+fileForGroup1Only < testFiles/testFileForGroup1.txt"
)"
if [ "$var" != "" ]; then
	echo failed 19
fi
