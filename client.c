#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <stdint.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define MASTER_SERVER_PORT 5001
#define LISTENTO 3
#define MAX_PEERS 30


int main() {


	int iResult;
	WSADATA wsaData;

	SOCKET connectSocket = INVALID_SOCKET;
	struct sockaddr_in masterServer, server, client;//sockaddr_in structure to define port, IP address, address family
	int recvbuflen = DEFAULT_BUFLEN;
	char *sendbuf = "Hello server";
	char recvbuf[DEFAULT_BUFLEN] = "";
	int i;
	char str[INET_ADDRSTRLEN];


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup failed with error: %d\n", iResult);
		return 1;
	}


	// Create a SOCKET for connecting to master server
	connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connectSocket == INVALID_SOCKET) {
		wprintf(L"connect socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	

	masterServer.sin_family = AF_INET;
	//clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
	masterServer.sin_port = htons(MASTER_SERVER_PORT);

	//use inet_pton, inet_addr deprecated
	if (inet_pton(AF_INET, "127.0.0.1", &masterServer.sin_addr) <= 0)
	{
		printf("\n inet_pton error occured\n");
		return 1;
	}

	//connect to master server
	iResult = connect(connectSocket, (SOCKADDR*)&masterServer, sizeof(masterServer));
	if (iResult == SOCKET_ERROR) {
		wprintf(L"connect failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}


	// Send an initial buffer to master server
	iResult = send(connectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"send failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %d\n", iResult);

	/*// shutdown the connection since no more data will be sent
	iResult = shutdown(connectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
	wprintf(L"shutdown failed with error: %d\n", WSAGetLastError());
	closesocket(connectSocket);
	WSACleanup();
	return 1;
	}*/

	// Receive until connection is closed
	do {

		iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {

			recvbuf[iResult] = '\0';//null character for use with printf
			wprintf(L"Bytes received: %d\n", iResult);
			printf("Message from server ip %s, port %d - %s \n", 
				inet_ntop(AF_INET, &(masterServer.sin_addr), str, INET_ADDRSTRLEN)
				, ntohs(masterServer.sin_port), recvbuf);
		}
		else if (iResult == 0)
			wprintf(L"Connection closed\n");
		else
			wprintf(L"recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);

	// close the socket
	iResult = closesocket(connectSocket);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"close failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	WSACleanup();
	return 0;
}//end main
