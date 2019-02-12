#include <bist/stdc++.h>
using namespace std;

static string rootDir = "/home/mayank/simple_slash";

string getCorrectedPath(string currDirec, string path){
	if(path[0]=='/'){
		path = rootDir + path; // absolute path
	}
	else{
		path = currDirec + "/" + path; // relative path
	}
	char buf[300];
	char *res = realpath(path.c_str(), buf);
	if(res){
		string returnPath = buf;
		return returnPath;
	}
	else{
		return "Error";
	}

}
string getUserandGroupForDirec(string metafilepath){
	string returnString="";
	ifstream infile;
	metafilepath = metafilepath + ".d";
	infile.open(metafilepath.c_str());
	string userLine;
	while(infile >> userLine){
		returnString +=userLine + " ";
	}
	infile.close();
	return returnString.substr(0,returnString.length()-1);
}

bool direcReadAllowed(string currUser, string currGroup, string filepath){
	string usergroup = getUserandGroupForDirec(filepath);
	string fileUser = usergroup.substr(0, usergroup.find(' '));
	string fileGroup= usergroup.substr(usergroup.find(' ')+1);
	
	if(fileUser == currUser){
		return true;
	}
	else if(fileGroup == currGroup){
		return true;
	}
	else if(userBelongsToGroup(currUser, fileGroup)){
		return true;
	}
	else{
		return false;
	}

}
int main(int argc, char** argv){
	if(argc!=2){
		cout << "Error: Incorrect number of arguments"<< endl;
		exit(0);
	}
	string argument = argv[1];
	char buff[200];
	getcwd(buff, 200);
	string currDirec = buff;
	argument = getCorrectedPath(currDirec, argument);
	if(argument=="Error"){
		message += "Error: Could not find directory\n";
	}
	else if(argument.length() < rootDir.length()){
		message += "Error: Unauthorised Access\n";
	}
	else if(argument.substr(0,rootDir.length()) != rootDir ){
		message += "Error: Unauthorised Access\n";
	}
	else{
		if(direcReadAllowed(currUser, currGroup, argument)){
			DIR *dr = opendir(argument.c_str());
			struct dirent *de;
			while ((de = readdir(dr)) != NULL){
				string this_inode(de->d_name);
				bool not_relative_dir = this_inode!="." && this_inode!="..";
				if(not_relative_dir){
					if(getFileExtension(this_inode)!="m" && getFileExtension(this_inode)!="d"){
						string inode_path = argument + "/" + this_inode;
						message += this_inode + " " + getUserandGroupForFile(inode_path) + getUserandGroupForDirec(inode_path) + "\n";
					}
				}
			}
		}
		else{
			message += "Error: No permissions to read this directory\n";
		}
	}

	return 0;
}