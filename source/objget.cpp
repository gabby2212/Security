using namespace std;
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <ctype.h>
#include <getopt.h>
#include "fileSystem.cpp"
#include <map>
#include <signal.h>

extern map<string, vector<string>>  users;
extern ACL acl;

class Objget :public FileSystem{
private:
	string username;
	string groupname;
	string objectname;
public:
	Objget(string objname){
		username = FileSystem::username;
		groupname = FileSystem::groupname;
		objectname = getObjectName(username, objname);
	}
	void readFile(){
  		char *line;
		streampos size;
		ifstream file;
		
		if(!acl.testACL(username, groupname, objectname, "r"))
			printError("Permission denied");

		file.open(("/fileSystem/" + objectname).c_str(), ios::in|ios::binary|ios::ate);
		if(file.is_open())
		{
			size = file.tellg();
		    line = new char [size];
		    file.seekg (0, ios::beg);
		    file.read (line, size);
			cout.write(line, size);
			delete line;
		    file.close();
		}
		else
			printError("Couldn't open file");
	}
};

int main(int argc, char *argv[]){
	int opt;
	struct sigaction sigIntHandler;
	string objname;

	//Start signal handling to capture crtl+C and ctrl+D
   	sigIntHandler.sa_handler = sig_handle;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;
   	sigaction(SIGINT, &sigIntHandler, NULL);
   	sigaction(SIGTERM, &sigIntHandler, NULL);

	if(argc != 2)
		printError("Usage objget objname");
	objname = argv[1];

	Objget *getObj = new Objget(objname);
	getObj->readFile();
	return 0;
}