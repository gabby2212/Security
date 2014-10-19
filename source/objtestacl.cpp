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

class Objtestacl :public FileSystem{
private:
	string username;
	string groupname;
	string objectname;
	string access;
public:
	Objtestacl(string objname, string a){
		if(a.length() != 1)
			printError("Please check one acess at a time");
		username = FileSystem::username;
		groupname = FileSystem::groupname;
		objectname = getObjectName(username, objname);
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
	string objname;
	string access;
	bool ac = false;
	struct sigaction sigIntHandler;

	//Start signal handling to capture crtl+C and ctrl+D
   	sigIntHandler.sa_handler = sig_handle;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;
   	sigaction(SIGINT, &sigIntHandler, NULL);
   	sigaction(SIGTERM, &sigIntHandler, NULL);
   	
   	//Check for valid input params
	while((opt = getopt(argc, argv, "a:")) != ERROR){
		switch(opt){
			case 'a':
				ac = true;
				access = optarg;
				break;
		}
	}
	if(argc != 4 || !ac)
		printError("Usage objtestacl -a access objname");
	objname = argv[3];

	Objtestacl *testaclObj = new Objtestacl(objname, access);
	testaclObj->test();
	return 0;
}