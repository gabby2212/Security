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
	string objname;
	struct sigaction sigIntHandler;

	//Start signal handling to capture crtl+C and ctrl+D
   	sigIntHandler.sa_handler = sig_handle;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;
   	sigaction(SIGINT, &sigIntHandler, NULL);
   	sigaction(SIGTERM, &sigIntHandler, NULL);

   	//Check for valid input params
	opt = getopt(argc, argv, "u:");
	if(opt != 'u' || argc != 4)
		printError("Usage objput -u username objname");
	username = optarg;
	objname = argv[3];
	validNameString(username);
	validNameString(objname);

	//Set up file system
	setUp();

	if(!userExists(username))
		printError("Invalid user");
	User currentUser = users.find(username)->second;
	// if(currentUser.hasFile(objname))
	// 	cerr << "Object already exists, overwritting" << endl;
	writeFile(objname, username);

}