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

bool direcExists(string pathname){
	DIR* dir = opendir(pathname.c_str());
	if (dir){
		closedir(dir);
		return true;
	}
	else{
		return false;
	}
}

string getAttribute(string path, string name, int size){
	// size is the size of the return expected since
	// getxattr gives garbage after the intended size
	char buff[size+1];
	ssize_t retSize = getxattr(path.c_str(), name.c_str(), buff, size);
	if (retSize < 0) {
		// cout << "Not found attribute "<< name << endl;
		return "";
	}
	buff[size]='\0';
	string temp = buff;
	return temp;
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


bool fileWriteAllowed(string currUser, uid_t currUserId, string path){
    string userP = getUserPermissions(currUser,path);
    string groupP = getGroupUnionPermissions(currUser,currUserId, path);
    if(userP!=""){
        if(userP[1]=='w'){
            return true;
        }
        else{
            return false;
        }
    }
    if(groupP[1]=='w'){
        return true;
    }
    return false;
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

bool setOwnerAttributes(string path, string currUser, string currGroup){
	bool t1 = setAttribute(path, string("user.owner"),currUser);
	bool t2 = setAttribute(path, string("user.group"),currGroup);
	// TODO: replace this group from currGroup to the group of directory
	bool t3 = setAttribute(path, string("user.user."+currUser),string("rwx"));
	bool t4 = setAttribute(path, string("user.group."+currGroup),string("r--"));
	// TODO: replace this group from currGroup to the group of directory
	bool fileFinish = t1 && t2 && t3 && t4;
	return fileFinish;
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
	string direcPath = getPathDirectory(currDirec, argument);
	string direcName = getPathName(currDirec, argument);
	argument = direcPath + "/" + direcName;
	if(direcPath.length() < rootDir.length()){
		cout << "Error: Unauthorised Access" << endl;
		exit(0);
	}
	else if(direcExists(argument)){
		cout << "Error: Invalid directory name" << endl;
		exit(0);
	}
	else if(fileWriteAllowed(currUser, currUid, direcPath)){
		if(mkdir(argument.c_str(), 0777) == -1){
			cout << "Error: Unable to make directory" << endl;
		}
		else{
			// directory creation successful
			if(!setOwnerAttributes(argument, currUser, currGroup)){
				string cmd("rm -r ");
				cmd+=argument;
				system(cmd.c_str());
				cout << "Error: Creating directory" << endl;
			}
			// else directory created and attributes set
		}
	}
	else{
		cout << "Error: No permission to create directory" << endl;
	}



    return 0;
}
