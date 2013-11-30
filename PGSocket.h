/************************************************************************
* PGSocketLib v1.0														*
* author: Pau Guillamon													*
* date: July 2012														*
* 																		*
* Based in Oscar Campos posts in GenbetaDev:							*
* http://www.genbetadev.com/tag/introduccion-al-multiprocesamiento-en-c *
*                                                               		*
*More info at: http://obj.sytes.net										* 
*                                                               		*
************************************************************************/

#ifndef _PGSocket_h_
#define _PGSocket_h_

#include <iostream>
#include <string.h>
using namespace std;


#ifdef WIN32
	#define _WIN32_WINNT 0x501
    #include <ws2tcpip.h>
    #include <stdio.h>
    #include <winsock2.h>
    #include <windows.h>
	#pragma comment(lib, "Ws2_32.lib")
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netdb.h>
	#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>


namespace PGSocketLib
{
	enum{
		PGS_TCP, PGS_UDP
	};
	
	/**
	 * PGSocket class
	 */
	class PGSocket{
		public:
			// ctor
			PGSocket();
			// dtor
			~PGSocket();

			// Binding method
			void bindSocket();

			// Connecting method
			void connectSocket();
			// Accept connection method
			int acceptConnection(struct sockaddr_storage&, socklen_t&);
			// Listen for connections petitions
			int listenSocket(int);

			// closing socket method
			void closeSocket();

			// setter functions
			void resetHints();
			void setFamily();
			void setSockType(int type);
			void setHost(string);
			void setPort(string);
			void setPort(const char *);
			void setSocketDescriptor(int);

			// getter functions
			char* getHost();
			char* getPort();
			int getSocketDescriptor();

			// read and write functions
			int readSocket(char *, int);
			int writeSocket(char *, int);
			// recvfrom and sendto functions
			int recvfromSocket(char*, int, int, 
							   struct sockaddr_storage&, socklen_t&);
			int sendtoSocket(const char*, int, int, 
							   struct sockaddr_storage&, socklen_t&);

		protected:
		private:
            #ifdef WIN32
                WSADATA wsaData;
                SOCKET socketDescriptor;
            #else
                int socketDescriptor;
            #endif
			//char host[25];
			char host[NI_MAXHOST];
			char port[6];
			struct addrinfo* hints;
	};
}


#endif // _PGSockets_h_
