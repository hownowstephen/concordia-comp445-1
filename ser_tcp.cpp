//    SERVER TCP PROGRAM
// revised and tidied up by
// J.W. Atwood
// 1999 June 30

#include <winsock.h>
#include <process.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "filetransfer.cpp"

#include <time.h>

//port data types
#define REQUEST_PORT 0x7070
#define BUFFER_SIZE 128
#define GET "get"
#define PUT "put"
#define OK "OK"
#define	STKSIZE	 16536

int port=REQUEST_PORT;

//socket data types
SOCKET s;
SOCKADDR_IN sa;      // filled by bind
SOCKADDR_IN sa1;     // fill with server info, IP, port

	

	//buffer data types

	char szbuffer[BUFFER_SIZE];

	char *buffer;
	int ibufferlen;
	int ibytesrecv;

	int ibytessent;

	//host data types

	char localhost[11];

	HOSTENT *hp;

	//wait variables

	int nsa1;

	int r,infds=1, outfds=0;

	struct timeval timeout;

	const struct timeval *tp=&timeout;

	fd_set readfds;

	//others

	HANDLE test;

	DWORD dwtest;

	void handle_client(){

		SOCKET s1;
		union {
			struct sockaddr generic;
			struct sockaddr_in ca_in;
		}ca;

		int calen=sizeof(ca); 

		char szbuffer[BUFFER_SIZE];

		try {

			//Found a connection request, try to accept. 
			if((s1=accept(s,&ca.generic,&calen))==INVALID_SOCKET)
				throw "Couldn't accept connection\n";

			//Connection request accepted.
			cout<<"accepted connection from "<<inet_ntoa(ca.ca_in.sin_addr)<<":"
				<<hex<<htons(ca.ca_in.sin_port)<<endl;

			memset(szbuffer,0,BUFFER_SIZE);

			//Fill in szbuffer from accepted request.
			if((ibytesrecv = recv(s1,szbuffer,BUFFER_SIZE,0)) == SOCKET_ERROR)
				throw "Error in client headers";

			cout << "Received headers from client" << szbuffer << endl;
			// Retrieve data about the user
			char client_name[11];
			char direction[3];
			char filename[100];

			sscanf(szbuffer,"%s %s %s",client_name,direction,filename);

			cout << "Client " << client_name << " requesting to " << direction << " file " << filename << endl;

			if(!strcmp(direction,GET)){
				put(s1,"server","put",filename);
			}else if(!strcmp(direction,PUT)){
				get(s1,"server","get",filename);
			}

		} catch(const char* str){
			cerr << str << endl;
		}

		//close Client socket
		closesocket(s1);	
	}

	int main(void){

		WSADATA wsadata;
		 
		try {
			if (WSAStartup(0x0202,&wsadata)!=0){  
				cout<<"Error in starting WSAStartup()\n";
			}else{
				buffer="WSAStartup was successful\n";   
				WriteFile(test,buffer,sizeof(buffer),&dwtest,NULL); 

				/* display the wsadata structure */
				cout<< endl
					<< "wsadata.wVersion "       << wsadata.wVersion       << endl
					<< "wsadata.wHighVersion "   << wsadata.wHighVersion   << endl
					<< "wsadata.szDescription "  << wsadata.szDescription  << endl
					<< "wsadata.szSystemStatus " << wsadata.szSystemStatus << endl
					<< "wsadata.iMaxSockets "    << wsadata.iMaxSockets    << endl
					<< "wsadata.iMaxUdpDg "      << wsadata.iMaxUdpDg      << endl;
			}  

			//Display info of local host

			gethostname(localhost,10);
			cout<<"hostname: "<<localhost<< endl;

			if((hp=gethostbyname(localhost)) == NULL) {
				cout << "gethostbyname() cannot get local host info?"
					<< WSAGetLastError() << endl; 
				exit(1);
			}

			//Create the server socket
			if((s = socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET) 
				throw "can't initialize socket";
			// For UDP protocol replace SOCK_STREAM with SOCK_DGRAM 


			//Fill-in Server Port and Address info.
			sa.sin_family = AF_INET;
			sa.sin_port = htons(port);
			sa.sin_addr.s_addr = htonl(INADDR_ANY);


			//Bind the server port

			if (bind(s,(LPSOCKADDR)&sa,sizeof(sa)) == SOCKET_ERROR)
				throw "can't bind the socket";
			cout << "Bind was successful" << endl;

			//Successfull bind, now listen for client requests.

			if(listen(s,10) == SOCKET_ERROR)
				throw "couldn't  set up listen on socket";
			else cout << "Listen was successful" << endl;

			FD_ZERO(&readfds);

			//wait loop

			int result;

			while(1)

			{
				Sleep(1);

				FD_SET(s,&readfds);  //always check the listener

				if(!(outfds=select(infds,&readfds,NULL,NULL,tp))) {}

				else if (outfds == SOCKET_ERROR) throw "failure in Select";

				else if (FD_ISSET(s,&readfds)){

					int args = 0;
				    if(( result = _beginthread((void (*)(void *))handle_client, STKSIZE, (void *) args))>=0)
					{
						cout << "New connection" << endl; 
					}
				}

			}//wait loop

		} catch(const char * str){
			cerr << str << endl;
		}


		//close server socket
		closesocket(s);

		/* When done uninstall winsock.dll (WSACleanup()) and exit */ 
		WSACleanup();
		return 0;
	}




