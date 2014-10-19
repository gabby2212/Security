#!/bin/bash
output="$(./objget 2>&1)"
if [ "$output" != "Usage objget objname" ]; then
	echo "failed 1"
fi

output="$(su foo -c "./objget newfile" 2>&1)"
if [ "$output" != "No passwd entry for user 'foo'" ]; then
	echo failed 2
fi

output="$(su gabi:foo -c "./objget newfile" 2>&1)"
if [ "$output" != "No passwd entry for user 'gabi:foo'" ]; then
	echo failed 3
fi

output="$(./objget @# 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

var="$(su gabi -c "./objget newfile")"
if [ "$var" != "this is a test file" ]; then
	echo failed 5
fi

output="$(su gabi -c "./objget existingFile" 2>&1 )"
if [ "$output" != "Invalid object" ]; then
	echo failed 6
fi

output="$(su fsManager -c "./objget gabi+newfile" 2>&1)"
if [ "$output" != "Permission denied" ]; then
	echo failed 9
fi