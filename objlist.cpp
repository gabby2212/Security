using namespace std;

#include <cstdio>
#include <cstdlib>
#include <climits>
#include <ctype.h>
#include <getopt.h>
#include <vector>
#include "fileSystem.cpp"
#include <map>
#include <signal.h>

extern map<string, User>  users;
extern list<string> groups;
extern ACL acl;

class Objlist :public FileSystem{
private:
	string username;
	string objectname;
	bool metaData;
public:
	Objlist(string uname, bool meta){
		validNameString(uname);
		if(!userExists(uname))
			printError("Invalid user");
		username = uname;
		metaData = meta;
	}
	long fileSize(string filename){
		FILE *fp;
		long size = 0;
		string fname = username + "." + filename;

		fp = fopen(fname.c_str(), "r");
		if(fp == NULL)
			printError("Error opening file");
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		if(size < 0)
			printError("Error reading file");
		fclose(fp);
		return size;
	}

	void printFiles(){
		string line;
		long size;
		vector<string>::iterator it;
		User currentUser = users.find(username)->second;
		vector<string> files = currentUser.getFiles();

		for (it = files.begin() ; it != files.end(); ++it){
		  	if(!metaData)
		        cout << *it << endl;
	    	else{
	    		size = fileSize(*it);
	    		cout << size << " " << *it << endl; 
	    	}
		}
	}
};

int main(int argc, char *argv[]){
	int opt;
	string username;
	struct sigaction sigIntHandler;
	bool meta = false;
	bool uname = false;

   	//Start signal handling to capture crtl+C and ctrl+D
   	sigIntHandler.sa_handler = sig_handle;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;
   	sigaction(SIGINT, &sigIntHandler, NULL);
   	sigaction(SIGTERM, &sigIntHandler, NULL);

   	//Check for valid input params
	while((opt = getopt(argc, argv, "lu:")) != ERROR){
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
		printError("Usage objlist -u username (-l)");

	Objlist *listObj = new Objlist(username, meta);
	listObj->printFiles();
	return 0;
}