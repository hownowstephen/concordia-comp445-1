// Basic FTP client
// @author Stephen Young
// @email st_youn@encs.concordia.ca
// @student_id 9736247

char* getmessage(char *);

#include <winsock.h>
#include <stdio.h>
#include <iostream>
using namespace std;

#include <fstream>
using namespace std;

#include <string.h>

#include <windows.h>
using namespace std;

#include "filetransfer.cpp"

//user defined port number
#define REQUEST_PORT 0x7070;

#define BUFFER_SIZE 128
#define GET "get"
#define PUT "put"
#define OK "OK"

int port=REQUEST_PORT;

//socket data types
SOCKET s;
SOCKADDR_IN sa;         // filled by bind
SOCKADDR_IN sa_in;      // fill with server info, IP, port

//buffer data types
char szbuffer[BUFFER_SIZE];

int ibufferlen=0;
int ibytessent;
int ibytesrecv=0;

//host data types
HOSTENT *hp;
HOSTENT *rp;

char localhost[11],
     remotehost[11];

// Filename to be transferred
char filename[11];
char direction[3];

int main(void){

	WSADATA wsadata;

	try {

		if (WSAStartup(0x0202,&wsadata)!=0){  
			cout<<"Error in starting WSAStartup()" << endl;
		} else {

			/* Display the wsadata structure */
			cout<< endl
				<< "wsadata.wVersion "       << wsadata.wVersion       << endl
				<< "wsadata.wHighVersion "   << wsadata.wHighVersion   << endl
				<< "wsadata.szDescription "  << wsadata.szDescription  << endl
				<< "wsadata.szSystemStatus " << wsadata.szSystemStatus << endl
				<< "wsadata.iMaxSockets "    << wsadata.iMaxSockets    << endl
				<< "wsadata.iMaxUdpDg "      << wsadata.iMaxUdpDg      << endl;
		}  


		//Display name of local host.

		gethostname(localhost,10);
		cout<<"Local host name is \"" << localhost << "\"" << endl;

		if((hp=gethostbyname(localhost)) == NULL) 
			throw "gethostbyname failed\n";

		//Ask for name of remote server
		cout << "please enter your remote server name: " << flush ;   
		cin >> remotehost ;

		if((rp=gethostbyname(remotehost)) == NULL)
			throw "remote gethostbyname failed\n";

		//Create the socket
		if((s = socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET) 
			throw "Socket failed\n";
		/* For UDP protocol replace SOCK_STREAM with SOCK_DGRAM */

		//Specify server address for client to connect to server.
		memset(&sa_in,0,sizeof(sa_in));
		memcpy(&sa_in.sin_addr,rp->h_addr,rp->h_length);
		sa_in.sin_family = rp->h_addrtype;   
		sa_in.sin_port = htons(port);

		//Display the host machine internet address

		cout << "Connecting to remote host:";
		cout << inet_ntoa(sa_in.sin_addr) << endl;

		//Connect Client to the server
		if (connect(s,(LPSOCKADDR)&sa_in,sizeof(sa_in)) == SOCKET_ERROR)
			throw "connect failed\n";

		// Ask for name of file
		cout << "please enter the filename: " << flush ;
		cin >> filename ;

		// Ask for direction
		cout << "Direction of transfer [get|put]: " << flush;
		cin >> direction;

		// Make sure the direction is one of get or put
		if(!strcmp(direction,GET) || !strcmp(direction,PUT)){ 
			
			char cusername[100];
			DWORD dwusername = sizeof(cusername);

			GetUserName(cusername,&dwusername);

			// Generate client headers
			sprintf(szbuffer,"%s %s %s", cusername, direction, filename); 

			ibytessent=0;    
			ibufferlen = strlen(szbuffer);
			
			// Send headers to the server
			if ((ibytessent = send(s,szbuffer,ibufferlen,0)) == SOCKET_ERROR)	
				throw "Send failed\n";  
			else	
				cout << "Message to server: " << szbuffer << endl;
			memset(szbuffer,0,BUFFER_SIZE); // zero the buffer

			// Perform a get request
			if(!strcmp(direction,GET)) get(s,cusername,direction,filename);
			// Perform a put request
			else if(!strcmp(direction,PUT))	put(s,cusername,direction,filename);

		}else{
			cout << "the request type was not recognized, this server only supports get or put";
		}

	} // try loop

	//Display any needed error response.
	catch (const char *str) { 
		cerr<<str<<endl;
	}

	//close the client socket
	closesocket(s);

	/* When done uninstall winsock.dll (WSACleanup()) and exit */ 
	WSACleanup();  
	return 0;
}





