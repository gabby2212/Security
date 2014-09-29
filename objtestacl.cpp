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
extern ACL currentACL;

int main(int argc, char *argv[]){
	int opt;
	string username;
	string objname;
	string groupname;
	string access;
	string userfile;
	bool uname = false;
	bool gname = false;
	bool ac = false;
	struct sigaction sigIntHandler;

	//Start signal handling to capture crtl+C and ctrl+D
   	sigIntHandler.sa_handler = sig_handle;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;
   	sigaction(SIGINT, &sigIntHandler, NULL);
   	sigaction(SIGTERM, &sigIntHandler, NULL);
   	//Check for valid input params
	while((opt = getopt(argc, argv, "g:u:a:")) != ERROR){
		switch(opt){
			case 'u':
				uname = true;
				username = optarg;
				break;
			case 'g':
				gname = true;
				groupname = optarg;
				break;
			case 'a':
				ac = true;
				access = optarg;
				break;
		}
	}
	if(argc != 9 || !uname || !gname)
		printError("Usage objtestacl -u username -g groupname -a acess objname (userfile)");

	objname = argv[7];
	validNameString(username);
	validNameString(groupname);
	validPermissions(access);
	userfile = argv[8];

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
	setUp(userfile);
	initACL();

	if(!userExists(username))
		printError("Invalid user");
	if(!groupExists(groupname))
		printError("Invalid group");
	if(access.length() != 1)
		printError("Please check one acess at a time");
	User currentUser = users.find(username)->second;

	if(testACL(username, groupname, objname, access))
		cout << "allowed" << endl;
	else
		cout << "denied" << endl;
	return 0;
}