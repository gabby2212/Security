#!/bin/bash
#OBJLIST

output="$(su foo -c "./objlist" 2>&1)"
if [ "$output" != "No passwd entry for user 'foo'" ]; then	
	echo failed 1
fi

var="$(su gabi -c "./objlist")"
if [ "$var" != "newfile" ]; then
	echo failed 1
fi

var="$(su gabi -c "./objlist -l")"
if [ "$var" != "20 newfile" ]; then
	echo failed 3
fi