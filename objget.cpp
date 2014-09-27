using namespace std;

#include <cstdio>
#include <cstdlib>
#include <climits>
#include <ctype.h>
#include <getopt.h>
#include "utilities.cpp"
#include <map>
#include <signal.h>

#define BUFFER_SIZE 256

void sig_hadle(int s){
	fileList.close();
	newFile.close();
	exit(0);
}

int main(int argc, char *argv[]){
	int opt;
	string username;
	string objname;
	struct sigaction sigIntHandler;

   	sigIntHandler.sa_handler = sig_hadle;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;

   	sigaction(SIGINT, &sigIntHandler, NULL);

	opt = getopt(argc, argv, "u:");
	if(opt != 'u' || argc != 4)
		printError("Usage objget -u username objname");
	username = optarg;
	objname = argv[3];
	setUp();

	if(!userExists(username))
		printError("Invalid user");
	User currentUser = users.find(username)->second;
	if(objname.length() > FILENAME_MAX)
		printError("Object name too long");
	readFile(objname, username);

}