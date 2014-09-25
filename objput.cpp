using namespace std;

#include <cstdio>
#include <cstdlib>
#include <climits>
#include <ctype.h>
#include <getopt.h>
#include "utilities.cpp"
#include <map>
#define BUFFER_SIZE 256

int main(int argc, char *argv[]){
	int opt;
	string username;
	string objname;

	opt = getopt(argc, argv, "u:");
	if(opt != 'u' || argc != 4)
		printError("Usage objput -u username objname");
	username = optarg;
	objname = argv[3];
	setUp();

	if(!userExists(username))
		printError("Invalid user");
	User currentUser = users.find(username)->second;
	if(currentUser.hasFile(objname))
	 	printError("Object already exists");
	if(objname.length() > NAME_MAX)
		printError("Object name too long");

}