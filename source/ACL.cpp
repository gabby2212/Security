using namespace std;
#include <list>
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
		if(ace.find(objname) == ace.end())
			printError("Invalid object");

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
		else if(a.groupPermissions.find(gname) != a.groupPermissions.end()){
			perm = a.groupPermissions.find(gname)->second;
			if(perm.find(access) != string::npos)
				return true;
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
};