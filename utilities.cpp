using namespace std;
#include <sys/stat.h>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <list>
#include "objects.cpp"
#include <algorithm>
#include <regex>

#define MAX_USERNAME_SIZE 25
#define VALID_CHARS ".*abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_"
#define ERROR -1
#define VALID_PERMS "rwxpv"

static map<string, User>  users;
static list<string> groups;
static int numUsers;
static ACL currentACL;

ofstream newFile;


void sig_handle(int s){
	newFile.close();
	exit(0);
}

void printError(string err){
	cerr << err << endl;
	exit(1);
}

void validNameString(string name){
	if(name.length() > MAX_USERNAME_SIZE)
		printError("Name too long");
	if (name.find_first_not_of(VALID_CHARS) != string::npos)
		printError("Invalid characters");
}

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
bool userExists(string uname){
	if(users.find(uname) == users.end())
		return false;
	return true;
}

bool groupExists(string gname){
	list<string>:: iterator findGrp;
	findGrp = find(groups.begin(), groups.end(), gname.c_str()); 	
	if(findGrp == groups.end() && gname.compare("*") != 0)
		return false;
	return true;
}
void setUp(string userfile){
	string line;
	string tok;
	numUsers = 0;

	//Create users
	ifstream userFile(userfile);
	if(userFile.is_open())
	{
		while(getline(userFile, line)){
			string uname;
			int numGroups;

			//Separate line by spaces
			istringstream buf(line);
		    istream_iterator<string> beg(buf), end;
		    vector<string> tokens(beg, end);
		    vector<string>::const_iterator i;

		    i = tokens.begin();
	        uname = (*i);
	        User *u;
			
			//Validate username
	        validNameString(uname);
			if(userExists(uname))
				printError("User is declared twice, userfile might be corrupted");
		    
		    //Create user
		    try{
		    	u = new User(uname);
		    }
		    catch(bad_alloc){
		    	printError("Alloc failed");
		    }

		    //Read and validate groups
		    for(++i; i != tokens.end(); ++i){
		    	tok = (*i);
				validNameString(tok);
			    u->groups.push_back(tok);
			    if(!groupExists(tok))
			    	groups.push_back(tok);
		    }
		    users[uname] = *u;
		    numUsers++;			
		}
		userFile.close();	
	}
	else
		printError("Couldn't open userfile");
}

long fileSize(string username, string filename){
	FILE *fp;
	long size = 0;
	string fname = "." + username + "-" + filename;

	fp = fopen(fname.c_str(), "r");
	if(fp == NULL)
		printError("Error opening file");

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	if(size < 0)
		printError("Error reading file");
	return size;
}

void writeFile(string filename, string uname){
	string line;
	string fileEntry = uname + "." + filename;
	//Files will be stored as .username-filename
	string fname = "." + uname + "-" + filename;

	//Check if user has file, if not add to fileList
	User currentUser = users.find(uname)->second;
	if(!currentUser.hasFile(filename)){
		string objname = uname + "." + filename;
		ACLEntry *a = new ACLEntry(objname, uname);
		a->userPermissions[uname] = "rwxpv";
		a->groupPermissions["*"] = "rwxpv";
		currentACL.ace[objname] = *a;
		saveACL();
		currentUser.addFile(filename);
	}

	//Write to file
	ofstream newFile(fname.c_str());
	if(newFile.is_open()){
		while(getline(cin, line)) {
			if(!(line.length() > MAX_INPUT)){
				newFile << line << endl;
			}
	    }
    	newFile.close();
	}
	else
		printError("Couldn't create file");   

}

void readFile(string filename, string uname){
	string line;
	//Files will be stored as .username-filename
	string fname = "." + uname + "-" + filename;

	ifstream file(fname.c_str());
	if(file.is_open())
	{
		while(getline(file, line))
			cout << line << endl;
		file.close();	
	}
	else
		printError("Couldn't open file");
}

void printFiles(string uname, bool meta){
	string line;
	User currentUser = users.find(uname)->second;
	list<string> currentFiles;
	for (list<string>::iterator it = currentFiles.begin() ; it != currentFiles.end(); ++it){
	  	if(!meta)
	        cout << *it << endl;
    	else{
    		long size = fileSize(uname, *it);
    		cout << size << " " << *it << endl; 
    	}
	}
}

void printACL(){
	map<string, ACLEntry> aclss = currentACL.ace;
	for (map<string, ACLEntry>::iterator it = aclss.begin(); it != aclss.end(); it++){
    	ACLEntry thisyeah = it->second;
		for (map<string,string>::iterator it2=thisyeah.userPermissions.begin(); it2!=thisyeah.userPermissions.end(); ++it2)
    		cout << it2->first << " => " << it2->second << '\n';
		for (map<string,string>::iterator it3=thisyeah.groupPermissions.begin(); it3!=thisyeah.groupPermissions.end(); ++it3)
    		std::cout << it3->first << " => " << it3->second << '\n';
	}
}

void getACL(string objname){
	if(currentACL.ace.find(objname) == currentACL.ace.end())
		printError("Invalid object");
	ACLEntry a = currentACL.ace.find(objname)->second;
	for (map<string,string>::iterator it = a.userPermissions.begin(); it != a.userPermissions.end(); ++it)
    	cout << it->second << " " << it->first << endl;
	for (map<string,string>::iterator it2 = a.groupPermissions.begin(); it2 != a.groupPermissions.end(); ++it2)
		cout << it2->second << " " << it2->first << endl;

}

bool testACL(string uname, string gname, string objname, string access){
	cout << objname << endl;
	if(currentACL.ace.find(objname) == currentACL.ace.end())
		printError("Invalid object");
	ACLEntry a = currentACL.ace.find(objname)->second;
	if(a.userPermissions.find(uname) != a.userPermissions.end()){
		string userperm = a.userPermissions.find(uname)->second;
		if(userperm.find(access) != string::npos)
			return true;
	}
	else if(a.groupPermissions.find(gname) != a.groupPermissions.end()){
		string gperm = a.groupPermissions.find(gname)->second;
		if(gperm.find(access) != string::npos)
			return true;
	}
	return false;
	
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
	string token;
	string user;

	ifstream aclFile("aclFile");
	if(aclFile.is_open())
	{
		while(getline(aclFile, line)){
			char ch = line.at(line.length()-1);
			string objname;
			if(ch != ':')
				printError("Couldn't create acl, aclFile might be corrupted");
			istringstream iss(line);
			std::getline(iss, token, '.');
			if(token.empty())
				printError("Usage user.group permissions");
			user = token;
			if(!userExists(user))
				printError("aclFile might be corrupted");
			User u = users.find(user)->second;
			getline(iss, token);
			objname = token.substr(0, token.size()-1);
			validNameString(objname);
			u.addFile(objname);

			ACLEntry *acle = new ACLEntry(user + "." +objname);
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
			currentACL.ace[user + "." +objname] = *acle;
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