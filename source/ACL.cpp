using namespace std;
#include <vector>
class ACLEntry{
public:
	string objname;
	map<pair<string, string>, string>  permissions;

	ACLEntry(){
	}
	ACLEntry(string oname){
		objname = oname;
	}

	ACLEntry(string oname, string uname){
		objname = oname;
		permissions[make_pair(uname, "*")] = "rwxpv";
	}
	~ACLEntry(){
		permissions.clear();
	}

	ACLEntry(const ACLEntry &a){
		objname = a.objname;
		permissions = a.permissions;

	}
	ACLEntry& operator= (const ACLEntry &a){
		objname = a.objname;
		permissions = a.permissions;
		return *this;
	}

	string findUserPermissions(string username){
		map<pair<string,string>, string>::iterator it;
		for(it = permissions.begin(); it != permissions.end(); it++){
			if((it->first).first == username)
				return it->second;
		}
		return "not found";
	}

	string findGroupPermissions(string gname){
		map<pair<string,string>, string>::iterator it;
		for(it = permissions.begin(); it != permissions.end(); it++){
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
		
		if((perm = a.findUserPermissions(uname)) != "not found"){
			if(perm.find(access) != string::npos)
				return true;
		}
		//if no user permissions, check group permissions
		else if((perm = a.findGroupPermissions("*")) != "not found"){
			if(perm.find(access) != string::npos)
				return true;
		}
		else{
			for(vector<string>::iterator it = groups.begin(); it != groups.end(); ++it) {
				string group = *it;
				if((perm = a.findGroupPermissions(group)) != "not found"){
					if(perm.find(access) != string::npos)
						return true;	
				}		
			}
		}
		return false;
	}


	void saveACL(){
		map<string, ACLEntry> aclss = ace;
		for (map<string, ACLEntry>::iterator it = aclss.begin(); it != aclss.end(); it++){
			string filename = it->first;
			long size = fileSize(filename);
			filename = "/fileSystem/" + filename + ".meta";
			ofstream file(filename, ios::out);
			if(file.is_open()){
				ACLEntry thisObject = it->second;
		    	file << size << endl;
				for (map<pair<string,string>, string>::iterator it2=thisObject.permissions.begin(); it2!=thisObject.permissions.end(); ++it2)
		    		file << (it2->first).first << "." << (it2->first).second << " " << it2->second << endl;
		    	file << endl;
    			file.close();
			}
			else
				printError("Couldn't save acl");	    	
		}
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
};