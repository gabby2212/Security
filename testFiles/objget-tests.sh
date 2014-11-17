#!/bin/bash
output="$(./objget 2>&1)"
if [ "$output" != "Usage objget -k passphrase objname" ]; then
	echo "failed 1"
fi

output="$(su foo -c "./objget -k hello newfile" 2>&1)"
if [ "$output" != "No passwd entry for user 'foo'" ]; then
	echo failed 2
fi

output="$(su u1:foo -c "./objget -k hello newfile" 2>&1)"
if [ "$output" != "No passwd entry for user 'u1:foo'" ]; then
	echo failed 3
fi

output="$(./objget -k tests @# 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

var="$(su u1 -c "./objget -k hello newfile")"
if [ "$var" != "this is a test file" ]; then
	echo failed 5
fi

output="$(su u1 -c "./objget -k pass existingFile" 2>&1 )"
if [ "$output" != "Couldn't find object" ]; then
	echo failed 6
fi

output="$(su u2 -c "./objget -k hello u1+newfile" 2>&1)"
if [ "$output" != "Permission denied" ]; then
	echo failed 9
fi

var="$(su u2 -c "./objget -k binary newBinary | diff testFiles/testBinary -")"
if [ "$var" != "" ]; then
	echo failed 10
fi
