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

string filterName(string name){
    if(name == "user.owner"){
        return string("owner");
    }
    else if(name == "user.group"){
        return string("group");
    }
    else{
        return name.substr(5);
    }
}

void printFileAttributes(string filePath){
    char list[100];
	ssize_t retVal = listxattr(filePath.c_str(), list, 100);
    // cout << retVal<< endl;
    char* p = list;
    string sarr[100];
    int i = 0;
    while(p<(list+retVal)){
        sarr[i] = p;
        // cout << *p << endl;
        p += sarr[i].length()+1;
        string attrName = filterName(sarr[i]);
        int bufSize;
        cout << attrName << " ";
        if(attrName=="owner" || attrName=="group"){
            bufSize = 2;
        }
        else{
            bufSize = 3;
        }
        cout << getAttribute(filePath.c_str(),sarr[i].c_str(),bufSize) << endl;
        i++;
    }
    return;
}

string getUserPermissions(string username, string path){
	string name = "user.user." + username;
	return getAttribute(path, name, 3);
}

string getGroupPermissions(string groupname, string path){
	string name = "user.group."+groupname;
	return getAttribute(path, name, 3);
}

string getGroupUnionPermissions(string currUser, uid_t currUserId, string path){
	struct passwd * pwuid = getpwuid(currUserId);
	gid_t userGroups[10];
    int nog=10;
    int retVal = getgrouplist(currUser.c_str(),pwuid->pw_gid,userGroups,&nog);
    if(retVal<0){
        cout << "Error: In getting groups" << endl;
        return "";
    }
	string finalPermissions = "---";
    for (int i=0;i<nog;i++){
		struct group * grpstruct= getgrgid(userGroups[i]);
		string groupP = getGroupPermissions(grpstruct->gr_name, path);
		if(groupP == ""){
			continue;
		}
		if(groupP[0]=='r'){
			finalPermissions[0]='r';
		}
		if(groupP[1]=='w'){
			finalPermissions[1]='w';
		}
		if(groupP[2]=='x'){
			finalPermissions[2]='x';
		}
    }
	return finalPermissions;
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

int main(int argc, char ** argv){
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
            printFileAttributes(argument);
		}
		else{
			cout << "Error: No permissions to get ACLs" << endl;
			exit(0);
		}
	}

}