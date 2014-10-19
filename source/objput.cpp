using namespace std;

#include <cstdio>
#include <cstdlib>
#include <climits>
#include <ctype.h>
#include <getopt.h>
#include <map>
#include <signal.h>
#include "fileSystem.cpp"

extern map<string, vector<string>>  users;
extern ACL acl;

class Objput :public FileSystem{
private:
	string username;
	string groupname;
	string objectname;
	ofstream newFile;
public:
	Objput(string objname){
		username = FileSystem::username;
		groupname = FileSystem::groupname;
		objectname = getObjectName(username, objname);
	}
	~Objput(){
		newFile.close();
	}
	void writeFile(){
		string line;
		string fileOwner;
		vector<string> *currentFiles;
		string filename = objectname.substr(objectname.find(".") + 1, objectname.length());

		//If current user is not the owner check if file/owner exist
		fileOwner = getOwner(objectname);
		if(!userExists(fileOwner))
			printError("Invalid user");
		if(fileOwner.compare(username) != 0 && !hasFile(fileOwner, filename))
			printError("Cannot create file for another user");

		//If current user is owner and new file, create file
		if(fileOwner.compare(username) == 0 && !hasFile(username, filename)){
			ACLEntry *a = new ACLEntry(objectname, username);
			a->userPermissions[username] = "rwxpv";
			a->groupPermissions["*"] = "rwxpv";
			acl.ace[objectname] = *a;
			acl.saveACL();
			currentFiles = &(users.find(username)->second);
			currentFiles->push_back(filename);
		}
		//If file exists test ACL
		else{
			if(!acl.testACL(username, groupname, objectname, "w"))
				printError("Permission denied");
	 		cerr << "Object already exists, overwritting" << endl;
		}

		//Write to file
		newFile.open(("./fileSystem/" + objectname).c_str());
		if(newFile.is_open()){
			while(getline(cin, line)) {
				if(!(line.length() > MAX_INPUT)){
					newFile << line << endl;
				}
		    }
	    	newFile.close();
		}
		else{
			printf("%s", strerror(errno));
			printError("Couldn't create/open file");   
		}
	}
};

int main(int argc, char *argv[]){
	string objname;
	struct sigaction sigIntHandler;

	//Start signal handling to capture crtl+C and ctrl+D
   	sigIntHandler.sa_handler = sig_handle;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;
   	sigaction(SIGINT, &sigIntHandler, NULL);
   	sigaction(SIGTERM, &sigIntHandler, NULL);

   	if(argc != 2)
		printError("Usage objput objname");
	objname = argv[1];

	Objput *putObj = new Objput(objname);
	putObj->writeFile();
	return 0;
}

