#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <iostream>
#include <list>
#include <fstream>
#include <dirent.h>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <algorithm>
#include <pthread.h>
#include <sys/xattr.h>
#include <pwd.h>
#include <grp.h>
#include <fstream>

using namespace std;

static string rootDir = "/home/mayank/simple_slash";
uid_t getProcessRuid(){
	uid_t ruid;
    uid_t euid;
    uid_t suid;
    int retVal = getresuid(&ruid, &euid, &suid);
    if(retVal!=0){
		cout << "Error: In getting ruid"<< endl;
		exit(0);
	}
	return ruid;
}
string getProcessUsername(uid_t ruid){
	struct passwd * pwuid = getpwuid(ruid);
    string username = "";
    if(pwuid){
        username = string(pwuid->pw_name);
    }
	else{
		cout << "Error: In getting username"<< endl;
		exit(0);
	}
	return username;
}
string getProcessGroupname(uid_t ruid){
	struct passwd * pwuid = getpwuid(ruid);
    string groupname = "";
    if(pwuid){
		struct group * grpstruct= getgrgid(pwuid->pw_gid);
		if(grpstruct!=NULL){
			groupname = grpstruct->gr_name;
		}
		else{
			cout << "Error: In getting groupname"<< endl;
		}
		return groupname;
    }
	else{
		cout << "Error: In getting groupname"<< endl;
		exit(0);
	}
	return groupname;
}
string getProcessDirectory(){
	char pwd[200];
	char* check = getcwd(pwd, 200);
	if(check == NULL){
		cout << "Error: In getting present working directory"<< endl;
		exit(0);
	}
	string retString = pwd;
	return retString;
}

string getAttribute(string path, string name, int size){
	// size is the size of the return expected since
	// getxattr gives garbage after the intended size
	char buff[size+1];
	ssize_t retSize = getxattr(path.c_str(), name.c_str(), buff, size);
	if (retSize < 0) {
		return "";
	}
	buff[size]='\0';
	string temp = buff;
	return temp;
}

// returns the corrected paths
string getCorrectedPath(string currDirec, string path){
	if(path[0]=='/'){
		path = rootDir + path; // absolute path
	}
	else{
		path = currDirec + "/" + path; // relative path
	}
	char buf[300];
	// cout << path << endl;
	char *res = realpath(path.c_str(), buf);
	if(res){
		string returnPath = buf;
		return returnPath;
	}
	else{
		cout << "Error: In path resolution"<< endl;
		exit(0);
	}
}

bool aclManipAllowed(string currUser, string path){
	string owner = getAttribute(path, string("user.owner"),2);
    if(owner == ""){
        cout << "Error: In getting attributes"<< endl;
    }
    if(currUser == "fr" || currUser == owner) {
        return true;
    }
    return false;
}

bool setAttribute(string path, string name, string value){
	// size is the size of the return expected since
	// getxattr gives garbage after the intended size
    int retval = setxattr(path.c_str(), name.c_str(), value.c_str(), (size_t)value.size(), 0);
    if(retval != 0){
        cout << "Error: Setting Attributes"<< endl;
        return false;
    }
    return true;
}

bool setFileAttributes(string filePath, string name, string value){
    name = "user."+ name;
    return setAttribute(filePath, name, value);
}

bool validateName(string name){
    size_t pos = name.find('.');
    if(pos==string::npos){
        return false;
    }
    string typeName = name.substr(0,pos);
    string uName = name.substr(pos+1);
    if(typeName != "user" && typeName != "group"){
        return false;
    }
    struct passwd* pwnam = getpwnam(uName.c_str());
    if(pwnam == NULL){
        return false;
    }
    if(uName.length()!=2){
        return false;
    }
    return true;
}

bool validateValue(string value){
    if(value.length()!=3){
        return false;
    }
    if(value[0]!='r' && value[0]!='-'){
        return false;
    }
    else if(value[1]!='w' && value[1]!='-'){
        return false;
    }
    else if(value[2]!='x' && value[2]!='-'){
        return false;
    }
    return true;
}



int main(int argc, char ** argv){
    if(argc!=4){
		cout << "Error: Incorrect number of arguments" << endl;
		exit(0);
	}
    string argument = argv[1];
    string name = argv[2];
    string value = argv[3];
    if(!validateName(name) || !validateValue(value)){
        cout << "Error: Incorrect format of arguments" << endl;
        exit(0);
    }

    // Process Variables
	string currDirec = getProcessDirectory();
	uid_t currUid = getProcessRuid();
	string currUser = getProcessUsername(currUid);
	string currGroup = getProcessGroupname(currUid);
    // 

    argument = getCorrectedPath(currDirec, argument);
    if(argument.length() < rootDir.length()){
		cout << "Error: Unauthorised Access" << endl;
		exit(0);
	}
	else if(argument.substr(0,rootDir.length()) != rootDir ){
		cout << "Error: Unauthorised Access" << endl;
		exit(0);
	}
    else {
		if(aclManipAllowed(currUser, argument)){
            if(!setFileAttributes(argument, name, value)){
                cout << "Error: Unable to set attribute"<< endl;
                exit(0);
            }
		}
		else{
			cout << "Error: No permissions to set ACLs" << endl;
			exit(0);
		}
	}
    return 0;
}