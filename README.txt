Gabriela Melchior gdm2118

Choice of uid:
I chose to create a standard user called fsManager, which belongs only to the fsManager group. The fsManager is in charge of all file system operations and owns all of the file system's folders.
I chose to use a non-root user for 2 main reasons:

1. The file system does not have to be run as root. Thus using root for it would be breaking the principle of least privilege.
2. If the fsManager account is compromised it won't automatically give the attacker root capabilities.

I am using the system's user and groups, but I kept the user.cpp object in order to avoid system calls. The users and groups are loaded at startup and from there on the user object is used.

Passwords for the TA's:
tinyvm root123
gabi user123
fsManager manager

other users don't have passwords.


setUp.sh:
Assumptions -
The set up script assumes there exists a user called fsManager and a group called fsManager. The user fsManager should only belong to the fsManager group.

The script/Makefile also assumes that there exists a folder called source which is protected (owner fsManager, group fsManager permissions 700) and contains all of the source code.

New Folders/Files -
The script creates a config folder which contains/will contain the file system's configuration files (userfile.txt and aclFile).

The script creates a fileSystem folder which contains/will contain all of the user's files.

The script makes fsManager:fsManager the owner of all executable files.

Permissions -
config and fileSystem have permissions 700, so that the fsManager can read, write and execute everything in the system, but no one else can.

The script sets the permissions of all executables (obj*) to 4101, so that any user can execute the files and the setuid bit is set.

Users -
Setup.sh also configures the userfile.txt, if a userfile is given on make exec the script will use that file, otherwise it will generate a userfile from all the users and groups in the machine.
The script will then create the users specified in userfile and add them to their specified groups. If the user already exists and/or is already in group these commands will have no effect.


ACL notes:
When the acl is changed the program saves it to config/aclFile to keep track of files and permissions. If no aclFile is present the program assumes the file system is empty and creates a new aclFile

From user's perspective files are called by their given names.
From system's perspective files are called username.filename

Running make test deletes the current version of the filesystem
