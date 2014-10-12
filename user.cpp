using namespace std;
#include <vector>
#include <string>
#include <iostream>
class User{
public:
	string username;
	vector<string> groups;
	vector<string> files;
	User(){
	}
	User(string uname){
		username = uname;
	}
	~User(){
		groups.clear();
		files.clear();
	}

	User (const User &u){
		groups =u.groups;
		username = u.username;
		files = u.files;
	}
	User& operator= (const User &u){
		groups =u.groups;
		username = u.username;
		files = u.files;
		return *this;
	}

	vector<string> getFiles(){
		return files;
	}

	vector<string> getGroups(){
		return groups;
	}

	bool hasFile(string filename){
		for (vector<string>::iterator it = files.begin() ; it != files.end(); ++it){
			if(filename.compare(*it) == 0)
				return true;
		}
		return false;
	}

	void printFiles(){
		vector<string>::iterator i;
		for( i = files.begin(); i != files.end(); ++i)
			cout << *i << " " << endl;
	}

	void printGroups(){
		vector<string>::iterator i;
		for( i = groups.begin(); i != groups.end(); ++i)
			cout << *i << " " << endl;
	}

	void addFile(string filename){
		files.push_back(filename);
	}
};
