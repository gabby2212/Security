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

class Objsetacl :public FileSystem{
private:
	string username;
	string groupname;
	string objectname;
public:
	Objsetacl(string objname){
		username = FileSystem::username;
		groupname = FileSystem::groupname;
		objectname = getObjectName(username, objname);
	}

	void setAcl(){
		if(!acl.testACL(username, groupname, objectname, "p"))
			printError("Permission denied");
		writeACL();
	}

	void writeACL(){
		string line;
		string token;
		ACLEntry *oldEntry;
		ACLEntry *newEntry;
		map<string, ACLEntry>::iterator aces = acl.ace.find(objectname);
		if(aces == acl.ace.end())
			printError("Invalid object");

		oldEntry = &(aces->second);
		newEntry = new ACLEntry(objectname);

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
				
				newEntry->userPermissions[user] = permissions;
				newEntry->groupPermissions[group] = permissions;
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