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

#include "PGSocket.h"





using namespace PGSocketLib;


PGSocket::PGSocket(){
	memset(this->host, 0, sizeof(this->host));
	memset(this->port, 0, sizeof(this->port));
	
	this->hints = NULL;
	this->resetHints();
	#ifdef WIN32
        int result = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (result != 0) {
            printf("WSAStartup failed: %d\n", result);
            exit(EXIT_FAILURE);
        }
    #endif
}

PGSocket::~PGSocket(){
	if(this->hints != NULL)
		delete this->hints;
	
	#ifdef WIN32
        WSACleanup();
	#endif
}

// Binding method
void PGSocket::bindSocket(){
    #ifdef WIN32
        socketDescriptor = INVALID_SOCKET;
    #endif
	struct addrinfo *result, *rp;
	int s;

	/* Obtain address(es) matching host/port */
	s = getaddrinfo(NULL, this->port, this->hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	/* getaddrinfo() returns a list of address structures.
	Try each address until we successfully connect(2).
	If socket(2) (or connect(2)) fails, we (close the socket
	and) try the next address. */
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		this->socketDescriptor = socket(rp->ai_family, rp->ai_socktype,
					rp->ai_protocol);

		if (socketDescriptor == -1)
			continue;

		if (bind(this->socketDescriptor, rp->ai_addr, rp->ai_addrlen) == 0){
			cout << "Binding socket at " << this->port << " port -> OK!" << endl;
			break;                  /* Success */
		}
        #ifdef WIN32
            closesocket(socketDescriptor);
            socketDescriptor = INVALID_SOCKET;
        #else
            close(socketDescriptor);
        #endif
	}

	if (rp == NULL) {               /* No address succeeded */
		fprintf(stderr, "Could not bind\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);           /* No longer needed */
}

// Connecting method
void PGSocket::connectSocket(){
    #ifdef WIN32
        socketDescriptor = INVALID_SOCKET;
    #endif
	struct addrinfo *result, *rp;
	int s;
	/* Obtain address(es) matching host/port */
	s = getaddrinfo(this->host, this->port, this->hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	/* getaddrinfo() returns a list of address structures.
	Try each address until we successfully connect(2).
	If socket(2) (or connect(2)) fails, we (close the socket
	and) try the next address. */
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		socketDescriptor = socket(rp->ai_family, rp->ai_socktype,
					rp->ai_protocol);

        #ifdef WIN32
		if (socketDescriptor == INVALID_SOCKET) {
            fprintf(stderr, "Error at socket(): %d\n", WSAGetLastError());
        #else
		if (socketDescriptor == -1){
		    fprintf(stderr, "Error at socket()");
        #endif
			continue;
		}
        #ifdef WIN32
        if (connect(socketDescriptor, rp->ai_addr, (int)rp->ai_addrlen) != SOCKET_ERROR){
        #else
		if (connect(socketDescriptor, rp->ai_addr, rp->ai_addrlen) != -1){
        #endif
			break;                  /* Success */
		}
		#ifdef WIN32
            closesocket(socketDescriptor);
            socketDescriptor = INVALID_SOCKET;
        #else
            close(socketDescriptor);
        #endif
	}
	if (rp == NULL) {               /* No address succeeded */
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);           /* No longer needed */
}

// Accept connection method
int PGSocket::acceptConnection(struct sockaddr_storage& addr, socklen_t& addr_len){
    #ifdef WIN32
    SOCKET newsocket = INVALID_SOCKET;
    #else
    int newsocket = -1;
    #endif

	newsocket = accept(this->socketDescriptor, (struct sockaddr *) &addr,
                        &addr_len);

    #ifdef WIN32
        if(newsocket == INVALID_SOCKET){
            closesocket(newsocket);
        }
    #else
        if(newsocket == -1){
            close(newsocket);
        }
    #endif
    return newsocket;
}


int PGSocket::listenSocket(int backlog){
	return listen(this->socketDescriptor, backlog);
}

// Close socket
void PGSocket::closeSocket(){
	#ifdef WIN32
        closesocket(socketDescriptor);
    #else
        close(socketDescriptor);
    #endif
}


/**
 Setter functions
 */
void PGSocket::resetHints(){
	if(this->hints != NULL)
		delete this->hints;
    this->hints = new struct addrinfo;

	memset(this->hints, 0, sizeof(struct addrinfo));

    this->hints->ai_flags = AI_PASSIVE;    // For wildcard IP address
    this->hints->ai_protocol = 0;          // Any protocol
    this->hints->ai_canonname = NULL;
    this->hints->ai_addr = NULL;
    this->hints->ai_next = NULL;
}
void PGSocket::setFamily(){
    // This method may seem useless, but it will be usefull
    // when IPv6 becomes a reality

    // Setting up the adress family
    // AF_INET -> IPv4
    // AF_INET6 -> IPv6
    // AF_UNSPEC -> allow IPv4 and IPv6
	this->hints->ai_family = AF_INET; // Allow IPv4
}

void PGSocket::setSockType(int type){
	if(type != PGS_TCP && type != PGS_UDP){
		cerr << "Error: PGSocket::setSockType(int type): incorrect type" << endl;
		return;
 	}
	if(type == PGS_TCP){
		this->hints->ai_socktype = SOCK_STREAM; // Stream (TCP) socket
		this->hints->ai_protocol = IPPROTO_TCP;
	}
	else{
		this->hints->ai_socktype = SOCK_DGRAM; // Datagram (UPD) socket
		this->hints->ai_protocol = IPPROTO_UDP;
	}
}

void PGSocket::setHost(string newHost){ strcpy(this->host, newHost.c_str()); }
void PGSocket::setPort(string newPort){
	strcpy(this->port, newPort.c_str());
}
void PGSocket::setPort(const char *newPort){
	strcpy(this->port, newPort);
}
void PGSocket::setSocketDescriptor(int newSD){
	this->socketDescriptor = newSD;
}


/**
 * Getter functions
 */
char* PGSocket::getHost(){ return this->host; }
char* PGSocket::getPort(){ return this->port; }
int PGSocket::getSocketDescriptor(){ return this->socketDescriptor; }



/**
 * Read and write functions
 */
int PGSocket::readSocket(char* buffer, int length){
    int n;
    #ifdef WIN32
        n = recv(socketDescriptor, buffer, length, 0);
    #else
        n = read(socketDescriptor, buffer, length);
    #endif
	return n;
}

int PGSocket::writeSocket(char* buffer, int length){
    int n;
    #ifdef WIN32
        n = send(socketDescriptor, buffer, length, 0);
    #else
        n = write(socketDescriptor, buffer, length);
    #endif
	return n;
}


/**
 * recvfrom and sendto functions
 */
int PGSocket::recvfromSocket(char* buf, int len, int flags,
							  struct sockaddr_storage &peer_addr,
							  socklen_t &peer_addr_len)
{
    int nread = recvfrom(this->socketDescriptor, buf, len, flags,
						(struct sockaddr *) &peer_addr, &peer_addr_len);
	if(nread < 0){
        fprintf(stderr, "Error receiving from socket. Buffer read: %s\n", buf);
        #ifdef WIN32
            printf("recv failed: %d\n", WSAGetLastError());
        #endif
        exit(1);
    }
    return nread;
}

int PGSocket::sendtoSocket(const char* buf, int len, int flags,
							  struct sockaddr_storage &peer_addr,
							  socklen_t &peer_addr_len)
{
	int n;
        n = sendto(socketDescriptor, buf, len, flags,
				   (struct sockaddr *) &peer_addr, peer_addr_len);

	if(n != len)
		return -1;
	else
		return n;
}




