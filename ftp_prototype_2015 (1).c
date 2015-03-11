//159.334 - Networks
//FTP server prototype for assignment 1, 159334
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
#include <windows.h>
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <winsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define WSVERS MAKEWORD(2,0)
WSADATA wsadata;
#endif

#define BUFFERSIZE 800
#if defined __unix__ || defined __APPLE__
#elif defined _WIN32 
#endif
//*******************************************************************
//MAIN
//*******************************************************************
int main(int argc, char *argv[]) {
//********************************************************************
// INITIALIZATION
//********************************************************************
   struct sockaddr_in localaddr,remoteaddr;
   #if defined __unix__ || defined __APPLE__
   int s,ns;
   #elif defined _WIN32 
   SOCKET s,ns;
   #endif
   char send_buffer[BUFFERSIZE],receive_buffer[BUFFERSIZE];
   //char remoteIP[INET_ADDRSTRLEN];
   int remotePort;
   int localPort;//no need for local IP...
   int n,bytes,addrlen;
   memset(&localaddr,0,sizeof(localaddr));//clean up the structure
   memset(&remoteaddr,0,sizeof(remoteaddr));//clean up the structure
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
//********************************************************************
//SOCKET
//********************************************************************

   s = socket(PF_INET, SOCK_STREAM, 0);
   if (s <0) {
      printf("socket failed\n");
   }
   localaddr.sin_family = AF_INET;
   if (argc == 2) localaddr.sin_port = htons((u_short)atoi(argv[1]));
   else localaddr.sin_port = htons(1221);//default listening port 
   localaddr.sin_addr.s_addr = INADDR_ANY;//server address should be local
//********************************************************************
//BIND
//********************************************************************
   if (bind(s,(struct sockaddr *)(&localaddr),sizeof(localaddr)) != 0) {
      printf("Bind failed!\n");
      exit(0);
   }
//********************************************************************
//LISTEN
//********************************************************************
   listen(s,5);
//********************************************************************
//INFINITE LOOP
//********************************************************************
   while (1) {
      addrlen = sizeof(remoteaddr);
      //********************************************************************
      //NEW SOCKET newsocket = accept
      //********************************************************************
#if defined __unix__ || defined __APPLE__
      ns = accept(s,(struct sockaddr *)(&remoteaddr),(socklen_t*)&addrlen);
#elif defined _WIN32
      ns = accept(s,(struct sockaddr *)(&remoteaddr),&addrlen);
#endif
      if (ns <0 ) break;
      printf("accepted a connection from client IP %s port %d \n",inet_ntoa(remoteaddr.sin_addr),ntohs(localaddr.sin_port));
      //********************************************************************
      //Respond with welcome message
      //*******************************************************************
      sprintf(send_buffer,"220 Welcome \r\n");
      bytes = send(ns, send_buffer, strlen(send_buffer), 0);
      while (1) {
         n = 0;
         while (1) {
//********************************************************************
//RECEIVE
//********************************************************************
            bytes = recv(ns, &receive_buffer[n], 1, 0);//receive byte by byte...
//********************************************************************
//PROCESS REQUEST
//********************************************************************
            if ( bytes <= 0 ) break;
            if (receive_buffer[n] == '\n') { /*end on a LF*/
               receive_buffer[n] = '\0';
               break;
            }
            if (receive_buffer[n] != '\r') n++; /*ignore CRs*/
         }
         if ( bytes <= 0 ) break;

      printf("-->DEBUG: the message from client reads: '%s' \r\n", receive_buffer);
	if (strncmp(receive_buffer,"USER",4)==0)  {
		printf("Logging in \n");
		sprintf(send_buffer,"331 Password required \r\n");
		bytes = send(ns, send_buffer, strlen(send_buffer), 0);
                if (bytes < 0) break;
	}
	if (strncmp(receive_buffer,"PASS",4)==0)  {
		printf("Typing password (anything will do... \n");
		sprintf(send_buffer,"230 Public login sucessful \r\n");
		bytes = send(ns, send_buffer, strlen(send_buffer), 0);
                if (bytes < 0) break;
	}
	if (strncmp(receive_buffer,"SYST",4)==0)  {
		printf("Information about the system \n");
		sprintf(send_buffer,"230 I don't know which OS this is: A very naive FTP system... \r\n");
		bytes = send(ns, send_buffer, strlen(send_buffer), 0);
                if (bytes < 0) break;
	}
        if (strncmp(receive_buffer,"QUIT",4)==0)  {
		printf("Quit \n");
		sprintf(send_buffer,"221 Connection close by client\r\n");
		bytes = send(ns, send_buffer, strlen(send_buffer), 0);
                if (bytes < 0) break;
#if defined __unix__ || defined __APPLE__
		close(ns);
#elif defined _WIN32 
		closesocket(ns);
#endif
	}

      }
//********************************************************************
//CLOSE SOCKET
//********************************************************************
#if defined __unix__ || defined __APPLE__
      close(ns);
#elif defined _WIN32 
      closesocket(ns);
#endif
      
      printf("disconnected from %s\n",inet_ntoa(remoteaddr.sin_addr));
   }
#if defined __unix__ || defined __APPLE__
   close(s);//it actually never gets to this point....use CTRL_C
#elif defined _WIN32 
   closesocket(s);//it actually never gets to this point....use CTRL_C
#endif
}
