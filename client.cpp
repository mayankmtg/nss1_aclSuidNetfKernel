#include <string.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>

using namespace std;


void send_string(string send_string, int size,int listenFd){
	char char_array[size];
	bzero(char_array, size+1);
	strcpy(char_array, send_string.c_str());
	write(listenFd,char_array,size);
}

string recv_string(int size, int listenFd){
	char char_array[size];
	bzero(char_array, size+1);
	read(listenFd, char_array, size);
	string curr_string(char_array);
	return curr_string;
}


int main (int argc, char* argv[]){
	int listenFd, portNo;
	bool loop = false;
	struct sockaddr_in svrAdd;
	struct hostent *server;
	if(argc < 3){
		cerr<<"Syntax : ./client <host name> <port>"<<endl;
		return 0;
	}
	portNo = atoi(argv[2]);
	if((portNo > 65535) || (portNo < 2000)){
		cerr<<"Please enter port number between 2000 - 65535"<<endl;
		return 0;
	}
	//create client skt
	listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenFd < 0){
		cerr << "Cannot open socket" << endl;
		return 0;
	}

	server = gethostbyname(argv[1]);
	if(server == NULL){
		cerr << "Host does not exist" << endl;
		return 0;
	}

	bzero((char *) &svrAdd, sizeof(svrAdd));
	svrAdd.sin_family = AF_INET;

	bcopy((char *)server->h_addr,(char *)&svrAdd.sin_addr.s_addr,server->h_length);

	svrAdd.sin_port = htons(portNo);
	int checker = connect(listenFd,(struct sockaddr *) &svrAdd, sizeof(svrAdd));
	if (checker < 0){
		cerr << "Cannot Connect!" << endl;
		return 0;
	}
	// else connected
	char test[300];
	cout << "Ctrl + C to exit"<< endl;
	// collect 'response' from server
	string response;
	bzero(test, 301);
	recv(listenFd, test, 300, 0);
	response = test;
	cout << response;

	//send 'message' to server
	string message;
	while(1){
		if(response == "Input Stream. Q to Quit.\n"){
			string line;
			message="";
			while (getline(cin, line)){
				if (line == "Q"){
					break;
				}
				message += line + "\n";
			}
		}
		else{
			cin.getline(test, 300);
			message = test;
		}
		send(listenFd, (void *)message.c_str(), 300, 0);
		// write(listenFd, test, strlen(test));
		// message=test;
		bzero(test, 301);
		ssize_t recvBytes = recv(listenFd, test, 300, 0);
		if(recvBytes==(ssize_t)0){
			break;
		}
		
		response = test;
		cout << response;

	}
	close(checker);
	return 0;
}