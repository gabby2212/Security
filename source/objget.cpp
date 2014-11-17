using namespace std;
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <ctype.h>
#include <getopt.h>
#include "fileSystem.cpp"
#include <map>
#include <signal.h>
#include <openssl/md5.h>


extern map<string, vector<string>>  users;
extern ACL acl;

class Objget :public FileSystem{
private:
	string username;
	string groupname;
	string objectname;
	string passphrase;
	char *decKey;

public:
	Objget(string objname, string pass){
		username = FileSystem::username;
		groupname = FileSystem::groupname;
		objectname = getObjectName(username, objname);
		passphrase = pass;
	}
	~Objget(){
		free(decKey);
	}
	void readFile(){
		string tempLine;
		char line[MAX_INPUT];
		char decLine[MAX_INPUT];
		char *key;
		int bytesRead = 0;
		FILE *fp;
		decKey = (char *)malloc(32*sizeof(char));
		unsigned long passLen = (unsigned long)strlen((const char *)passphrase.c_str());
		const unsigned char *pass = (const unsigned char *)passphrase.c_str();
		unsigned char digest[MD5_DIGEST_LENGTH];
		int bytesToWrite = 0;

		if(!acl.testACL(username, groupname, objectname, "r"))
			printError("Permission denied");

		string path = "/fileSystem/" + objectname + ".meta";
		ifstream aclFile(path.c_str(), ios::binary);
		if(aclFile.is_open())
		{
			getline(aclFile, tempLine);
			if(tempLine.empty())
				printError("Invalid meta file");
			key = (char *)tempLine.c_str();
			aclFile.close();
		}
		else
			printError("Couldn't read encrypted key");

		MD5(pass, passLen, (unsigned char*)&digest);
		int encryptedBytes = decryptLine(key, (unsigned char *)digest, (unsigned char *)decKey);
		fp = fopen(("/fileSystem/" + objectname).c_str(), "rb");
		if(fp != NULL)
		{
			cout << "what?" << endl;
			while((bytesRead = fread(line, 1, MAX_INPUT, fp)) > 0) {
				cout << line << endl;
				cout << bytesRead << endl;
				if(bytesRead < MAX_INPUT)
					line[bytesRead] = '\0';
				bytesToWrite = decryptLine(line, (unsigned char *)decKey, (unsigned char *)decLine);
				cout << decLine << endl; 
				//fwrite(decLine, 1, bytesToWrite, stdout);
		    }
			fclose(fp);
		}
		else
			printError("Couldn't open file");
	}
};

int main(int argc, char *argv[]){
	int opt;
	struct sigaction sigIntHandler;
	bool k = false;
	string passphrase;
	string objname;

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
		printError("Usage objget -k passphrase objname");
	string str(argv[1]);
	if(str.compare("-k") == 0)
		objname = argv[3];
	else
		objname = argv[1];

	Objget *getObj = new Objget(objname, passphrase);
	getObj->readFile();
	return 0;
}