using namespace std;
#include <list>
class User{
public:
	string username;
	list<string> groups;
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
};