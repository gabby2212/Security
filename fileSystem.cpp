using namespace std;
#include <list>
#include "user.cpp"
#include "utilities.cpp"
#include "ACL.cpp"
static map<string, User>  users;
static list<string> groups;
static ACL acl;

class FileSystem{
public:
	FileSystem(){
		setUp();
		initACL();
	}
	void setUp(){
		string line;
		string tok;
		string userfile = "userfile.txt";

		//Create users
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
			}
			userFile.close();	
		}
		else
			printError("Couldn't open userfile");
	}
	void initACL(){
		string line;
		string token;
		string username;

		ifstream aclFile("aclFile");
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
				User *u = &(users.find(username)->second);
				validNameString(objname);

				//Add filename to owner
				u->files.push_back(objname);

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
				    	if(!groupExists(entityName))
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
			aclFile.open("aclFile");
			aclFile.close();
		}
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

	void validateUserAndGroup(string username, string groupname){
		User thisUser;
		vector<string> thisGroups;
		validNameString(username);
		validNameString(groupname);
		if(!userExists(username))
			printError("Invalid user");
		if(!groupExists(groupname))
			printError("Invalid group");
		thisUser =  users.find(username)->second;
		thisGroups = thisUser.getGroups();
		if(find(thisGroups.begin(), thisGroups.end(), groupname) == thisGroups.end())
			printError("User not in group");

	}
};