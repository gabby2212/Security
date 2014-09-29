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
	string objname;
	string userfile;
	struct sigaction sigIntHandler;
	bool meta = false;
	bool uname = false;

   	//Start signal handling to capture crtl+C and ctrl+D
   	sigIntHandler.sa_handler = sig_handle;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;
   	sigaction(SIGINT, &sigIntHandler, NULL);
   	sigaction(SIGTERM, &sigIntHandler, NULL);

   	//Check for valid input params
	while((opt = getopt(argc, argv, "lu:")) != ERROR){
		switch(opt){
			case 'u':
				uname = true;
				username = optarg;
				break;
			case 'l':
				meta = true;
				break;
		}
	}
	if(!uname || (!meta && argc != 4) || (meta && argc != 5))
		printError("Usage objlist -u username (-l) (userfile)");
	validNameString(username);
	if(!meta)
		userfile = argv[3];
	else
		userfile = argv[4];
	//Set up file system
	setUp(userfile);

	if(!userExists(username))
		printError("Invalid user");
	printFiles(username, meta);
	return 0;
}