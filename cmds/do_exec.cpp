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
#include <bits/stdc++.h>
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

string getFileOwner(string filePath){
    string owner = getAttribute(filePath, "user.owner", 2);
    if(owner==""){
        cout << "Error getting owner"<< endl;
        exit(0);
    }
    return owner;
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

    argument = getCorrectedPath(currDirec, argument);
    // string ag2 = argv[1];
    // stringstream geek(ag2);
    // int x = 0; 
    // geek >> x;
    // uid_t muid = (uid_t) x;
    uid_t ruid;
    uid_t euid;
    uid_t suid;
    int retVal = getresuid(&ruid, &euid, &suid);
    if(retVal!=0){
        cout << "Error1"<< endl;
        exit(0);
    }
    cout << ruid << endl;
    cout << euid << endl;
    cout << suid << endl;
    string owner = getFileOwner(argument);
    struct passwd* pwnam = getpwnam(owner.c_str());
    if(pwnam == NULL){
        cout << "Error: Finding the owner"<< endl;
        exit(0);
    }
    uid_t muid = pwnam->pw_uid;
    retVal = setuid(muid);
    if(retVal!=0){
        cout << "error2"<< endl;
        exit(0);
    }
    cout << "Gone"<< endl;
    cout << system(argument.c_str())<< endl;
    cout << "Returned"<< endl;
    retVal = getresuid(&ruid, &euid, &suid);
    if(retVal!=0){
        cout << "Error1"<< endl;
        exit(0);
    }
    cout << ruid << endl;
    cout << euid << endl;
    cout << suid << endl;
    return 0;
}