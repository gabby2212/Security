#!/bin/bash
# OBJTESTACL
output="$(su u1 -c "./objtestacl" 2>&1)"
if [ "$output" != "Usage objtestacl -a access objname" ]; then
	echo "failed 1"
fi

output="$(su foo -c "./objtestacl -a r newfile" 2>&1)"
if [ "$output" != "No passwd entry for user 'foo'" ]; then	
	echo failed 2
fi

output="$(su u1:foo -c "./objtestacl -a r newfile" 2>&1)"
if [ "$output" != "No passwd entry for user 'u1:foo'" ]; then	
	echo failed 3
fi

output="$(su u1 -c "./objtestacl -a r \;." 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

var="$(su u1 -c "./objtestacl -a r u2+fileForOnly2")"
if [ "$var" != "denied" ]; then
	echo failed 5
fi

var="$(su u1 -c "./objtestacl -a r fileForGroup1Only")"
if [ "$var" != "allowed" ]; then
	echo failed 6
fi