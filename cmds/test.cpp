#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

int is_regular_file(const char *path)
{
	struct stat path_stat;
	stat(path, &path_stat);
	return S_ISREG(path_stat.st_mode);
}

int main(){
	string path = "/home/mayank/Mayank/nss1_aclSuidNetfKernel/cmds/";
	cout << is_regular_file(path.c_str()) << endl;
	return 0;
}
