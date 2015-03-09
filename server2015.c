//159.334 - Networks
// SERVER updated 2013
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
WSADATA wsadata;
#endif

#define BUFFESIZE 200
//remember that the BUFFESIZE has to be at least big enough to receive the answer
#define SEGMENTSIZE 198
//segment size, i.e., BUFFESIZE - 2 bytes (for \r\n)

//*******************************************************************
//MAIN
//*******************************************************************
main(int argc, char *argv[]) {

//********************************************************************
// INITIALIZATION
//********************************************************************
   struct sockaddr_in localaddr,remoteaddr;

#if defined __unix__ || defined __APPLE__
   int s, ns;
#elif defined _WIN32
   SOCKET s, ns;
#endif
   char send_buffer[BUFFESIZE],receive_buffer[BUFFESIZE];
   memset(&send_buffer,0,BUFFESIZE);
   memset(&receive_buffer,0,BUFFESIZE);

   int n,bytes,addrlen;
   memset(&localaddr,0,sizeof(localaddr));//clean up the structure
   memset(&remoteaddr,0,sizeof(remoteaddr));//clean up the structure

#if defined __unix__ || defined __APPLE__
   //nothing to do here
#elif defined _WIN32
//********************************************************************
// WSSTARTUP
//********************************************************************
   if (WSAStartup(WSVERS, &wsadata) != 0) {
      WSACleanup();
      printf("WSAStartup failed\n");
   }
#endif
//********************************************************************
//SOCKET
//********************************************************************

   s = socket(PF_INET, SOCK_STREAM, 0);
   if (s < 0) {
      printf("socket failed\n");
   }

   localaddr.sin_family = AF_INET;
   if (argc == 2) localaddr.sin_port = htons((u_short)atoi(argv[1]));
   else localaddr.sin_port = htons(1234);//default listening port
   localaddr.sin_addr.s_addr = INADDR_ANY;//server IP address should be local
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
      if (ns < 0) break;
      printf("accepted a connection from client IP %s port %d \n",inet_ntoa(remoteaddr.sin_addr),ntohs(localaddr.sin_port));
      while (1) {
         n = 0;
         while (1) {
//********************************************************************
//RECEIVE
//********************************************************************
            bytes = recv(ns, &receive_buffer[n], 1, 0);
//********************************************************************
//PROCESS REQUEST
//********************************************************************
            if (bytes <=0) break;
            if (receive_buffer[n] == '\n') { /*end on a LF*/
               receive_buffer[n] = '\0';
               break;
            }
            if (receive_buffer[n] != '\r') n++; /*ignore CRs*/
         }
         if (bytes <= 0) break;
	 printf("The client is sending: %s\n",receive_buffer);
	 memset(&send_buffer, 0, BUFFESIZE);
	 sprintf(send_buffer, "<<< SERVER SAYS:The client typed '%s' - There are %d bytes of information\r\n", receive_buffer, n);

//********************************************************************
//SEND
//********************************************************************
         bytes = send(ns, send_buffer, strlen(send_buffer), 0);
         if (bytes < 0) break;
      }
//********************************************************************
//CLOSE SOCKET
//********************************************************************
#if defined __unix__ || defined __APPLE__
      close(ns);//close connecting socket
#elif defined _WIN32
      closesocket(ns);//close connecting socket
#endif
      printf("disconnected from %s\n",inet_ntoa(remoteaddr.sin_addr));
   }
#if defined __unix__ || defined __APPLE__
   close(s);//close listening socket
#elif defined _WIN32
   closesocket(s);//close listening socket
#endif
   return 0;
}

