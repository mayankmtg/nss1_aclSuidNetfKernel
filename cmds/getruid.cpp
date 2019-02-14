#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
using namespace std;
int main(){

    uid_t ruid;
    uid_t euid;
    uid_t suid;
    int retVal = getresuid(&ruid, &euid, &suid);
    if(retVal==0){
        cout << ruid << endl;
        cout << euid << endl;
        cout << suid << endl;
    }
    // ruid is of significance in a setuid program
    struct passwd * pwuid = getpwuid(ruid);
    string username = "";
    if(pwuid){
        username = string(pwuid->pw_name);
    }

    cout << username << endl;

    return 0;
}