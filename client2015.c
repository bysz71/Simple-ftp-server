//159.334 - Networks
// CLIENT updated 2013
//This prototype can be compiled with gcc (or g++) in both Linux and Windows
//To see the differences, just follow the ifdefs
#if defined __unix__ || defined __APPLE__
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#elif defined _WIN32 
# include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#define WSVERS MAKEWORD(2,0)
#endif


#define BUFFESIZE 200 
//remember that the BUFFESIZE has to be at least big enough to receive the answer from the server
#define SEGMENTSIZE 70
//segment size, i.e., if fgets gets more than this number of bytes it segments the message

#if defined __unix__ || defined __APPLE__

#elif defined _WIN32 
WSADATA wsadata;
#endif

int main(int argc, char *argv[]) {
//*******************************************************************
// Initialization
//*******************************************************************
   struct sockaddr_in remoteaddr;
   struct hostent *h;

#if defined __unix__ || defined __APPLE__
   int s;
#elif defined _WIN32 
   SOCKET s;
#endif
   char send_buffer[BUFFESIZE],receive_buffer[BUFFESIZE];
   int n,bytes;
   memset(&remoteaddr, 0, sizeof(remoteaddr)); //clean up 
//*******************************************************************
//WSASTARTUP 
//*******************************************************************
#if defined __unix__ || defined __APPLE__
//nothing to do in Linux
#elif defined _WIN32 
   if (WSAStartup(WSVERS, &wsadata) != 0) {
      WSACleanup();
      printf("WSAStartup failed\n");
   	exit(1);
   }
#endif
//*******************************************************************
//	Dealing with user's arguments
//*******************************************************************
   if (argc != 3) {
      printf("USAGE: client IP-address port\n");
      exit(1);
   }
   else {
	   remoteaddr.sin_addr.s_addr = inet_addr(argv[1]);//IP address
	   remoteaddr.sin_port = htons((u_short)atoi(argv[2]));//Port number
   }
//*******************************************************************
//CREATE CLIENT'S SOCKET 
//*******************************************************************
   s = socket(AF_INET, SOCK_STREAM, 0);
   if (s < 0) {
      printf("socket failed\n");
   	exit(1);
   }
   remoteaddr.sin_family = AF_INET;
//*******************************************************************
//CONNECT
//*******************************************************************
   if (connect(s, (struct sockaddr *)&remoteaddr, sizeof(remoteaddr)) != 0) {
      printf("connect failed\n");
   	exit(1);
   }
//*******************************************************************
//Get input while user don't type "."
//*******************************************************************
   memset(&send_buffer, 0, BUFFESIZE);
   fgets(send_buffer,SEGMENTSIZE,stdin);
   while (strncmp(send_buffer,".",1) != 0) {
#if defined __unix__ || defined __APPLE__
   send_buffer[strlen(send_buffer)-1]='\0';//strip send_buffer from '\n'
   printf("lenght is %d \n",(int)strlen(send_buffer));
#elif defined _WIN32 
  //nothing to do
#endif
      printf(">>> %s\n",send_buffer);//line sent
      strcat(send_buffer,"\r\n");
//*******************************************************************
//SEND
//*******************************************************************
      bytes = send(s, send_buffer, strlen(send_buffer),0);
      if (bytes < 0) {
         printf("send failed\n");
      	exit(1);
      }
      n = 0;
      while (1) {
//*******************************************************************
//RECEIVE
//*******************************************************************
         bytes = recv(s, &receive_buffer[n], 1, 0);
         if ((bytes <= 0)) {
            printf("recv failed\n");
         	exit(1);
         }
         if (receive_buffer[n] == '\n') {  /*end on a LF*/
            receive_buffer[n] = '\0';
            break;
         }
         if (receive_buffer[n] != '\r') n++;   /*ignore CR's*/
      }
      printf("%s \n",receive_buffer);// line received
      memset(&send_buffer, 0, BUFFESIZE);
      fgets(send_buffer,SEGMENTSIZE,stdin);
   }
//*******************************************************************
//CLOSESOCKET   
//*******************************************************************
#if defined __unix__ || defined __APPLE__
   close(s);
#elif defined _WIN32 
   closesocket(s);
#endif
   
   return 0;
}
