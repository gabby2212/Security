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

class Objgetacl :public FileSystem{
private:
	string username;
	string groupname;
	string objectname;
public:
	Objgetacl(string objname){
		username = FileSystem::username;
		groupname = FileSystem::groupname;
		objectname = getObjectName(username, objname);
	}

	void getACL(){
		ACLEntry a;
		if(!acl.testACL(username, groupname, objectname, "v"))
			printError("Permission denied");
		if(acl.ace.find(objectname) == acl.ace.end())
			printError("Invalid object");
		a = acl.ace.find(objectname)->second;
		a.printACLEntry();
	}
};

int main(int argc, char *argv[]){
	int opt;
	string objname;
	struct sigaction sigIntHandler;

	//Start signal handling to capture crtl+C and ctrl+D
   	sigIntHandler.sa_handler = sig_handle;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;
   	sigaction(SIGINT, &sigIntHandler, NULL);
   	sigaction(SIGTERM, &sigIntHandler, NULL);

	if(argc != 2)
		printError("Usage objgetacl objname");
	objname = argv[1];

	Objgetacl *getaclObj = new Objgetacl(objname);
	getaclObj->getACL();
	return 0;

}