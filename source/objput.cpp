using namespace std;

#include <cstdio>
#include <cstdlib>
#include <climits>
#include <ctype.h>
#include <getopt.h>
#include <map>
#include <signal.h>
#include "fileSystem.cpp"
#include <openssl/md5.h>

extern map<string, vector<string>>  users;
extern ACL acl;

class Objput :public FileSystem{
private:
	string username;
	string groupname;
	string objectname;
	string passphrase;
	char key[16];
	char *encKey;
public:
	Objput(string objname, string pass){
		username = FileSystem::username;
		groupname = FileSystem::groupname;
		objectname = getObjectName(username, objname);
		passphrase = pass;
		generateKeys();
	}
	~Objput(){
	}
	void writeFile(){
		char line[MAX_INPUT];
		string fileOwner;
		char encLine[MAX_INPUT];
		int bytesToWrite = 0;
		ssize_t bytesRead = 0;
		FILE *fp;
		string filename = objectname.substr(objectname.find(".") + 1, objectname.length());

		//If current user is not the owner check if file/owner exist
		fileOwner = getOwner(objectname);
		if(!userExists(fileOwner))
			printError("Invalid user");
		if(fileOwner.compare(username) != 0 && !hasFile(fileOwner, filename))
			printError("Cannot create file for another user");
	    
		//If current user is owner and new file, create file
		if(fileOwner.compare(username) == 0 && !hasFile(username, filename)){
			createUserFile(filename);
		}
		//If file exists test ACL
		else{
			if(!acl.testACL(username, groupname, objectname, "w"))
				printError("Permission denied");
	 		cerr << "Object already exists, overwritting" << endl;
	 		ACLEntry *a = &(acl.ace.find(objectname)->second);
	 		a->setEncKey(encKey);
			acl.ace[objectname] = *a;
		}

		//Write to file
		fp = fopen(("/fileSystem/" + objectname).c_str(), "wb");
		if(fp != NULL){
			while((bytesRead = read(STDIN_FILENO, line, MAX_INPUT)) > 0) {
				line[bytesRead] = '\0';
				bytesToWrite = encryptLine(line, (unsigned char *)key, (unsigned char *)encLine); 
				fwrite(encLine, 1, bytesToWrite, fp);
		    }
			fclose(fp);
			acl.saveACL();
		}
		else{
			printf("%s\n", strerror(errno));
			printError("Couldn't create/open file");   
		}
	}

	void createUserFile(string filename){
		vector<string> *currentFiles;
		ACLEntry *a = new ACLEntry(objectname, username);
		a->setEncKey(encKey);
		acl.ace[objectname] = *a;
		currentFiles = &(users.find(username)->second);
		currentFiles->push_back(filename);
	}

	void generateKeys(){
		int byte_count = 16;
		encKey = (char *)calloc(32, sizeof(char));
		unsigned long passLen = (unsigned long)strlen((const char *)passphrase.c_str());
		const unsigned char *pass = (const unsigned char *)passphrase.c_str();
		unsigned char digest[MD5_DIGEST_LENGTH];
		FILE *fp;
		fp = fopen("/dev/urandom", "rb");
		fread(&key, 1, byte_count, fp);
		fclose(fp);

		MD5(pass, passLen, (unsigned char*)&digest);
		int encryptedBytes = encryptLine(key, digest, (unsigned char *)encKey);
	}
};

int main(int argc, char *argv[]){
	int opt;
	string objname;
	string passphrase;
	bool k = false;
	struct sigaction sigIntHandler;

	//Start signal handling to capture crtl+C and ctrl+D
   	sigIntHandler.sa_handler = sig_handle;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;
   	sigaction(SIGINT, &sigIntHandler, NULL);
   	sigaction(SIGTERM, &sigIntHandler, NULL);

	//Check for valid input params
	while((opt = getopt(argc, argv, "k:")) != ERROR){
		switch(opt){
			case 'k':
				k = true;
				passphrase = optarg;
				break;
		}
	}
	if(!k || argc != 4)
		printError("Usage objput -k passphrase objname");
	string str(argv[1]);
	if(str.compare("-k") == 0)
		objname = argv[3];
	else
		objname = argv[1];
	validNameString(passphrase, true);

	Objput *putObj = new Objput(objname, passphrase);
	putObj->writeFile();
	return 0;
}

