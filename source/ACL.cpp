using namespace std;
#include <vector>
#include <string.h>

class ACLEntry{
public:
	string objname;
	unsigned char *encKey;
	unsigned char *iv;
	map<pair<string, string>, string>  permissions;
	int keySetFlag = 0;
	ACLEntry(){
	}
	ACLEntry(string oname){
		objname = oname;
		keySetFlag = 0;
	}

	ACLEntry(string oname, string uname){
		objname = oname;
		permissions[make_pair(uname, "*")] = "rwxpv";
		keySetFlag = 0;
	}
	~ACLEntry(){
		permissions.clear();
	}

	ACLEntry(const ACLEntry &a){
		objname = a.objname;
		permissions = a.permissions;
		encKey = a.encKey;
		iv = a.iv;
		keySetFlag = a.keySetFlag;
	}
	ACLEntry& operator= (const ACLEntry &a){
		objname = a.objname;
		permissions = a.permissions;
		encKey = a.encKey;
		keySetFlag = a.keySetFlag;
		iv = a.iv;
		return *this;
	}
	unsigned char *getEncKey(){
		if(keySetFlag == 1)
			return encKey;
		else
			return NULL;
	}

	unsigned char *getIv(){
		if(keySetFlag == 1)
			return iv;
		else
			return NULL;
	}

	void setEncKey(char * key, char *v){
		encKey = (unsigned char *)key;
		iv = (unsigned char *)v;
		keySetFlag = 1;
	}

	int getKeyFlag(){
		return keySetFlag;
	}

	string findPermissions(string username, string gname){
		map<pair<string,string>, string>::iterator it;
		for(it = permissions.begin(); it != permissions.end(); it++){
			if((it->first).first == username)
				if((it->first).second == gname)
					return it->second;
		}
		return "not found";
	}

	void printACLEntry(){
		map<pair<string,string>, string>::iterator it;
		for(it = permissions.begin(); it != permissions.end(); it++)
		    cout << (it->first).first << "." << (it->first).second << " => " << it->second << '\n';	
	}
};

class ACL{
public:
	map<string, ACLEntry> ace;
	ACL(){
	}
	ACL(ACLEntry acEntry, string name){
		ace[name] = acEntry;
	}
	~ACL(){
		ace.clear();
	}

	ACL (const ACL &a){
		ace = a.ace;
	}
	ACL& operator= (const ACL &a){
		ace = a.ace;
		return *this;
	}
	void printACL(){
		for (map<string, ACLEntry>::iterator it = ace.begin(); it != ace.end(); it++){
	    	string filename = it->first;
	    	ACLEntry thisEntry = it->second;
	    	cout << filename << ":" << endl;
			thisEntry.printACLEntry();
		}
	}

	bool testACL(string uname, string gname, string objname, string access){
		ACLEntry a;
		string perm;
		vector<string> groups;
		if(ace.find(objname) == ace.end())
			printError("Couldn't find object");
		groups = getUserGroups(uname);

		//check user permissions
		a = ace.find(objname)->second;
		
		if((perm = a.findPermissions(uname, "*")) != "not found"){
			if(perm.find(access) != string::npos)
				return true;
		}
		else{
			for(vector<string>::iterator it = groups.begin(); it != groups.end(); ++it) {
				string group = *it;
				if((perm = a.findPermissions(uname, group)) != "not found"){
					if(perm.find(access) != string::npos)
						return true;	
				}		
			}
		}
		return false;
	}


	void saveACL(){
		map<string, ACLEntry> aclss = ace;
		char *key = (char *)calloc(32, sizeof(char));
		char *iv = (char *)calloc(16, sizeof(char));

		for (map<string, ACLEntry>::iterator it = aclss.begin(); it != aclss.end(); it++){
			string filename = it->first;
			long size = fileSize(filename);
			filename = "/fileSystem/" + filename + ".meta";
			ACLEntry thisObject = it->second;
			if(thisObject.getKeyFlag() == 0)
				getEncKey(filename, key, iv);				
			else{
				key = (char *)thisObject.getEncKey();
				iv = (char *)thisObject.getIv();
			}

			ofstream file(filename, ios::out);
			if(file.is_open()){
				file << key << endl;
				file << "" << endl;
				file << iv << endl;
				file << "" << endl;
		    	file << size << endl;
				for (map<pair<string,string>, string>::iterator it2=thisObject.permissions.begin(); it2!=thisObject.permissions.end(); ++it2)
		    		file << (it2->first).first << "." << (it2->first).second << " " << it2->second << endl;
		    	file << endl;
    			file.close();
			}
			else
				printError("Couldn't save acl");	    	
		}
		free(key);
	}

	vector<string> getUserGroups(string username){
		string line;
		string tok;
		string userfile = "/config/userfile.txt";
		vector<string> groups;

		//Get user's groups from userfile.txt
		ifstream userFile(userfile);
		if(userFile.is_open())
		{
			while(getline(userFile, line)){
				string uname;
				//Separate line by spaces
				istringstream buf(line);
			    istream_iterator<string> beg(buf), end;
			    vector<string> tokens(beg, end);
			    vector<string>::const_iterator i;

			    i = tokens.begin();
		        uname = (*i);

		        validNameString(uname, false);
				if(uname.compare(username) == 0 && ++i != tokens.end()){
				    //Read and validate groups
				    for(++i; i != tokens.end(); ++i){
				    	tok = (*i);
						validNameString(tok, false);
  					    groups.push_back(tok);
				    }
				    userFile.close();
				    return groups;
				}
			}
			userFile.close();	
		}
		else
			printError("Couldn't open userfile");
	}

	long fileSize(string filename){
		FILE *fp;
		long size = 0;
		string fname = "/fileSystem/" + filename;

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

	void getEncKey(string objectname, char *key, char *iv){
		string path = objectname;
		string tempKey;
		string tempIv;
		string line;
		ifstream aclFile(path.c_str(), ios::binary);
		if(aclFile.is_open())
		{
			getline(aclFile, line);
			if(line.empty())
				printError("Invalid meta file (No key)");
			while (!line.empty()){
				tempKey += line;
				getline(aclFile, line);
			}

			getline(aclFile, line);
			if(line.empty())
				printError("Invalid meta file (No iv)");
			while (!line.empty()){
				tempIv += line;
				getline(aclFile, line);
			}
		}
		else
			printError("Couldn't read encrypted key");
		strcpy(key, tempKey.c_str());
		strcpy(iv, tempIv.c_str());
	}
};