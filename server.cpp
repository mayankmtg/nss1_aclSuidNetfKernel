#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <algorithm>
#include <pthread.h>
using namespace std;

//static int connFd;
static string rootDir = "/home/mayank/Sem-8/NSS/nss0_fileSystem/direc/root";
static string homeDir = "/simple_home";


bool authenticate_user(string currUser){
	ifstream infile;
	string user_file = "/../users.txt";
	user_file = rootDir + user_file;
	infile.open(user_file.c_str());
	string userLine;
	while(infile >> userLine){
		if(userLine == currUser){
			return true;
		}
	}
	infile.close();
	return false;
}

bool authenticate_group(string currGroup){
	ifstream infile;
	string groupFile = "/../groups.txt";
	groupFile = rootDir + groupFile;
	infile.open(groupFile.c_str());
	string groupLine;
	while(infile >> groupLine){
		if(groupLine.substr(0,groupLine.find(':')) == currGroup){
			return true;
		}
	}
	infile.close();
	return false;
}

bool addUserToUserFile(string username){
	fstream outfile;

	string user_file = rootDir + "/../users.txt";
	outfile.open(user_file.c_str(), ofstream::app);
	outfile << username << endl;
	outfile.close();
	return true;
}

bool addUserToGroupFile(string username, string groupname){
	ifstream infile;
	string group_file = rootDir + "/../groups.txt";
	infile.open(group_file.c_str());
	string fileLine;
	string newString="";
	bool groupFound = false;
	while(infile >> fileLine){

		if( fileLine.substr(0,fileLine.find(':')) == groupname ){
			groupFound = true;
			newString += fileLine + "," + username + "\n";
		}
		else{
			newString +=fileLine + "\n";
		}
	}
	infile.close();
	fstream outfile;
	if(groupFound==false){
		outfile.open(group_file.c_str(), ofstream::app);
		outfile << groupname << ":" <<username << endl;
	}
	else{
		outfile.open(group_file.c_str());
		outfile << newString;
	}
	outfile.close();
	return true;
}

bool addUserToSimpleHome(string username, string groupname){
	string userDir = rootDir + homeDir + "/" + username;
	if(mkdir(userDir.c_str(), 0777) == -1){
		return false;
	}
	string meta_file = userDir + ".d";
	fstream outfile;
	outfile.open(meta_file.c_str(), ofstream::app);
	outfile << username << endl;
	outfile << groupname << endl;
	outfile.close();
	return true;
}

bool createNewUser(string username, string groupname){
	bool addToUser = addUserToUserFile(username);
	if(!addToUser){
		return false;
	}
	bool addToGroup = addUserToGroupFile(username, groupname);
	if(!addToGroup){
		return false;
	}
	bool addToHome = addUserToSimpleHome(username, groupname);
	if(!addToHome){
		return false;
	}
	return true;
}

string getFileExtension(string filename) {
	string filename_copy(filename);
	reverse(filename_copy.begin(), filename_copy.end());
	int pos = filename_copy.find('.');
	string sub = filename_copy.substr(0,pos);
	reverse(sub.begin(), sub.end());
	return sub;
}


void *closing_seq (string message, int connFd){
	char test[300];
	send(connFd, (void *)message.c_str(), 300, 0);
	cout << endl << "Closing thread and conn" << endl;
	close(connFd);
	void *returnVal;
	return returnVal;
}

