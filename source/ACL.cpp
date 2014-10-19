using namespace std;
#include <vector>
class ACLEntry{
public:
	string objname;
	map<string, string>  userPermissions;
	map<string, string>  groupPermissions;

	ACLEntry(){
	}
	ACLEntry(string oname){
		objname = oname;
	}

	ACLEntry(string oname, string uname){
		objname = oname;
		userPermissions[uname] = "rwxpv";
		groupPermissions["*"] = "rwxpv";
	}
	~ACLEntry(){
		userPermissions.clear();
		groupPermissions.clear();
	}

	ACLEntry(const ACLEntry &a){
		objname = a.objname;
		userPermissions = a.userPermissions;
		groupPermissions = a.groupPermissions;

	}
	ACLEntry& operator= (const ACLEntry &a){
		objname = a.objname;
		userPermissions = a.userPermissions;
		groupPermissions = a.groupPermissions;
		return *this;
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
			for (map<string,string>::iterator it2=thisEntry.userPermissions.begin(); it2!=thisEntry.userPermissions.end(); ++it2)
	    		cout << it2->first << " => " << it2->second << '\n';
			for (map<string,string>::iterator it3=thisEntry.groupPermissions.begin(); it3!=thisEntry.groupPermissions.end(); ++it3)
	    		std::cout << it3->first << " => " << it3->second << '\n';
		}
	}

	bool testACL(string uname, string gname, string objname, string access){
		ACLEntry a;
		string perm;
		vector<string> groups;
		if(ace.find(objname) == ace.end())
			printError("Invalid object");
		groups = getUserGroups(uname);

		//check user permissions
		a = ace.find(objname)->second;
		if(a.userPermissions.find(uname) != a.userPermissions.end()){
			perm = a.userPermissions.find(uname)->second;
			if(perm.find(access) != string::npos)
				return true;
		}
		//if no user permissions, check group permissions
		else if(a.groupPermissions.find("*") != a.groupPermissions.end()){
			perm = a.groupPermissions.find("*")->second;
			if(perm.find(access) != string::npos)
				return true;
		}
		else{
			for(vector<string>::iterator it = groups.begin(); it != groups.end(); ++it) {
				string group = *it;
				if(a.groupPermissions.find(group) != a.groupPermissions.end()){
					perm = a.groupPermissions.find(group)->second;
					if(perm.find(access) != string::npos)
						return true;	
				}		
			}
		}
		return false;
	}


	void saveACL(){
		ofstream file("./config/aclFile",ios::out);
		map<string, ACLEntry> aclss = ace;
		for (map<string, ACLEntry>::iterator it = aclss.begin(); it != aclss.end(); it++){
	    	ACLEntry thisyeah = it->second;
	    	file << thisyeah.objname << ":" << endl;
			for (map<string,string>::iterator it2=thisyeah.userPermissions.begin(); it2!=thisyeah.userPermissions.end(); ++it2)
	    		file << "user " << it2->first << " " << it2->second << endl;
			for (map<string,string>::iterator it3=thisyeah.groupPermissions.begin(); it3!=thisyeah.groupPermissions.end(); ++it3)
	    		file << "group " << it3->first << " " << it3->second << endl;
	    	file << endl;
		}
		file.close();
	}

	vector<string> getUserGroups(string username){
		string line;
		string tok;
		string userfile = "./config/userfile.txt";
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

		        validNameString(uname);
				if(uname.compare(username) == 0 && ++i != tokens.end()){
				    //Read and validate groups
				    for(++i; i != tokens.end(); ++i){
				    	tok = (*i);
						validNameString(tok);
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
};