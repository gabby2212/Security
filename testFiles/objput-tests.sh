#!/bin/bash
#OBJPUT
output="$(./objput 2>&1)"
if [ "$output" != "Usage objput -k passphrase objname" ]; then
	echo "failed 1"
fi

output="$(su foo -c "./objput -k hello newfile" 2>&1)"
if [ "$output" != "No passwd entry for user 'foo'" ]; then
	echo failed 2
fi

output="$(su u1:foo -c "./objput -k hello newfile" 2>&1)"
if [ "$output" != "No passwd entry for user 'u1:foo'" ]; then
	echo failed 3
fi

output="$(./objput -k hello {} 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

var="$(su u1 -c "./objput -k  hello newfile < testFiles/test.txt")"
if [ "$var" != "" ]; then
	echo failed 5
fi

output="$(su u1 -c "./objput -k hello newfile < testFiles/test.txt" 2>&1 )"
if [ "$output" != "Object already exists, overwritting" ]; then
	echo failed 6
fi

output="$(su u1 -c "./objput -k pass u2+newfile < testFiles/test.txt" 2>&1)"
if [ "$output" != "Cannot create file for another user" ]; then
	echo failed 7
fi

output="$(su u1 -c "./objsetacl newfile < testFiles/testFileForOnly1.txt" 2>&1)"
if [ "$var" != "" ]; then
	echo failed 8
fi

output="$(su u2 -c "./objput -k hello u1+newfile < testFiles/test.txt" 2>&1)"
if [ "$output" != "Permission denied" ]; then
	echo failed 9
fi

var="$(su u2 -c "./objput -k long newfile11111111111111111111111111111111111111111111111111111111111111111 < testFiles/test.txt")"
if [ "$var" != "" ]; then
	echo failed 10
fi

var="$(su u2 -c "./objput -k binary newBinary < testFiles/testBinary")"
if [ "$var" != "" ]; then
	echo failed 11
fi