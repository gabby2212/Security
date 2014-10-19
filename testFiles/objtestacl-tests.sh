#!/bin/bash
# OBJTESTACL
output="$(su gabi -c "./objtestacl" 2>&1)"
if [ "$output" != "Usage objtestacl -a access objname" ]; then
	echo "failed 1"
fi

output="$(su foo -c "./objtestacl -a r newfile" 2>&1)"
if [ "$output" != "No passwd entry for user 'foo'" ]; then	
	echo failed 2
fi

output="$(su gabi:foo -c "./objtestacl -a r newfile" 2>&1)"
if [ "$output" != "No passwd entry for user 'gabi:foo'" ]; then	
	echo failed 3
fi

output="$(su gabi -c "./objtestacl -a r \;." 2>&1)"
if [ "$output" != "Invalid characters" ]; then
	echo failed 4
fi

var="$(su gabi -c "./objtestacl -a r fsManager+fileForOnly2")"
if [ "$var" != "denied" ]; then
	echo failed 5
fi

var="$(su gabi -c "./objtestacl -a r fileForGroup1Only")"
if [ "$var" != "allowed" ]; then
	echo failed 6
fi