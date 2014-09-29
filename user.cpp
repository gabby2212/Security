using namespace std;
#include <list>
class User{
public:
	string username;
	list<string> groups;
	list<string> files;
	User(){
	}
	User(string uname){
		username = uname;
	}
	~User(){
		groups.clear();
	}

	User (const User &u){
		groups =u.groups;
		username = u.username;
	}
	User& operator= (const User &u){
		groups =u.groups;
		username = u.username;
		return *this;
	}

	bool hasFile(string filename){
	  for (list<string>::iterator it = files.begin() ; it != files.end(); ++it){
	  	if(filename.compare(*it) == 0)
	  		return true;
	  }
	  return false;
	}

	void addFile(string filename){
		files.push_back(filename);
	}
};