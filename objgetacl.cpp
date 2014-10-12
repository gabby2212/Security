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

class Objgetacl :public FileSystem{
private:
	string username;
	string groupname;
	string objectname;
public:
	Objgetacl(string uname, string gname, string objname){
		validateUserAndGroup(uname, gname);
		objectname = getObjectName(uname, objname);
		username = uname;
		groupname = gname;
	}

	void getACL(){
		if(!acl.testACL(username, groupname, objectname, "v"))
			printError("Permission denied");
		readACL();
	}

	void readACL(){
		ACLEntry a;
		if(acl.ace.find(objectname) == acl.ace.end())
			printError("Invalid object");
		a = acl.ace.find(objectname)->second;
		for (map<string,string>::iterator it = a.userPermissions.begin(); it != a.userPermissions.end(); ++it)
	    	cout << it->second << " " << it->first << endl;
		for (map<string,string>::iterator it2 = a.groupPermissions.begin(); it2 != a.groupPermissions.end(); ++it2)
			cout << it2->second << " " << it2->first << endl;
	}
};

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
	if(argc != 6 || !uname || !gname)
		printError("Usage objgetacl -u username -g groupname objname");
	objname = argv[5];

	Objgetacl *getaclObj = new Objgetacl(username, groupname, objname);
	getaclObj->getACL();
	return 0;

}