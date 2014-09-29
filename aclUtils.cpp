using namespace std;
#include "ACL.cpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#define VALID_PERMS "rwxpv"

static ACL currentACL;

void saveACL(){
	ofstream file("aclFile",ios::out);
	map<string, ACLEntry> aclss = currentACL.ace;
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

void printACL(){
	map<string, ACLEntry> aclss = currentACL.ace;
	for (map<string, ACLEntry>::iterator it = aclss.begin(); it != aclss.end(); it++){
    	ACLEntry thisyeah = it->second;
    	cout << thisyeah.objname << endl;
		for (map<string,string>::iterator it2=thisyeah.userPermissions.begin(); it2!=thisyeah.userPermissions.end(); ++it2)
    		cout << it2->first << " => " << it2->second << '\n';
		for (map<string,string>::iterator it3=thisyeah.groupPermissions.begin(); it3!=thisyeah.groupPermissions.end(); ++it3)
    		std::cout << it3->first << " => " << it3->second << '\n';
	}
}

void getACL(string objname){
	ACLEntry a = currentACL.ace.find(objname)->second;
	for (map<string,string>::iterator it = a.userPermissions.begin(); it != a.userPermissions.end(); ++it)
    	cout << it->second << " " << it->first << endl;
	for (map<string,string>::iterator it2 = a.groupPermissions.begin(); it2 != a.groupPermissions.end(); ++it2)
		cout << it2->second << " " << it2->first << endl;

}

void writeACL(string oname){
	string line;
	string token;
	
	map<string, ACLEntry>::iterator aces = currentACL.ace.find(oname);
	if(aces == currentACL.ace.end())
		printError("Invalid object");
	ACLEntry *objectEntry = &(aces->second);

	while(getline(cin, line)) {
		if(!(line.length() > MAX_INPUT)){
			string user;
			string group;
			string permissions;
			
			istringstream iss(line);
			std::getline(iss, token, '.');
			if(token.empty())
				printError("Usage user.group permissions");
			user = token;
			getline(iss, token);

			istringstream iss2(token);
			vector<string> tok{istream_iterator<string>{iss2}, istream_iterator<string>{}};
			group = tok.front();
			if(group.empty())
				printError("Usage user.group permissions");
			permissions = tok.back();
			if(permissions.empty())
				printError("Usage user.group permissions");
			validNameString(user);
			validNameString(group);
			if(!userExists(user) || !groupExists(group))
				printError("Invalid user or group");
			
			objectEntry->userPermissions[user] = permissions;
			objectEntry->groupPermissions[group] = permissions;
			saveACL();
		}
    }
}

void validPermissions(string permissions){
	bool valid = true;
 	regex rx("r*w*x*p*v*");	
	valid = regex_match(permissions.begin(), permissions.end(), rx);
	if(!valid)
		printError("Invalid permissions");
}

void initACL(){
	string line;
	ifstream aclFile("aclFile");
	if(aclFile.is_open())
	{
		while(getline(aclFile, line)){
			char ch = line.at(line.length()-1);
			string objname;
			if(ch != ':')
				printError("Couldn't create acl, aclFile might be corrupted");
			objname = line.substr(0, line.size()-1);
			validNameString(objname);
			ACLEntry *acle = new ACLEntry(objname);
			while(getline(aclFile, line) && !line.empty()){
				string pType;
				//Separate lines by spaces
				istringstream buf(line);
			    istream_iterator<string> beg(buf), end;
			    vector<string> tokens(beg, end);
			    vector<string>::const_iterator i;
			    i = tokens.begin();

			    pType = (*i);
		    	if(++i == tokens.end())
		    		printError("Expected user, aclFile might be corrupted");
		    	string entityName = (*i);
		    	validNameString(entityName);
		    	if(!userExists(entityName) && !groupExists(entityName))
		    		printError("User or Group doesn't exist, files might be corrupted");
		    	if(++i == tokens.end())
		    		printError("Expected permissions, aclFile might be corrupted");
		    	string permissions = (*i);
		    	validPermissions(permissions);
			    if(pType.compare("user") == 0)
			    	acle->userPermissions[entityName] = permissions;
			    else if(pType.compare("group") == 0)
			    	acle->groupPermissions[entityName] = permissions;
			    else
			    	printError("No permissions for a file, aclFile might be corrupted");
			}
			currentACL.ace[objname] = *acle;
		}
		aclFile.close();
	}
	else{
		//File system is empty create aclFile
		ofstream aclFile;
		aclFile.open("aclFile");
		aclFile.close();
	}	
}