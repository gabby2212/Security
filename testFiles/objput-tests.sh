#!/bin/bash
#OBJPUT
output="$(./objput 2>&1)"
if [ "$output" != "Usage objput objname" ]; then
	echo "failed 1"
fi

output="$(su foo -c "./objput newfile" 2>&1)"
if [ "$output" != "No passwd entry for user 'foo'" ]; then
	echo failed 2
fi

output="$(su u1:foo -c "./objput newfile" 2>&1)"
if [ "$output" != "No passwd entry for user 'u1:foo'" ]; then
	echo failed 3
fi

output="$(./objput {} 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

var="$(su u1 -c "./objput newfile < testFiles/test.txt")"
if [ "$var" != "" ]; then
	echo failed 5
fi

output="$(su u1 -c "./objput newfile < testFiles/test.txt" 2>&1 )"
if [ "$output" != "Object already exists, overwritting" ]; then
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

output="$(su u2 -c "./objput u1+newfile < testFiles/test.txt" 2>&1)"
if [ "$output" != "Permission denied" ]; then
	echo failed 9
fi

var="$(su u2 -c "./objput newfile11111111111111111111111111111111111111111111111111111111111111111 < testFiles/test.txt")"
if [ "$var" != "" ]; then
	echo failed 10
fi

var="$(su u2 -c "./objput newBinary < testFiles/testBinary")"
if [ "$var" != "" ]; then
	echo failed 11
fi