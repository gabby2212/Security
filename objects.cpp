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
};
class User{
public:
	string username;
	list<string> groups;
	list<string> files;
	User(){
	}
	User(string uname){
		username = uname;
	}
	~User(){
		groups.clear();
	}

	User (const User &u){
		groups =u.groups;
		username = u.username;
	}
	User& operator= (const User &u){
		groups =u.groups;
		username = u.username;
		return *this;
	}

	bool hasFile(string filename){
	  for (list<string>::iterator it = files.begin() ; it != files.end(); ++it){
	  	if(filename.compare(*it) == 0)
	  		return true;
	  }
	  return false;
	}

	void addFile(string filename){
		files.push_back(filename);
	}
};
