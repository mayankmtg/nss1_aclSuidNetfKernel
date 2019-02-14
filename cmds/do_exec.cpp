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

int main(int argc, char** argv){
    if(argc!=2){
		cout << "Error: Incorrect number of arguments" << endl;
		exit(0);
	}
    string ag2 = argv[1];
    stringstream geek(ag2);
    int x = 0; 
    geek >> x;
    uid_t muid = (uid_t) x;
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
    retVal = setuid(muid);
    system("./uid");
    if(retVal!=0){
        cout << "error2"<< endl;
        exit(0);
    }
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