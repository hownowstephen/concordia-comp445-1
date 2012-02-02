#include <winsock.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
#include <fstream>
#include <string.h>
#include <windows.h>

//user defined port number
#define REQUEST_PORT 0x7070;
#define BUFFER_SIZE 128
#define GET "get"
#define PUT "put"
#define OK "OK"
#define MISSING "NO"


void get(SOCKET s, char * username, char * direction, char * filename){

	char szbuffer[BUFFER_SIZE];
	memset(szbuffer,0,BUFFER_SIZE);

	char *buffer;
	int ibufferlen=0;
	int ibytessent;
	int ibytesrecv=0;

	//host data types
	HOSTENT *hp;
	HOSTENT *rp;

 	// Perform a get request
 	// Parse response and filesize from server
	char response[2];
	int filesize;
	ofstream output_file;

	try {
		//wait for reception of server response.
		ibytesrecv=0; 
		if((ibytesrecv = recv(s,szbuffer,BUFFER_SIZE,0)) == SOCKET_ERROR)
			throw "Receive failed\n";
		
		cout << "Server responded with " << szbuffer << endl;

		sscanf(szbuffer,"%s %d",response,&filesize);
		cout << "Response " << response << " filesize " << filesize << endl;

		// Ensure the response from the server is OK
		if(!strcmp(response,OK)){

			// Open our local file for writing
			output_file.open(filename);

			// Send ack to signal to the server to send data
			memset(szbuffer,0,BUFFER_SIZE);
			sprintf(szbuffer,"SEND");
			ibufferlen = strlen(szbuffer);

			if ((ibytessent = send(s,szbuffer,ibufferlen,0)) == SOCKET_ERROR)
				throw "Send failed\n";  

			// Intermediary buffer for formatting incoming data
			char outdata[BUFFER_SIZE];
			int count = 0;

			// Read data from the server until we have received the file
			while(count < filesize){
				if((ibytesrecv = recv(s,szbuffer,BUFFER_SIZE-1,0)) == SOCKET_ERROR)
					throw "Receive failed\n";

				sprintf(outdata,"%s",szbuffer);
				output_file.write(outdata,strlen(outdata));

				count += strlen(outdata);

				cout << "Received " << count << " bytes" << endl;
				// Sanitize buffer
				memset(szbuffer,0,BUFFER_SIZE);
				memset(outdata,0,BUFFER_SIZE);
			}

			// Close our output file
			output_file.close();

			// Clear the buffer and send an ack to the server to confirm receipt
			memset(szbuffer,0,BUFFER_SIZE);
			sprintf(szbuffer,"OK");
			ibufferlen = strlen(szbuffer);

			if ((ibytessent = send(s,szbuffer,ibufferlen,0)) == SOCKET_ERROR)
				throw "Send failed\n";  
		}else{
			cout << "Requested file does not exist" << endl;
		}
	} catch(const char* str){
		cerr<<str<<WSAGetLastError()<<endl;
	}
 }


void put(SOCKET s, char * username, char * direction, char * filename){

	char szbuffer[BUFFER_SIZE];
	memset(szbuffer,0,BUFFER_SIZE);

	char *buffer;
	int ibufferlen=0;
	int ibytessent;
	int ibytesrecv=0;
	//host data types
	HOSTENT *hp;
	HOSTENT *rp;

	cout << "Sending file " << filename << endl;

	ifstream filedata;
	filebuf *pbuf;
	int filesize;

	try {

		// Open the file
		filedata.open(filename);

		if(filedata.is_open()){

			// Get pointer to file buffer and determine file size
			pbuf = filedata.rdbuf();
			filesize = pbuf->pubseekoff(0,ios::end,ios::in);
			pbuf->pubseekpos(0,ios::in);

			cout << "File size: " << filesize << endl;

			// Send back an OK message to the client to confirm receipt
			memset(szbuffer,0,BUFFER_SIZE);
			sprintf(szbuffer,"OK %d",filesize);
			ibufferlen = strlen(szbuffer);

			if((ibytessent = send(s,szbuffer,ibufferlen,0))==SOCKET_ERROR)
				throw "error in send in server program\n";

			// Wait for confirmation from the client  
			memset(szbuffer,0,BUFFER_SIZE);
			if((ibytesrecv = recv(s,szbuffer,BUFFER_SIZE,0)) == SOCKET_ERROR)
				throw "Receive error in server program\n";

			int count = 0;
			// Loop through the file and stream to the client
			while(!filedata.eof()){
				filedata.read(szbuffer,BUFFER_SIZE-1);
				ibufferlen = strlen(szbuffer);
				count += ibufferlen;
				cout << "Sent " << count << " bytes" << endl;
				if((ibytessent = send(s,szbuffer,ibufferlen,0))==SOCKET_ERROR)
					throw "error in send in server program\n";

				memset(szbuffer,0,BUFFER_SIZE);
			}

			filedata.close();

			if((ibytesrecv = recv(s,szbuffer,BUFFER_SIZE,0)) == SOCKET_ERROR)
				throw "Receive error in server program\n";

			if(!strcmp(szbuffer,OK)){
				cout << "File transfer completed" << endl;
			}

		}else{

			cout << "File does not exist, sending decline" << endl;
			// Send back a NO to the client to indicate that the file does not exist
			memset(szbuffer,0,BUFFER_SIZE);
			sprintf(szbuffer,"NO -1");
			ibufferlen = strlen(szbuffer);

			if((ibytessent = send(s,szbuffer,ibufferlen,0))==SOCKET_ERROR)
				throw "error in send in server program\n";
		}
	} catch(const char* str){
		cerr<<str<<WSAGetLastError()<<endl;
	}
	memset(szbuffer,0,BUFFER_SIZE);
 }