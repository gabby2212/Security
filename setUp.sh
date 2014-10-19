#!/bin/bash
mkdir -p fileSystem
mkdir -p config
chown fsManager:fsManager fileSystem
chown fsManager:fsManager config
chown -c fsManager:fsManager obj*
chmod 700 fileSystem
chmod 700 config
chmod 4101 obj*

if [ $# == 2 ] ; then
	> ./config/userfile.txt
	cat $1 > ./config/userfile.txt
else
	> ./config/userfile.txt
	getent passwd | awk -F: '{print $1}' | while read name; do groups $name; done > ./config/userfile.txt
fi

cat config/userfile.txt | 
while read name; do awk -F: '{
	gsub(" ", ",", $2);
	groups = substr($2,2);
	system("useradd " $1);
	system("usermod -a -G " groups " " $1);
}'; done 2> /dev/null