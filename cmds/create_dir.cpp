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

// returns the directory that the file is contained in
string getPathDirectory(string currDirec, string path){
    if(path[0]=='/'){
		path = rootDir + path;
	}
	else{
		path = currDirec +"/"+ path;
	}
    string filename_copy(path);
	reverse(filename_copy.begin(), filename_copy.end());
	int pos = filename_copy.find('/');
	string sub = filename_copy.substr(pos+1);
	reverse(sub.begin(), sub.end());
    char buf[300];
	char *res = realpath(sub.c_str(), buf);
	if(res){
		string returnPath = buf;
		return returnPath;
	}
	else{
		cout << "Error: Incorrect path to file"<< endl;
		exit(0);
	}
}

// returns the filename for the path entered
string getPathName(string currDirec, string path){
    if(path[0]!='/'){
		path = currDirec+ "/" + path;
	}
	string filename_copy(path);
	reverse(filename_copy.begin(), filename_copy.end());
	int pos = filename_copy.find('/');
	string sub = filename_copy.substr(0,pos);
	reverse(sub.begin(), sub.end());
	return sub;
}

int main(int argc, char** argv){
    if(argc!=2){
		cout << "Error: Incorrect number of arguments" << endl;
		exit(0);
	}
    string argument = argv[1];
    
    // Process Variables
	string currDirec = getProcessDirectory();
	uid_t currUid = getProcessRuid();
	string currUser = getProcessUsername(currUid);
	string currGroup = getProcessGroupname(currUid);
    // 
	cout << getPathDirectory(currDirec, argument) << endl;
	cout << getPathName(currDirec, argument) << endl;


    return 0;
}