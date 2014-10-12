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

class Objtestacl :public FileSystem{
private:
	string username;
	string groupname;
	string objectname;
	string access;
public:
	Objtestacl(string uname, string gname, string objname, string a){
		validateUserAndGroup(uname, gname);
		if(a.length() != 1)
			printError("Please check one acess at a time");
		objectname = getObjectName(uname, objname);
		username = uname;
		groupname = gname;
		access = a;
	}

	void test(){
		if(acl.testACL(username, groupname, objectname, access))
			cout << "allowed" << endl;
		else
			cout << "denied" << endl;
	}	
};

int main(int argc, char *argv[]){
	int opt;
	string username;
	string objname;
	string groupname;
	string access;
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
	if(argc != 8 || !uname || !gname)
		printError("Usage objtestacl -u username -g groupname -a access objname");
	objname = argv[7];

	Objtestacl *testaclObj = new Objtestacl(username, groupname, objname, access);
	testaclObj->test();
	return 0;
}