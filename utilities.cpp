using namespace std;
#include <sys/stat.h>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "user.cpp"

#define MAX_USERNAME_SIZE 25
#define MAX_NUMBER_OF_GROUPS 50
#define INITIAL_NUMBER_OF_USERS 100
#define MAX_LINE_SIZE (MAX_USERNAME_SIZE*MAX_NUMBER_OF_GROUPS + MAX_USERNAME_SIZE)


static std::map<string, User>  users;
static int numUsers;
ofstream fileList;
ofstream newFile;

void printError(string err){
	cerr << err << endl;
	exit(0);
}

void setUp(){
	FILE *fp;
	FILE *fp2;
	char line[MAX_LINE_SIZE];
	string tok;
	numUsers = 0;
	int userArraySize = INITIAL_NUMBER_OF_USERS;

	fp = fopen("userfile.txt", "r");
	fp2 = fopen("fileList", "rw");
	if(fp == NULL || fp2 == NULL)
		printError("Error opening file");

	while(fgets(line, MAX_LINE_SIZE, fp)!= NULL){
		istringstream buf(line);
	    istream_iterator<std::string> beg(buf), end;

	    vector<string> tokens(beg, end);
	    vector<string>::const_iterator i;
	    i = tokens.begin();
        string uname = (*i);
		vector<string> groups;
		int numGroups = 0;
		
		if(uname.length() > MAX_USERNAME_SIZE){
			cerr << "Username:" << uname << "too long, userfile might be corrupted\n";
			continue;
		}
		if(users.find(uname) != users.end())
			printError("User is declared twice, userfile might be corrupted");
	    
	    for(++i; i != tokens.end(); ++i){
	    	tok = (*i);
	    	if(tok.length() > MAX_USERNAME_SIZE){
				cerr << "Groupname:" << tok << "too long, userfile might be corrupted\n";
			}
			else{
		        groups.push_back(tok);
		        numGroups++;
		    }
	    }
	    User *u = new User(uname, groups, numGroups);
	    users[uname] = *u;
	    numUsers++;
	}

	while(fgets(line, MAX_LINE_SIZE, fp2) != NULL){
		istringstream buf(line);
	    istream_iterator<std::string> beg(buf), end;

	    vector<string> tokens(beg, end);
	    vector<string>::const_iterator i;
	    i = tokens.begin();
        string uname = (*i);
        ++i;
        if(i == tokens.end() ||users.find(uname) == users.end())
        	printError("File system is corrupted");
        string fname = (*i);
        User u = users.find(uname)->second;
        u.addFile(fname);
	}

	fclose(fp);
	fclose(fp2);
}

bool userExists(string uname){
	bool exists = false;
	if(users.find(uname) != users.end())
		exists = true;
	return exists;
}
long fileSize(string username, string filename){
	FILE *fp;
	string fname = ".";
	fname.append(username);
	fname.append("-");
	fname.append(filename);
	fp = fopen(fname.c_str(), "r");
	if(fp == NULL)
		printError("Error opening file");
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	if(size < 0)
		printError("Error reading file");
	return size;

}

void writeFile(string filename, string uname){
	ofstream fileList;
	ofstream newFile;
	User currentUser = users.find(uname)->second;
	string fname = ".";
	string fileEntry = uname + " " + filename;
	fname.append(uname);
	fname.append("-");
	fname.append(filename);
	currentUser.addFile(filename);

	fileList.open("fileList", ios::app);
	newFile.open(fname.c_str());
	
	fileList << fileEntry << endl;
	for (string line; getline(cin, line);) {
		if(!(line.length() > MAX_INPUT)){
			newFile << line << endl;
		}
    }
    fileList.close();
    newFile.close();

}

void readFile(string filename, string uname){
	FILE *fp;
	char line[MAX_LINE_SIZE];
	string fname = ".";
	fname.append(uname);
	fname.append("-");
	fname.append(filename);
	fp = fopen(fname.c_str(), "r");
	if(fp == NULL)
		printError("Error opening file");
	while(fgets(line, MAX_LINE_SIZE, fp)!= NULL){
		if(sizeof(line) > MAX_LINE_SIZE)
			printError("Line too long, file might be corupted");
		cout << line;
	}
	fclose(fp);
}

void printFiles(string username, bool meta){
	FILE *fp;
	char line[MAX_LINE_SIZE];
	fp = fopen("fileList", "rw");
	if(fp == NULL)
		printError("Error opening file");
	while(fgets(line, MAX_LINE_SIZE, fp) != NULL){
		istringstream buf(line);
	    istream_iterator<std::string> beg(buf), end;

	    vector<string> tokens(beg, end);
	    vector<string>::const_iterator i;
	    i = tokens.begin();
        string uname = (*i);
        ++i;
        if(i == tokens.end() ||users.find(uname) == users.end())
        	printError("File system is corrupted");
        string fname = (*i);
        if(username.compare(uname) == 0){
        	if(!meta)
        		cout << fname << endl;
        	else{
        		long size = fileSize(uname, fname);
        		cout << size << " " << fname << endl; 
        	}
        }
	}
	fclose(fp);
}