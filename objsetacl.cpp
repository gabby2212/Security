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

int main(int argc, char *argv[]){
	int opt;
	string username;
	string objname;
	string groupname;
	bool uname;
	bool gname;
	struct sigaction sigIntHandler;

	//Start signal handling to capture crtl+C and ctrl+D
   	sigIntHandler.sa_handler = sig_hadle;
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
	validNameString(username);
	if(argc != 6 || !uname || !gname)
		printError("Usage objsetacl -u username -g groupname objname");

	//Set up file system
	setUp();

	if(!userExists(username))
		printError("Invalid user");
	if(!groupExists(groupname))
		printError("Invalid group");
	User currentUser = users.find(username)->second;
	if(currentUser.hasFile(objname))
		cerr << "Object already exists, overwritting" << endl;
	writeACL(objname, username);

}