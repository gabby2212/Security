using namespace std;
#include <list>
#include "utilities.cpp"
#include "ACL.cpp"
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <grp.h>

static map<string, vector<string>>  users;
static ACL acl;

class FileSystem{
public:
	string username;
	string groupname;

	FileSystem(){
		setUserAndGroup();
		initACL();
	}

	void initACL(){
		string line;
		string token;
		string username;

		ifstream aclFile("./config/aclFile");
		if(aclFile.is_open())
		{
			while(getline(aclFile, line)){
				string objname;

				//First line should be username.filename:
				char ch = line.at(line.length()-1);
				if(ch != ':')
					printError("Invalid acl format");
				istringstream iss(line);

				//First token is username
				getline(iss, token, '.');
				if(token.empty())
					printError("Usage user.group permissions");
				username = token;

				//Second token is object name, remove :
				getline(iss, token);
				objname = token.substr(0, token.size()-1);
				
				//Validate user and object
				if(!userExists(username))
					printError("Invalid user aclFile might be corrupted");
				validNameString(objname);

				//Add filename to owner
				if(users.find(username) == users.end()){
					vector<string> v = {objname};
					users[username] = v;
				}
				else{
					vector<string> *currentFiles = &(users.find(username)->second);
					currentFiles->push_back(objname);
				}				

				//Read acl permission lines, create entry
				ACLEntry *acle = new ACLEntry(username + "." +objname);
				while(getline(aclFile, line) && !line.empty()){
					string entityType;
					string entityName;
					string permissions;
					//Separate lines by spaces
					istringstream buf(line);
				    istream_iterator<string> beg(buf), end;
				    vector<string> tokens(beg, end);
				    vector<string>::const_iterator i;
				    i = tokens.begin();

				    //acl line user/group username/groupname permissions
				    entityType = (*i);
			    	if(++i == tokens.end())
			    		printError("Invalid aclFile");
			    	entityName = (*i);
			    	if(++i == tokens.end())
			    		printError("Invalid aclFile");
			    	permissions = (*i);
			    
			    	//validate line, add to acl entry
			    	validNameString(entityName);
			    	validPermissions(permissions);
				    if(entityType.compare("user") == 0){
				    	if(!userExists(entityName))
				    		printError("Invalid username in aclFile");
				    	acle->userPermissions[entityName] = permissions;
				   	}
				    else if(entityType.compare("group") == 0){
				    	if(!groupExists(entityName) && entityName.compare("*") != 0)
				    		printError("Invalid groupname in aclFile");
				    	acle->groupPermissions[entityName] = permissions;
				    }
				    else
				    	printError("Invalid keyword (user/group expected)");
				}
				acl.ace[username + "." +objname] = *acle;
			}
			aclFile.close();
		}
		else{
			//File system is empty create aclFile
			ofstream aclFile;
			aclFile.open("./config/aclFile");
			aclFile.close();
		}
	}

	bool userExists(string uname){
		if(getpwnam(uname.c_str()) != NULL)
			return true;
		return false;
	}

	bool groupExists(string gname){
		if(getgrnam(gname.c_str()) != NULL && gname.compare("*") != 0)
			return true;
		return false;
	}

	void validateUserAndGroup(string username, string groupname){
		validNameString(username);
		validNameString(groupname);
		if(!userExists(username))
			printError("Invalid user");
		if(!groupExists(groupname) && groupname.compare("*") != 0)
			printError("Invalid group");
	}

	void setUserAndGroup(){
		uid_t  uid = getuid();
		uid_t  gid = getgid();
		struct passwd *currentUser = getpwuid(uid);
		struct group *currentGroup = getgrgid(gid);
		username = currentUser->pw_name;
		groupname = currentGroup->gr_name;
	}

	bool hasFile(string uname, string objname){
		vector<string>:: iterator findFile;
		vector<string> userFiles = users.find(uname)->second;
		findFile = find(userFiles.begin(), userFiles.end(), objname);
		if(findFile == userFiles.end())
			return false;
		return true;
	}
};