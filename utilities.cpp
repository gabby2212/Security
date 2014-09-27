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
#include "user.cpp"
#include <algorithm>

#define MAX_USERNAME_SIZE 25
#define VALID_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_"
#define ERROR -1

static map<string, User>  users;
static list<string> groups;
static int numUsers;
ofstream fileList;
ofstream newFile;

void sig_hadle(int s){
	fileList.close();
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

bool userExists(string uname){
	if(users.find(uname) == users.end())
		return false;
	return true;
}

bool groupExists(string gname){
	list<string>:: iterator findGrp;
	findGrp = find(groups.begin(), groups.end(), gname.c_str()); 	
	if(findGrp == groups.end())
		return false;
	return true;
}
void setUp(){
	string line;
	string tok;
	numUsers = 0;

	//Create users
	ifstream userFile("userfile");
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
	
	//Add files to users	
	ifstream fileList("fileList");
	if(fileList.is_open())
	{
		while(getline(fileList, line)){
			string uname;
			//Separate lines by spaces
			istringstream buf(line);
		    istream_iterator<string> beg(buf), end;
		    vector<string> tokens(beg, end);
		    vector<string>::const_iterator i;

		    i = tokens.begin();
	        uname = (*i);
	              
	        //Validate username
	        validNameString(uname);
			if(users.find(uname) == users.end())
				printError("User doesn't exit fileList might be corrupted");
   	        User *currentUser = &(users.find(uname)->second);

		    //Validate and add file
		    ++i;
		    if(i == tokens.end())
	    		printError("FileList might be corrupted");
	    	tok = (*i);   	
			validNameString(tok);
    		currentUser->files.push_back(tok);
		}
		fileList.close();
	}
	else{
		//File system is empty create fileList
		ofstream fileList;
		fileList.open("fileList");
		fileList.close();
	}
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
	string fileEntry = uname + " " + filename;
	//Files will be stored as .username-filename
	string fname = "." + uname + "-" + filename;

	//Check if user has file, if not add to fileList
	User currentUser = users.find(uname)->second;
	if(!currentUser.hasFile(filename)){
		ofstream fileList("fileList", ios::app);
		if(fileList.is_open())
			fileList << fileEntry << endl;
		else
			printError("Couldn't open fileList");
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
	list<string> currentFiles = currentUser.files;
	for (list<string>::iterator it = currentFiles.begin() ; it != currentFiles.end(); ++it){
	  	if(!meta)
	        cout << *it << endl;
    	else{
    		long size = fileSize(uname, *it);
    		cout << size << " " << *it << endl; 
    	}
	}
}

