#!/bin/bash
# OBJGETACL
output="$(su u1 -c "./objgetacl" 2>&1)"
if [ "$output" != "Usage objgetacl objname" ]; then
	echo failed 1
fi

output="$(su foo -c "./objgetacl newfile" 2>&1)"
if [ "$output" != "No passwd entry for user 'foo'" ]; then	
	echo failed 2
fi

output="$(su u1:foo -c "./objgetacl newfile" 2>&1)"
if [ "$output" != "No passwd entry for user 'u1:foo'" ]; then	
	echo failed 3
fi

output="$(su u1 -c "./objgetacl !" 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

output="$(su u1 -c "./objgetacl foo" 2>&1)"
if [ "$output" != "Couldn't find object" ]; then
	echo failed 5
fi

var="$(su u2 -c "./objgetacl fileFor1and2")"
if [ "$var" != "u1.g3 => rwxpv
u2.g4 => rwxpv" ]; then
	echo failed 6
fi

output="$(su u1 -c "./objgetacl u2+fileForOnly2" 2>&1)"
if [ "$output" != "Permission denied" ]; then
	echo failed 7
fi
