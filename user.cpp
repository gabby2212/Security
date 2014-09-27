using namespace std;
#include <list>
class User{
public:
	string username;
	list<string> files;
	list<string> groups;
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

	bool hasFile(string filename){
	  for (list<string>::iterator it = files.begin() ; it != files.end(); ++it){
	  	if(filename.compare(*it) == 0)
	  		return true;
	  }
	  return false;
	}
};