#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#define WSVERS MAKEWORD(2,0)
#define BUFFERSIZE 200
#define SEGMENTSIZE 70

WSADATA wsadata;

int main(int argc, char *argv[]){
	struct sockaddr_in remoteaddr;
	struct hostent *h;
	
	SOCKET s;
	
	char send_buffer[BUFFERSIZE],receive_buffer[BUFFERSIZE];
	int n,bytes;
	memset(&remoteaddr, 0, sizeof(remoteaddr));
	//clean up remoteaddr
	
	if(WSAStartup(WSVERS, &wsadata) != 0){
		WSACleanup();
		printf("WSAStartup failed\n");
		exit(1);
	}else{
		remoteaddr.sin_addr.s_addr = inet_addr(argv[1]);
		remoteaddr.sin_port = htons((u_short)atoi(argv[2]));
	}
	
	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s<0){
		printf("socket failed\n");
		exit(1);
	}
	remoteaddr.sin_family = AF_INET;
	
	if(connect(s,(struct sockaddr *)&remoteaddr,sizeof(remoteaddr))!=0){
		printf("connect failed\n");
		exit(1);
	}
	
	memset(&send_buffer, 0, BUFFERSIZE);
	fgets(send_buffer,SEGMENTSIZE,stdin);
	int cnt = 0;
	while(strncmp(send_buffer,".",1)!=0){
		send_buffer[strlen(send_buffer)-1] = '\0';
		printf(">>>%s\n",send_buffer);
		strcat(send_buffer,"\r\n");
		bytes = send(s, send_buffer, strlen(send_buffer),0);
		if(bytes<0){
			printf("send failed\n");
			exit(1);
		}
		n=0;
		while(1){
			bytes = recv(s,&receive_buffer[n],1,0);
			if(bytes <= 0){
				printf("recv failed\n");
				exit(1);
			}
			if(receive_buffer[n] =='\n'){
				receive_buffer[n] = '\0';
				break;
			}
			if(receive_buffer[n] != '\r')n++;
		}
		n++;
		printf("%d th recv\n");
		printf("%s \n",receive_buffer);
		memset(&send_buffer,0,BUFFERSIZE);
		fgets(send_buffer,SEGMENTSIZE,stdin);;
	}
	printf("error\n");
	closesocket(s);
	return 0;
	
}