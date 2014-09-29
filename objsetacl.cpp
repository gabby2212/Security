using namespace std;
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <ctype.h>
#include <getopt.h>
#include "utilities.cpp"
#include "aclUtils.cpp"
#include <map>
#include <signal.h>

extern map<string, User>  users;
extern list<string> groups;
extern ACL currentACL;

int main(int argc, char *argv[]){
	int opt;
	string username;
	string objname;
	string groupname;
	bool uname;
	bool gname;
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
	if(argc != 6 || !uname || !gname)
		printError("Usage objsetacl -u username -g groupname objname");

	//Validate object
	if(objname.find("+") == string::npos){
		validNameString(objname);
		objname = username + "." + objname;
	}
	else{
		string targetUser = objname.substr(0, objname.find("+"));
		string targetObject = objname.substr(objname.find("+") + 1, objname.length());
		validNameString(targetUser);
		validNameString(targetObject);
		objname = targetUser + "." + targetObject;
	}

	//Set up file system
	setUp();
	initACL();

	if(!userExists(username))
		printError("Invalid user");
	if(!groupExists(groupname))
		printError("Invalid group");
	User currentUser = users.find(username)->second;
	if(testACL(username, groupname, objname, "p"))
		writeACL(objname);
	else
		printError("Permission Denied");writeACL(objname);

}