string getUserandGroupForFile(string metafilepath){
	string returnString="";
	ifstream infile;
	metafilepath = metafilepath + ".m";
	infile.open(metafilepath.c_str());
	string userLine;
	while(infile >> userLine){
		returnString +=userLine + " ";
	}
	infile.close();
	return returnString.substr(0,returnString.length()-1);
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

// path is either like /simple_home/user/... or like direc1/direc2/...
// returns the complete path with rootDir concatenated
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
string getFilePathDirectory(string currDirec, string path){
	if(path[0]=='/'){
		path = rootDir + path;
	}
	else{
		path = currDirec+ "/" + path;
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
		return "Error";
	}
	return sub;
}

string getFilePathName(string currDirec, string path){
	if(path[0]=='/'){
		path = rootDir + path;
	}
	else{
		path = currDirec+ "/" + path;
	}
	string filename_copy(path);
	reverse(filename_copy.begin(), filename_copy.end());
	int pos = filename_copy.find('/');
	string sub = filename_copy.substr(0,pos);
	reverse(sub.begin(), sub.end());
	return sub;
}
string getFileContents(string filename){
	ifstream infile;
	infile.open(filename.c_str());
	string userLine;
	string returnString="";
	while(getline(infile, userLine)){
		returnString+=userLine + "\n";
	}
	infile.close();
	return returnString;
}

string getDispPath(string path){
	char buf[300];
	char *res = realpath(path.c_str(), buf);
	if(res){
        string returnPath = buf;
		return returnPath.substr(rootDir.length(), returnPath.length());
    }
	else{
        return "Error";
    }
}

bool userBelongsToGroup(string username, string groupname){
	if(groupname==("all")){
		return true;
	}
	else{
		ifstream infile;
		string group_file = rootDir + "/../groups.txt";
		infile.open(group_file.c_str());
		string fileLine;
		while(infile >> fileLine){
			if( fileLine.substr(0,fileLine.find(':')) == groupname ){
				string allGroupUsers = fileLine.substr(fileLine.find(':')+1);
				while(allGroupUsers.find(',') != string::npos){
					int pos = allGroupUsers.find(',');
					if(username == allGroupUsers.substr(0,allGroupUsers.find(','))){
						infile.close();
						return true;
					}
					allGroupUsers = allGroupUsers.substr(allGroupUsers.find(',')+1);
				}
				if(username == allGroupUsers){
					infile.close();
					return true;
				}
				else{
					infile.close();
					return false;
				}
			}
		}
		infile.close();
	}
	return false;


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

bool direcWriteAllowed(string currUser, string filepath){
	string usergroup = getUserandGroupForDirec(filepath);
	string fileUser = usergroup.substr(0, usergroup.find(' '));
	
	if(fileUser == currUser){
		return true;
	}
	else{
		return false;
	}
}
bool fileReadAllowed(string currUser, string currGroup, string filepath){
	string usergroup = getUserandGroupForFile(filepath);
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

bool fileWriteAllowed(string currUser, string filepath){
	string usergroup = getUserandGroupForFile(filepath);
	string fileUser = usergroup.substr(0, usergroup.find(' '));
	
	if(fileUser == currUser){
		return true;
	}
	else{
		return false;
	}
}

bool filePathExists(string pathname){
	std::ifstream infile(pathname.c_str());
    return infile.good();
}

bool direcPathExists(string pathname){
	DIR* dir = opendir(pathname.c_str());
	if (dir){
		closedir(dir);
		return true;
	}
	else{
		return false;
	}
}


void appendToFile(string filename, string content){
	fstream outfile;
	outfile.open(filename.c_str(), ofstream::app);
	outfile << content;
	outfile.close();
}

void *serverHandler (void* dummyPt){
	cout << "Thread No: " << pthread_self() << endl;
	char test[300];
	// 'message' to be sent to the server
	string message="Enter your username: ";
	int connFd =  *((int *)dummyPt);
	send(connFd, (void *)message.c_str(), 300, 0);
	
	// 'response' received from the server
	bzero(test, 301);
	recv(connFd, test, 300, 0);
	string response = test;
	string threadUser = response;
	string threadGroup= response;
	// 'response' contains username entered by client;
	
	string currDirec;
	string currUser;
	string currGroup;

	if(!authenticate_user(response)){
		message = "User does not exist. Do you want to create a new one? (yes/no): ";
		send(connFd, (void *)message.c_str(), 300, 0);
		
		bzero(test, 301);
		recv(connFd, test, 300, 0);
		response = test;

		if(response == "yes"){
			bool newUserBool = createNewUser(threadUser, threadGroup);
			if(newUserBool){
				currUser = threadUser;
				currGroup= threadGroup;
				currDirec = rootDir + homeDir + "/" + threadUser;
			}
			else{
				closing_seq("Error creating user", connFd);
			}
		}
		else{
			return closing_seq("Closing Connection", connFd);
		}
	}
	else{
		currUser = threadUser;
		currGroup= threadGroup;
		currDirec= rootDir + homeDir + "/" + threadUser;
	}
	if(getDispPath(currDirec)!="Error"){
		message = getDispPath(currDirec) + "$ ";
		send(connFd, (void *)message.c_str(), 300, 0);
	}
	else{
		return closing_seq("Error in Prompt", connFd);
	}


	bool loop = false;
	while(!loop){
		bzero(test, 301);
		recv(connFd, test, 300, 0);
		string response = test;
		int pos = response.find(' ');
		string command = response.substr(0, pos);
		string argument = response.substr(pos+1);
		message = "";
		if(command == "ls"){
			argument = getCorrectedPath(currDirec, argument);
			if(argument=="Error"){
				message += "Error: Could not find directory\n";
			}
			else if(argument.length() < rootDir.length()){
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
		}
		else if(command == "cd"){
			argument = getCorrectedPath(currDirec, argument);
			if(argument=="Error"){
				message += "Error: Could not find directory\n";
			}
			else if(argument.length() < rootDir.length()){
				message += "Error: Unauthorised Access\n";
			}
			else{
				if(direcReadAllowed(currUser, currGroup, argument)){
					currDirec = argument;
				}
				else{
					message += "Error: No permissions to change to this directory\n";
				}
			}
		}
		else if(command == "fput"){
			string extractDirec = getFilePathDirectory(currDirec, argument);
			string extractName = getFilePathName(currDirec, argument);
			argument = extractDirec + "/" + extractName;
			if(extractDirec == "Error"){
				message += "Error: Incorrect path to file\n";
			}
			else if(extractDirec.length() < rootDir.length()){
				message += "Error: Unauthorised Access\n";
			}
			else if(filePathExists(argument)){
				if(fileWriteAllowed(currUser, argument)){
					message = "Input Stream. Q to Quit.\n";
					send(connFd, (void *)message.c_str(), 300, 0);
					bzero(test, 301);
					recv(connFd, test, 300, 0);
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
					message += "Enter File Owner: ";
					send(connFd, (void *)message.c_str(), 300, 0);
					bzero(test, 301);
					recv(connFd, test, 300, 0);
					response = test;
					if(authenticate_user(response)){
						metaData+=response + "\n";
					}

					message = "Enter File Group: ";
					send(connFd, (void *)message.c_str(), 300, 0);
					bzero(test, 301);
					recv(connFd, test, 300, 0);
					response = test;
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
				recv(connFd, test, 300, 0);
				response = test;
				appendToFile(argument,response);
				message = "";
			}
			else{
				message += "Error: No write permissions for the directory\n";
			}
		}
		else if(command == "fget"){
			string extractDirec = getFilePathDirectory(currDirec, argument);
			string extractName = getFilePathName(currDirec, argument);
			argument = extractDirec + "/" + extractName;
			if(extractDirec == "Error"){
				message += "Error: Incorrect path to file\n";
			}
			else if(extractDirec.length() < rootDir.length()){
				message += "Error: Unauthorised Access\n";
			}
			else if(filePathExists(argument)){
				if(fileReadAllowed(currUser,currGroup, argument)){
					message += getFileContents(argument);
				}
				else{
					message += "Error: No permissions to read this file\n";
				}
			}
			else{
				message += "Error: No such file exists\n";
			}
		}
		else if(command == "mkdir"){
			string extractDirec = getFilePathDirectory(currDirec, argument);
			string extractName = getFilePathName(currDirec, argument);
			argument = extractDirec + "/" + extractName;
			if(extractDirec=="Error"){
				message += "Error: Could not find directory\n";
			}
			else if(extractDirec.length() < rootDir.length()){
				message += "Error: Unauthorised Access\n";
			}
			else if(direcPathExists(argument)){
				message += "Error: Invalid directory name\n";
			}
			else if(direcWriteAllowed(currUser, extractDirec)){
				if(mkdir(argument.c_str(), 0777) == -1){
					message += "Error: Unable to make directory\n";
				}
				else{
					string metaData = "";
					message = "";
					while(1){
						message += "Enter Directory Owner: ";
						send(connFd, (void *)message.c_str(), 300, 0);
						bzero(test, 301);
						recv(connFd, test, 300, 0);
						response = test;
						if(authenticate_user(response)){
							metaData+=response + "\n";
						}
						message = "Enter Directory Group: ";
						send(connFd, (void *)message.c_str(), 300, 0);
						bzero(test, 301);
						recv(connFd, test, 300, 0);
						response = test;
						if(authenticate_group(response)){
							metaData+=response + "\n";
							break;
						}
						metaData = "";
						message = "Error: does not exist\n";
					}
					appendToFile(argument+".d", metaData);
					message = "";
				}
			}
			else{
				message += "Error: No permissions to create directory here\n";
			}
			
		}
		else{
			message += "No such command\n";
		}
		message += getDispPath(currDirec) + "$ ";
		send(connFd, (void *)message.c_str(), 300, 0);

	}

	cout << "Closing thread and conn" << endl;
	close(connFd);
}

int main(int argc, char* argv[]) {
	int pId, portNo, listenFd;
	socklen_t len;
	bool loop = false;
	struct sockaddr_in svrAdd, clntAdd;
	pthread_t threadA[3];
	int connFd;

	if (argc < 2){
		cerr << "Syntax : ./server <port>" << endl;
		return 0;
	}
	portNo = atoi(argv[1]);
	if((portNo > 65535) || (portNo < 2000)){
		cerr << "Please enter a port number between 2000-65535" << endl;
		return 0;
	}

	//create socket
	listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenFd < 0){
		cerr << "Cannot Open Socket" << endl;
		return 0;
	}

	bzero((char*) &svrAdd, sizeof(svrAdd));

	svrAdd.sin_family = AF_INET;
	svrAdd.sin_addr.s_addr = INADDR_ANY;
	svrAdd.sin_port = htons(portNo);

	//bind socket
	if(bind(listenFd, (struct sockaddr *)&svrAdd, sizeof(svrAdd)) < 0){
		cerr << "Cannot bind" << endl;
		return 0;
	}

	listen(listenFd, 5);
	len = sizeof(clntAdd);
	int noThread = 0;
	while (noThread < 3){
		cout << "Listening" << endl;

		connFd = accept(listenFd, (struct sockaddr *)&clntAdd, &len);
		if (connFd < 0){
			cerr << "Cannot Accept Connection" << endl;
			return 0;
		}
		else{
			cout << "Connection Successful" << endl;
		}
		pthread_create(&threadA[noThread], NULL, serverHandler, (void *) &connFd); 
		noThread++;
	}

	for(int i = 0; i < 3; i++){
		pthread_join(threadA[i], NULL);
	}
}
