using namespace std;

void writeACL(){
	string line;

	//Write to ACL
	ofstream newACL(fname.c_str());
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