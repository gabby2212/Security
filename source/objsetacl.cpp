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

class Objsetacl :public FileSystem{
private:
	string username;
	string groupname;
	string objectname;
	char *key;
	char *iv;
public:
	Objsetacl(string objname){
		username = FileSystem::username;
		groupname = FileSystem::groupname;
		objectname = getObjectName(username, objname);
	}

	~Objsetacl(){
		free(key);
		free(iv);
	}

	void setAcl(){
		if(!acl.testACL(username, groupname, objectname, "p"))
			printError("Permission denied");
		writeACL();
	}

	void writeACL(){
		string line;
		string token;
		key = (char *)calloc(32, sizeof(char));
		iv = (char *)calloc(16, sizeof(char));
		ACLEntry *oldEntry;
		ACLEntry *newEntry;
		map<string, ACLEntry>::iterator aces = acl.ace.find(objectname);
		if(aces == acl.ace.end())
			printError("Invalid object");

		oldEntry = &(aces->second);
		getEncKey(objectname, key, iv);
		newEntry = new ACLEntry(objectname);
		newEntry->setEncKey(key, iv);

		while(getline(cin, line)) {
			if(!(line.length() > MAX_INPUT)){
				string user;
				string group;
				string permissions;
				
				istringstream iss(line);
				std::getline(iss, token, '.');
				if(token.empty())
					printError("Usage user.group permissions");
				user = token;
				getline(iss, token);

				istringstream iss2(token);
				vector<string> tok{istream_iterator<string>{iss2}, istream_iterator<string>{}};
				group = tok.front();
				if(group.empty())
					printError("Usage user.group permissions");
				permissions = tok.back();
				if(permissions.empty())
					printError("Usage user.group permissions");
				validateUserAndGroup(user, group);
				validPermissions(permissions);
				
				newEntry->permissions[make_pair(user, group)] = permissions;
			}
	    }
	    acl.ace[objectname] = *newEntry;
	    acl.saveACL();
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
		printError("Usage objsetacl objname");
	objname = argv[1];

	Objsetacl *setAclObj = new Objsetacl(objname);
	setAclObj->setAcl();
	return 0;
}