using namespace std;
#include <cstdlib>
#include <cstdio>
#include <map>
#include <iostream>
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


std::map<string, User>  users;
int numUsers;


void printError(string err){
	cerr << err << endl;
	exit(0);
}

void setUp(){
	FILE *fp;
	char line[MAX_LINE_SIZE];
	string tok;
	numUsers = 0;
	int userArraySize = INITIAL_NUMBER_OF_USERS;

	fp = fopen("userfile.txt", "r");
	if(fp == NULL)
		printError("Couldn't find userfile");

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
	fclose(fp);
}

bool userExists(string uname){
	bool exists = false;
	if(users.find(uname) != users.end())
		exists = true;
	return exists;
}
