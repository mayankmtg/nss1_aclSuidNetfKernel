#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

static string rootDir = "/home/mayank/simple_slash";

// returns the directory that the argument file is contained in
string getFilePathDirectory(string currDirec, string path){
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
	// cout << "mtg" << endl;
	// cout << sub << endl;
	char buf[300];
	char *res = realpath(sub.c_str(), buf);
	if(res){
		string returnPath = buf;
		return returnPath;
	}
	else{
		return "Error";
	}
	return sub;
}

// returns the file name at the end of the argument
string getFilePathName(string currDirec, string path){
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
	if(argc != 2){
		cout << "Incorrect number of arguments" << endl;
		exit(0);
	}
	string argument = argv[1];
	
	char buff[200];
	getcwd(buff, 200);
	string currDirec = buff;
	// cout << argument << endl;
	string extractDirec = getFilePathDirectory(currDirec, argument);
        string extractName = getFilePathName(currDirec, argument);
	string message ="";
	if(extractDirec == "Error"){
		message += "Error: Incorrect path to file\n";
	}
	else if(extractDirec.length() < rootDir.length()){
		message += "Error: Unauthorised Access\n";
	}
	else if(getFileExtension(extractName)==".m" || getFileExtension(extractName)==".d"){
		message += "Error: File cannot have .m or .d extension";
	}
	else if(filePathExists(argument)){
		if(fileWriteAllowed(currUser, argument)){
			message = "Input Stream. Q to Quit.\n";
			send(connFd, (void *)message.c_str(), 300, 0);
			bzero(test, 301);
			recvBytes = recv(connFd, test, 300, 0);
			if(recvBytes == (ssize_t)0){
				pthread_exit(NULL);
			}

			response = test;
			appendToFile(argument,response);
			message = "";
		}
		else{
			message = "Error: No permissions to write to this file\n";
		}
	}
	else if(direcWriteAllowed(currUser, extractDirec)){
		string metaData = "";
		message = "";
		while(1){
			message += "Enter File Owner (d for default): ";
			string usergroup = getUserandGroupForDirec(extractDirec);
			string fileUser = usergroup.substr(0, usergroup.find(' '));
			send(connFd, (void *)message.c_str(), 300, 0);
			bzero(test, 301);
			recvBytes = recv(connFd, test, 300, 0);
			if(recvBytes == (ssize_t)0){
				pthread_exit(NULL);
			}
			response = test;
			if(response == "d"){
				response = fileUser;
			}
			if(authenticate_user(response)){
				metaData+=response + "\n";
			}

			message = "Enter File Group (d for default): ";
			string fileGroup= usergroup.substr(usergroup.find(' ')+1);
			send(connFd, (void *)message.c_str(), 300, 0);
			bzero(test, 301);
			recvBytes = recv(connFd, test, 300, 0);
			if(recvBytes == (ssize_t)0){
				pthread_exit(NULL);
			}
			response = test;
			if(response == "d"){
				response = fileGroup;
			}
			if(authenticate_group(response)){
				metaData+=response + "\n";
				break;
			}
			metaData = "";
			message = "Error: does not exist\n";
		}
		appendToFile(argument+".m", metaData);

		message = "Input Stream. Q to Quit.\n";
		send(connFd, (void *)message.c_str(), 300, 0);
		bzero(test, 301);
		recvBytes = recv(connFd, test, 300, 0);
		if(recvBytes == (ssize_t)0){
			pthread_exit(NULL);
		}
		response = test;
		appendToFile(argument,response);
		message = "";
	}
	else{
		message += "Error: No write permissions for the directory\n";
	}

	return 0;
}
