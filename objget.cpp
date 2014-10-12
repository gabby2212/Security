using namespace std;
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <ctype.h>
#include <getopt.h>
#include "fileSystem.cpp"
#include <map>
#include <signal.h>

extern map<string, User>  users;
extern list<string> groups;
extern ACL acl;
class Objget :public FileSystem{
private:
	string username;
	string groupname;
	string objectname;
public:
	Objget(string uname, string gname, string objname){
		validateUserAndGroup(uname, gname);
		objectname = getObjectName(uname, objname);
		username = uname;
		groupname = gname;
	}
	void readFile(){
		string line;
		ifstream file;
		if(!acl.testACL(username, groupname, objectname, "r"))
			printError("Permission denied");

		file.open(objectname.c_str());
		if(file.is_open())
		{
			while(getline(file, line))
				cout << line << endl;
			file.close();	
		}
		else
			printError("Couldn't open file");
	}
};

int main(int argc, char *argv[]){
	int opt;
	string username;
	string groupname;
	string objname;
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
	if(!uname || !gname || argc != 6)
		printError("Usage objget -u username -g groupname objname");
	objname = argv[5];

	Objget *getObj = new Objget(username, groupname, objname);
	getObj->readFile();
	return 0;
}