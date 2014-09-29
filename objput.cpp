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
extern list<string> groups;
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
		printError("Usage objput -u username -g groupname objname (userfile)");
	validNameString(username);
	validNameString(objname);
	userfile = argv[6];

	//Set up file system
	setUp(userfile);
	initACL();

	if(!userExists(username))
		printError("Invalid user");
	User currentUser = users.find(username)->second;
	if(currentUser.hasFile(objname))
	 	cerr << "Object already exists, overwritting" << endl;
	writeFile(objname, username);
	return 0;

}