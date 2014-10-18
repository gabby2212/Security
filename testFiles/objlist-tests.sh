#!/bin/bash
#OBJLIST
# output="$(./objlist 2>&1)"
# if [ "$output" != "Invalid user" ]; then
# 	echo failed 2l
# fi

var="$(./objlist)"
if [ "$var" != "newfile" ]; then
	echo failed 3
fi

var="$(./objlist -l)"
if [ "$var" != "20 newfile" ]; then
	echo failed 4
fi