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

		ifstream aclFile("/config/aclFile");
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
					printError("Usage user.objname:");
				username = token;

				//Second token is object name, remove :
				getline(iss, token);
				objname = token.substr(0, token.size()-1);
				
				//Validate user and object
				if(!userExists(username))
					printError("Invalid user aclFile might be corrupted");
				validNameString(objname, true);

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
				//u2.g1 rwp
				ACLEntry *acle = new ACLEntry(username + "." + objname);
				while(getline(aclFile, line) && !line.empty()) {
					if(!(line.length() > MAX_INPUT)){
						string user;
						string group;
						string permissions;
						
						//First token is username
						istringstream iss(line);
						std::getline(iss, token, '.');
						if(token.empty())
							printError("Invalid aclFile");
						user = token;
						getline(iss, token);

						//Second token is groupname
						istringstream iss2(token);
						vector<string> tok{istream_iterator<string>{iss2}, istream_iterator<string>{}};
						group = tok.front();
						if(group.empty())
							printError("Invalid aclFile");
						permissions = tok.back();
						if(permissions.empty())
							printError("Invalid aclFile");
						validateUserAndGroup(user, group);
						validPermissions(permissions);
						
						acle->permissions[make_pair(user, group)] = permissions;
					}
			    }
				acl.ace[username + "." +objname] = *acle;
			}
			aclFile.close();
		}
		else{
			//File system is empty create aclFile
			ofstream aclFile;
			aclFile.open("/config/aclFile");
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
		validNameString(username, false);
		validNameString(groupname, false);
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