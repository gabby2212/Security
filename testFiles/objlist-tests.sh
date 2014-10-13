#!/bin/bash
#OBJLIST
output="$(./objlist 2>&1)"
if [ "$output" != "Usage objlist -u username (-l)" ]; then
	echo "failed 1"
fi

output="$(./objlist -u user 2>&1)"
if [ "$output" != "Invalid User" ]; then
	echo failed 2
fi

var="$(./objlist -u u1)"
if [ "$var" != "newfile" ]; then
	echo failed 3
fi

var="$(./objlist -u u1 -l)"
if [ "$var" != "20 newfile" ]; then
	echo failed 4
fi