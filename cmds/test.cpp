#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/xattr.h>

using namespace std;

int main(){
	string path = "/home/mayank/simple_slash/random/";
	string name = "user.user.u1";
	// string value = "rw-";
	// size_t size = (size_t)value.size();
	// setxattr(path.c_str(), name.c_str(), value.c_str(), size, 0);
	char buff[4];
	ssize_t size = getxattr(path.c_str(), name.c_str(), buff, 3);
	if (size < 0) {
		cout << "Return Error"<< endl;
	}
	else{
        buff[3]='\0';
		cout << strlen(buff)<< endl;
		string temp = buff;
		cout << temp << endl;
	}
	return 0;
}