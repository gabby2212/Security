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
#include <algorithm>
#include <regex>

#define MAX_USERNAME_SIZE 25
#define VALID_CHARS ".abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_"
#define ERROR -1

void sig_handle(int s){
	exit(0);
}

void printError(string err){
	cerr << err << endl;
	exit(1);
}

void validNameString(string name){
	if(name.length() > MAX_USERNAME_SIZE)
		printError("Name too long");
	if(name.compare("*") == 0)
		return;
	if (name.find_first_not_of(VALID_CHARS) != string::npos)
		printError("Invalid characters");
}

string getObjectName(string username, string objname){
	if(objname.find("+") == string::npos){
		validNameString(objname);
		return username + "." + objname;
	}
	else{
		string targetUser = objname.substr(0, objname.find("+"));
		string targetObject = objname.substr(objname.find("+") + 1, objname.length());
		validNameString(targetUser);
		validNameString(targetObject);
		return targetUser + "." + targetObject;
	}
}

string getOwner(string objname){
	if(objname.find(".") == string::npos)
		printError("Invalid object");
	return objname.substr(0, objname.find("."));
}

void validPermissions(string permissions){
	bool valid = true;
 	regex rx("r*w*x*p*v*");	
	valid = regex_match(permissions.begin(), permissions.end(), rx);
	if(!valid)
		printError("Invalid permissions");
}
