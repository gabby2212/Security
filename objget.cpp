using namespace std;

#include <cstdio>
#include <cstdlib>
#include <climits>
#include <ctype.h>
#include <getopt.h>
#include "utilities.cpp"
#include <map>
#include <signal.h>

extern map<string, User>  users;

int main(int argc, char *argv[]){
	int opt;
	string username;
	string groupname;
	string objname;
	string userfile;
	bool uname = false;
	bool gname = false;
	struct sigaction sigIntHandler;

	//Start signal handling to capture crtl+C and ctrl+D
   	sigIntHandler.sa_handler = sig_handle;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;
   	sigaction(SIGINT, &sigIntHandler, NULL);
   	sigaction(SIGTERM, &sigIntHandler, NULL);

   	//Check for valid input params
	//Check for valid input params
	while((opt = getopt(argc, argv, "g:u:")) != ERROR){
		switch(opt){
			case 'u':
				uname = true;
				username = optarg;
				break;
			case 'g':
				gname = true;
				groupname = optarg;
				break;
		}
	}
	objname = argv[5];
	validNameString(username);
	validNameString(groupname);	
	if(!uname || !gname || argc != 7)
		printError("Usage objget -u username -g groupname objname (usefile)");
	validNameString(objname);
	string aclObjname = username + "." + objname;
	userfile = argv[6];

	//Set up file system
	setUp(userfile);
	initACL();

	if(!userExists(username))
		printError("Invalid user");
	if(testACL(username, groupname, aclObjname, "r"))
		readFile(objname, username);
	else
		printError("Permission denied");
	return 0;
}