class User{
private:
	string username;
	vector<string> files;
	vector<string> groups;
	int numGroups;
public:
	User(){
	}
	User(string uname, vector<string> g, int nGroups){
		groups = g;
		numGroups = numGroups;
		username = uname;
	}
	~User(){
		groups.clear();
	}

	User (const User &u){
		groups =u.getGroups();
		numGroups = u.getNumGroups();
		username = u.getUname();
	}
	User& operator= (const User &u){
		groups = u.getGroups();
		numGroups = u.getNumGroups();
		username = u.getUname();
		return *this;
	}

	string getUname() const{
		return username;
	}

	vector<string> getGroups() const{
		return groups;
	}

	int getNumGroups() const{
		return numGroups;
	}

	bool hasFile(string filename){
	  for (vector<string>::iterator it = files.begin() ; it != files.end(); ++it){
	  	if(filename.compare(*it) == 0)
	  		return true;
	  }
	  return false;
	}

	void addFile(string filename){
		files.push_back(filename);
	}
};