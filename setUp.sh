#!/bin/bash
mkdir -p fileSystem
mkdir -p config
chown fsManager:fsManager fileSystem
chown fsManager:fsManager config
chmod 700 fileSystem
chmod 700 config
if [ $# == 2 ] ; then
	> ./config/userfile.txt
	cat $1 > ./config/userfile.txt
else
	> ./config/userfile.txt
	getent passwd | awk -F: '{print $1}' | while read name; do groups $name; done > ./config/userfile.txt
fi
chown -c fsManager:fsManager obj*
chmod 4101 obj*