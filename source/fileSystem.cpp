using namespace std;
#include <list>
#include "utilities.cpp"
#include "ACL.cpp"
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <grp.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

static map<string, vector<string>>  users;
static ACL acl;

class FileSystem{
public:
	string username;
	string groupname;

	FileSystem(){
		setUserAndGroup();
		initACL();
	}

	void initACL(){
		string line;
		string token;

		DIR *pDIR;
        struct dirent *entry;
        if (pDIR = opendir("/fileSystem") ){
            while(entry = readdir(pDIR)){
            	string username;
            	string objname;
            	string meta;
            	int mIndex = 0;
            	//File entry is username.objectname
            	//Metadata entry is username.objectname.meta
                if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
                	istringstream iss(entry->d_name);

					//First token is username
					getline(iss, token, '.');
					if(token.empty())
						printError("Usage user.objectname or user.objectname.meta");
					username = token;
					validNameString(username, false);

					//Second token is object name
					getline(iss, token);
					if(token.empty())
						printError("Usage user.objectname or user.objectname.meta");
					objname = token;
					validNameString(objname, true);

					if(!userExists(username))
						printError("Invalid file owner in fileSystem");

					//Last (optional) token is metaData signature
					string name(objname);
					mIndex = objname.find_last_of(".");
					if(mIndex > 0){
				  		meta = objname.substr(mIndex+1);
				  		objname = objname.substr(0, mIndex);
					}

					//If we are seeing the actual file, check if metadata exists
					if(meta != "meta"){
						string str(entry->d_name);
						if(!fileExists(str + ".meta"))
							printError("Couldn't find a file's metadata");
					}
					//If we are seeing the metadata file, check real file exists and create entry
					else{
						if(!fileExists(username + "." + objname))
							printError("Couldn't find the file for a metadata file");
						createACLEntry(username, objname);
					}
                }
            }
            closedir(pDIR);
        }
        else
        	printError("Could not open fileSystem folder");
	}

	bool userExists(string uname){
		if(getpwnam(uname.c_str()) != NULL)
			return true;
		return false;
	}

	bool groupExists(string gname){
		if(getgrnam(gname.c_str()) != NULL && gname.compare("*") != 0)
			return true;
		return false;
	}

	void validateUserAndGroup(string username, string groupname){
		validNameString(username, false);
		validNameString(groupname, false);
		if(!userExists(username))
			printError("Invalid user");
		if(!groupExists(groupname) && groupname.compare("*") != 0)
			printError("Invalid group");
	}

	void setUserAndGroup(){
		uid_t  uid = getuid();
		uid_t  gid = getgid();
		struct passwd *currentUser = getpwuid(uid);
		struct group *currentGroup = getgrgid(gid);
		username = currentUser->pw_name;
		groupname = currentGroup->gr_name;
	}

	bool hasFile(string uname, string objname){
		vector<string>:: iterator findFile;
		vector<string> userFiles = users.find(uname)->second;
		findFile = find(userFiles.begin(), userFiles.end(), objname);
		if(findFile == userFiles.end())
			return false;
		return true;
	}

	bool fileExists(string filename){
	   	DIR *dirp = opendir("/fileSystem");
	   	struct dirent *dp;
	   	while ((dp = readdir(dirp)) != NULL){
	    	if (strcmp(dp->d_name, filename.c_str()) == 0) {
				closedir(dirp);
				return true;
			}
		}
	   closedir(dirp);
	   return false;
	}

	void createACLEntry(string username, string objname){
		ACLEntry *acle = new ACLEntry(objname);
		string line;
		string token;

		//Add filename to owner
		if(users.find(username) == users.end()){
			vector<string> v = {objname};
			users[username] = v;
		}
		else{
			vector<string> *currentFiles = &(users.find(username)->second);
			currentFiles->push_back(objname);
		}

		//Add entry to acl object in memory
		string path = "/fileSystem/" + username + "." + objname + ".meta";
		ifstream aclFile(path.c_str(), ios::binary);
		if(aclFile.is_open())
		{
			//First lines should be encrypted Key, then empty line, and iv skip
			getline(aclFile, line);
			if(line.empty())
				printError("Invalid meta file (No key)");
			while (!line.empty())
				getline(aclFile, line);

			getline(aclFile, line);
			if(line.empty())
				printError("Invalid meta file (No iv)");
			while (!line.empty())
				getline(aclFile, line);

			//Next line is the size, skip
			getline(aclFile, line);
			if(line.empty())
				printError("Invalid meta file");

			//Read acl permission lines, create entry
			//Format: u2.g1 rwp
			while(getline(aclFile, line) && !line.empty()) {
				if(!(line.length() > MAX_INPUT)){
					string user;
					string group;
					string permissions;

					//First token is username
					istringstream iss(line);
					std::getline(iss, token, '.');
					if(token.empty())
						printError("Invalid aclFile");
					user = token;
					getline(iss, token);

					//Second token is groupname
					istringstream iss2(token);
					vector<string> tok{istream_iterator<string>{iss2}, istream_iterator<string>{}};
					group = tok.front();
					if(group.empty())
						printError("Invalid aclFile");
					permissions = tok.back();
					if(permissions.empty())
						printError("Invalid aclFile");
					validateUserAndGroup(user, group);
					validPermissions(permissions);
					
					acle->permissions[make_pair(user, group)] = permissions;
				}
			}
			acl.ace[username + "." + objname] = *acle;
			aclFile.close();
		}
		else{
			printError("Couldn't open file's metaData");
		}
	}

	int decryptLine(char *line, unsigned char* k, unsigned char* decryptedtext, unsigned char *v){
		int decryptedtext_len;
		unsigned char *key = k;
		unsigned char *iv = v;
		unsigned char *ciphertext = (unsigned char *)line;
		int ciphertext_len = (int)strlen((const char *)line);

		ERR_load_crypto_strings();
		OpenSSL_add_all_algorithms();
		OPENSSL_config(NULL);

		decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv,
		decryptedtext);
		decryptedtext[decryptedtext_len] = '\0';

		EVP_cleanup();
		ERR_free_strings();
		return decryptedtext_len;
	}

	int encryptLine(char *line, unsigned char* k, unsigned char* ciphertext, unsigned char* v){
		int ciphertext_len;
		unsigned char *key = k;
		unsigned char *iv = v;
		unsigned char *plaintext = (unsigned char *)line;
		int textLen = (int)strlen((const char *)plaintext);
		unsigned char *decryptedtext;

		ERR_load_crypto_strings();
		OpenSSL_add_all_algorithms();
		OPENSSL_config(NULL);
		ciphertext_len = encrypt(plaintext, textLen, key, iv, ciphertext);
		ciphertext[ciphertext_len] = '\0';

		EVP_cleanup();
		ERR_free_strings();

		return ciphertext_len;
	}

	int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
	unsigned char *iv, unsigned char *ciphertext)
	{
		EVP_CIPHER_CTX *ctx;
		int len;
		int ciphertext_len;

		if(!(ctx = EVP_CIPHER_CTX_new())) 
			printError("Context couldn't be initialized");
		if(EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv) != 1)
			printError("Encryption operation couldn't be initialized");
		if(EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len) != 1)
			printError("Encrypt update failed");
		ciphertext_len = len;
		if(EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1) 
			printError("Couldn't finilize encryption");
		ciphertext_len += len;

		EVP_CIPHER_CTX_free(ctx);
		return ciphertext_len;
	}

	int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
	unsigned char *iv, unsigned char *plaintext)
	{
		EVP_CIPHER_CTX *ctx;
		int len;
		int plaintext_len;
		EVP_CIPHER_CTX_set_padding(ctx, 0);

		if(!(ctx = EVP_CIPHER_CTX_new())) 
			printError("Context couldn't be initialized");
		if(EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv) != 1)
			printError("Decryption operation couldn't be initialized");
		if(EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len) != 1)
			printError("Decrypt update failed");
		plaintext_len = len;
		if(EVP_DecryptFinal_ex(ctx, plaintext + len, &len) != 1){
			ERR_print_errors_fp(stdout);
			printError("Couldn't finilize decryption");
		}
		plaintext_len += len;

		EVP_CIPHER_CTX_free(ctx);
		return plaintext_len;
	}
	void getEncKey(string objectname, char *key, char *iv){
		string path = "/fileSystem/" + objectname + ".meta";
		string tempKey;
		string tempIv;
		string line;
		ifstream aclFile(path.c_str(), ios::binary);
		if(aclFile.is_open())
		{
			getline(aclFile, line);
			if(line.empty())
				printError("Invalid meta file (No key)");
			while (!line.empty()){
				tempKey += line;
				getline(aclFile, line);
			}

			getline(aclFile, line);
			if(line.empty())
				printError("Invalid meta file (No iv)");
			while (!line.empty()){
				tempIv += line;
				getline(aclFile, line);
			}
		}
		else
			printError("Couldn't read encrypted key");
		strcpy(key, tempKey.c_str());
		strcpy(iv, tempIv.c_str());
	}
		
};