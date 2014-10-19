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

extern map<string, vector<string>>  users;
extern ACL acl;

class Objlist :public FileSystem{
private:
	string username;
	bool metaData;
public:
	Objlist(bool meta){
		username = FileSystem::username;
		metaData = meta;
	}
	long fileSize(string filename){
		FILE *fp;
		long size = 0;
		string fname = "./fileSystem/" + username + "." + filename;

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
		vector<string> files = users.find(username)->second;

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
	struct sigaction sigIntHandler;
	bool meta = false;

   	//Start signal handling to capture crtl+C and ctrl+D
   	sigIntHandler.sa_handler = sig_handle;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;
   	sigaction(SIGINT, &sigIntHandler, NULL);
   	sigaction(SIGTERM, &sigIntHandler, NULL);

   	//Check for valid input params
	while((opt = getopt(argc, argv, "l")) != ERROR){
		switch(opt){
			case 'l':
				meta = true;
				break;
		}
	}
	if((!meta && argc != 1) || (meta && argc != 2))
		printError("Usage objlist (-l)");

	Objlist *listObj = new Objlist(meta);
	listObj->printFiles();
	return 0;
}