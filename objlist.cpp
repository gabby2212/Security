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
	bool meta = false;
	bool uname = false;

   	sigIntHandler.sa_handler = sig_hadle;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;

   	sigaction(SIGINT, &sigIntHandler, NULL);

	while((opt = getopt(argc, argv, "lu:")) != -1){
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
	if(!uname || (!meta && argc != 3) || (meta && argc != 4))
		printError("Usage objlist -u username objname");
	setUp();

	if(!userExists(username))
		printError("Invalid user");
	User currentUser = users.find(username)->second;
	printFiles(username, meta);

}