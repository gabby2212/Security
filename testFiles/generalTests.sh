#!/bin/bash
output="$(su u1 -c "ls fileSystem/" 2>&1)"
if [ "$output" != "ls: cannot open directory fileSystem/: Permission denied" ]; then
	echo failed 1
fi

output="$(su u1 -c "cat config/userfile.txt" 2>&1)"
if [ "$output" != "cat: config/userfile.txt: Permission denied" ]; then
	echo failed 2
fi

output="$(su u1 -c "mv myuserfile.txt config/userfile.txt" 2>&1)"
if [ "$output" != "mv: accessing config/userfile.txt: Permission denied" ]; then
	echo failed 3
fi

output="$(su u1 -c "rm -rf config" 2>&1)"
if [ "$output" != "rm: cannot remove config: Permission denied" ]; then
	echo failed 4
fi