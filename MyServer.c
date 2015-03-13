//159.334 assignment1

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#define WSVERS MAKEWORD(2,0) 
//for wsastartup() use only, make 2 byte -> 16bit word
WSADATA wsadata;


#define BUFFERSIZE 200
#define SEGMENTSIZE 198

static const char USER[] = "admin";
static const char PW[] = "admin";
void str_cut(char* output_string, const char* input_string, int start, int end);

char rfc[20][4];




main(int argc, char *argv[]){
	strcpy(rfc[0],"SYST");
	strcpy(rfc[1],"USER");
	strcpy(rfc[2],"PASS");
	struct sockaddr_in localaddr,remoteaddr;
	//declare 2 sockaddr_in struct
	SOCKET s, ns;
	//declare 2 socket
	
	char send_buffer[BUFFERSIZE],receive_buffer[BUFFERSIZE];
	char temp[BUFFERSIZE];
	char bool;
	memset(&send_buffer,0,BUFFERSIZE);
	memset(&receive_buffer,0,BUFFERSIZE);
	//initialize send_buffer and receive_buffer
	
	int n,bytes,addrlen;
	memset(&localaddr,0,sizeof(localaddr));
	memset(&remoteaddr,0,sizeof(remoteaddr));
	//clean up localaddr and remoteaddr
	
	printf("\nInitialising Winsock ...\n");
	if(WSAStartup(WSVERS, &wsadata) != 0){
		WSACleanup();
		printf("WSAStartup failed. Error Code : %d", WSAGetLastError());
		exit(1);
	}
	printf("Initialised.\n");

//SOCKET
	s = socket(PF_INET, SOCK_STREAM, 0);
	//initialize socket s
	//A/P family, PF_INET for TCP
	//transport, SOCK_STREAM for TCP
	//no protocol specified, 0
	if (s < 0) {
		printf("socket failed\n");
	}
	localaddr.sin_family = AF_INET;
	//address family set for TCP
	if (argc == 2) {
		localaddr.sin_port = htons((u_short)atoi(argv[1]));
		printf("Communication port is specified to %s. \n",argv[1]);
	//argv[1] specifies port, convert from string to int, cast to unsigned shot, convert from host to network short
	}else{
		localaddr.sin_port = htons(1234);
		printf("No port specified, use default port 1234.\n");
	}
	//default port
	localaddr.sin_addr.s_addr = INADDR_ANY;
	//s_addr is 32 bit unsigned long, IP address
	//INADDR_ANY sets sin_addr to 0,0,0,0 local
	
//BIND
	if (bind(s,(struct sockaddr *)(&localaddr),sizeof(localaddr))!=0){	//jiu shi zhe yi hang la
		printf("Bind failed!\n");
		exit(0);
	}
	printf("binding succeeded.\n");
	//bind port with localaddr so client can access

//LISTEN
	listen(s,5);
	printf("Listening...\n");

//infinite loop
	while(1){
		addrlen = sizeof(remoteaddr);
		ns = accept(s,(struct sockaddr *)(&remoteaddr),&addrlen);
		if (ns < 0) break;
		printf("accepted a connection from client IP %s port %d \n",inet_ntoa(remoteaddr.sin_addr),ntohs(localaddr.sin_port));
		//inet_ntoa converts Ipv4 address into ASCII string in internet standard dotted-decimal format
		//ntohs converts a u_short from TCP/IP network byte order to host byte order (format conversion blah blah)

//system info feed 
		sprintf(send_buffer,"220 System ready\n");
		bytes = send(ns,send_buffer,strlen(send_buffer),0);
		if(bytes<0)break;
		
		while(1){
			n = 0;
			while(1){
				bytes = recv(ns,&receive_buffer[n],1,0);
				//recv returns the number of bytes received
				//socket, buffer pointer, length in bytes, flag(dont know)
				if(bytes<=0) break;
				if(receive_buffer[n] == '\n'){
					receive_buffer[n] = '\0';
					//if current byte is line feed, change it to enter
					break;
				}
				if(receive_buffer[n] != '\r'){
					//if carriage return, ignore
					n++;
				}
			}
			if(bytes <= 0) break;

			printf("-->Debug: message reads from client <%s>\n", receive_buffer);
			
			int cnt=0;
			while(cnt<BUFFERSIZE){
				if(strncmp(receive_buffer,rfc[cnt],4)==0)break;
				cnt++;
			}
			str_cut(temp,receive_buffer,5,strlen(receive_buffer));
			memset(&send_buffer,0,BUFFERSIZE);
			switch(cnt){
				case 0:
					sprintf(send_buffer,"211 system info\n");
					break;
				case 1:
					if(strncmp(temp,"admin",strlen(temp))==0){
						sprintf(send_buffer,"331 user ok, wait for password\n");
					}else{
						sprintf(send_buffer,"332 user not found\n");
					}
					break;
				case 2:
					if(strncmp(temp,"admin",strlen(temp))==0){
						sprintf(send_buffer,"230 logged in\n");
					}else{
						sprintf(send_buffer,"332 type your username again\n");
					}
				default:
					sprintf(send_buffer,"332 user required");
			}

			
//send message to client
			bytes = send(ns,send_buffer,strlen(send_buffer),0);
			//send send_buffer
			if (bytes < 0) break;
			//??? in what conditions send() returns minus value?
			
			
		}
		closesocket(ns);
		printf("disconnected from %s \n", inet_ntoa(remoteaddr.sin_addr));
		printf("exit?(y/n)");
		bool = getchar();
		if(bool=='y')exit(1);
		
	}
	closesocket(s);
	return 0;
}

void str_cut(char* output_string, const char* input_string, int start, int end){
	int i = 0;
	int j = 0;
	for(j = start; j <= end; j++){
		output_string[i] = input_string[j];
		i++;
	}
}