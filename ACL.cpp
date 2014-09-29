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