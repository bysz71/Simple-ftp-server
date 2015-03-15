//159.334 assignment1

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <dirent.h>
#include <sys/types.h>
#define WSVERS MAKEWORD(2,0) 
//for wsastartup() use only, make 2 byte -> 16bit word
WSADATA wsadata;


#define BUFFERSIZE 200
#define SEGMENTSIZE 198

static const char USER[] = "admin";
static const char PW[] = "admin";
void str_cut(char* output_string, const char* input_string, int start, int end);
int recv_msg(char* receive_buffer,int ns);
char rfc[20][4];




main(int argc, char *argv[]){
	strcpy(rfc[0],"SYST");
	strcpy(rfc[1],"USER");
	strcpy(rfc[2],"PASS");
	strcpy(rfc[3],"QUIT");
	
	const char username[] = "admin";
	const char password[] = "admin";
	const int cport = 1234;
	struct sockaddr_in localaddr,remoteaddr,data_addr_act;
	SOCKET s, ns, s_data_act;
	
	int act_ip[4];
	unsigned char act_port[2];
	int port_dec, connect_result;
	char addr_data[20];
	
	char send_buffer[BUFFERSIZE],receive_buffer[BUFFERSIZE];
	char temp[BUFFERSIZE];
	char bool;
	memset(&send_buffer,0,BUFFERSIZE);
	memset(&receive_buffer,0,BUFFERSIZE);	//init buffers
	
	int n,bytes,addrlen;
	memset(&localaddr,0,sizeof(localaddr));
	memset(&remoteaddr,0,sizeof(remoteaddr));	//init local and remote addresses
	
	DIR *dp;
	struct dirent *ep;


//winsock init	
	printf("\nInitialising Winsock ...\n");
	if(WSAStartup(WSVERS, &wsadata) != 0){
		WSACleanup();
		printf("WSAStartup failed. Error Code : %d", WSAGetLastError());
		exit(1);
	}
	printf("Initialised.\n");

//SOCKET
	s = socket(PF_INET, SOCK_STREAM, 0);	//init s socket(A/P family[PF_INET for tcp] , transport[SOCK_STREAM for tcp] , protocol specified [0 for no specification])
	if (s < 0) {
		printf("socket failed\n");
		exit(1);
	}
	localaddr.sin_family = AF_INET;	//address family set for tcp
	if (argc == 2) {	//decision on using specified port or default port
		localaddr.sin_port = htons((u_short)atoi(argv[1]));
		printf("Communication port is specified to %s. \n",argv[1]);
	}else{
		localaddr.sin_port = htons(cport);
		printf("No port specified, use default port %d.\n",cport);
	}
	localaddr.sin_addr.s_addr = INADDR_ANY;	//s_addr, 32 bit unsigned long IP addr, INADDR_ANY -> 0,0,0,0 local
	
//BIND: bind socket with local address
	if (bind(s,(struct sockaddr *)(&localaddr),sizeof(localaddr))!=0){
		printf("Bind failed!\n");
		exit(0);
	}
	printf("binding succeeded.\n");

//LISTEN
	listen(s,5);
	printf("Listening...\n");

//infinite loop
	while(1){
		addrlen = sizeof(remoteaddr);
		ns = accept(s,(struct sockaddr *)(&remoteaddr),&addrlen);
		if (ns < 0){
			sprintf(send_buffer,"421 Service not available, closing control connection\n");
			break;
		}
		printf("accepted a connection from client IP %s port %d \n",inet_ntoa(remoteaddr.sin_addr),ntohs(localaddr.sin_port));
		//inet_ntoa converts Ipv4 address into ASCII string in internet standard dotted-decimal format
		//ntohs converts a u_short from TCP/IP network byte order to host byte order (format conversion blah blah)

//system info feed if 421 not happen
		sprintf(send_buffer,"220 Microsoft FTP service\n");
		bytes = send(ns,send_buffer,strlen(send_buffer),0);
		if(bytes<0)break;

//log in session
		while(1){
			bytes = recv_msg(receive_buffer,ns);
			if(bytes < 0)break;
			
			memset(&send_buffer,0,BUFFERSIZE);
			if(strncmp(receive_buffer,"USER",4) == 0){
				sprintf(send_buffer,"331 password required\n");
				bytes = send(ns,send_buffer,strlen(send_buffer),0);
				continue;
			}
			if(strncmp(receive_buffer,"PASS",4) == 0){
				sprintf(send_buffer,"230 logged in\n");
				bytes = send(ns,send_buffer,strlen(send_buffer),0);
				break;
			}
		}

//port session
		bytes = recv_msg(receive_buffer,ns);
		if(bytes < 0) break;
		if(strncmp(receive_buffer,"PORT",4) == 0){
			//str_cut(temp,receive_buffer,5,strlen(receive_buffer));
			//printf(temp);
			//sscanf(temp, "%d,%d,%d,%d,%d,%d",&act_ip[0],&act_ip[1],&act_ip[2],&act_ip[3],&act_port[0],&act_port[1]);
			//sprintf(addr_data,"%d.%d.%d.%d",act_ip[0],act_ip[1],act_ip[2],act_ip[3]);
			//printf("%s",addr_data);
			sscanf(receive_buffer, "PORT %d,%d,%d,%d,%d,%d",&act_ip[0],&act_ip[1],&act_ip[2],&act_ip[3],&act_port[0],&act_port[1]);
			port_dec = (act_port[0] << 8) + act_port[1];
			data_addr_act.sin_port = htons(port_dec);			
			data_addr_act.sin_addr.s_addr = inet_addr("192.168.1.5");
			data_addr_act.sin_family = AF_INET;
			s_data_act = socket(AF_INET, SOCK_STREAM, 0);
			if(s_data_act == INVALID_SOCKET){
				printf("socket function failed with error:%d \n",WSAGetLastError());
				WSACleanup();
				return 1;
			}
			connect_result = connect(s_data_act, (struct sockaddr *) & data_addr_act, sizeof(data_addr_act));
			if(connect_result == SOCKET_ERROR){
				printf("connect function failed with error:%d \n",WSAGetLastError());
				connect_result == closesocket(s_data_act);
				if(connect_result == SOCKET_ERROR)
					printf("close socket function failed with error:%d \n",WSAGetLastError());
				WSACleanup();
				return 1;
			}
			printf("data connection established\n");
			sprintf(send_buffer,"200 PORT command successful\n");
			bytes = send(ns,send_buffer,strlen(send_buffer),0);
			if(bytes < 0) break;
		}
		
//command session
//windows ftp client seems do not support passive mode Orz		
		while(1){
			bytes = recv_msg(receive_buffer,ns);
			if(bytes < 0) break;
			if(strncmp(receive_buffer,"LIST",4) == 0){
				sprintf(send_buffer,"150 opening data connection for file list...\n");
				bytes = send(ns,send_buffer,strlen(send_buffer),0);
				if(bytes < 0) break;
				dp = opendir("./");
				if(dp != NULL){
					while(ep = readdir(dp)){
						puts(ep->d_name);
						//sprintf(send_buffer,ep->d_name);
						//strcat(send_buffer,"\n");
						//bytes = send(s_data_act, send_buffer,strlen(send_buffer),0);
						//if(bytes < 0) break;
					}
					printf("aaa\n");
					(void) closedir (dp);
				}else
					perror("couldn't open the directory");
				printf("bbb\n");
				sprintf(send_buffer,"226 closing data connection. Requested file action successful.\n");
				bytes = send(ns,send_buffer,strlen(send_buffer),0);
				if(bytes < 0) break;
			}else{
				sprintf(send_buffer,"150 hahahah");
				bytes = send(ns,send_buffer,strlen(send_buffer),0);
				if(bytes < 0) break;
			}
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

int recv_msg(char* receive_buffer,int ns){
	int n = 0;
	int bytes = 0;
	while(1){
		bytes = recv(ns,&receive_buffer[n],1,0);
		if(bytes<=0) break;
		if(receive_buffer[n] == '\n'){
			receive_buffer[n] = '\0';
			break;
		}
		if(receive_buffer[n] != '\r') n++;
	}
	//if(bytes <= 0) break;
	printf("-->Debug: message reads from client <%s>\n", receive_buffer);
	return bytes;
}