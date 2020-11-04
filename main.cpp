#define WIN32_LEAN_AND_MEAN

#include"../../std_lib_facilities.h"
#include<Windows.h>
#include<WinSock2.h>
#include<WS2tcpip.h>


//link with ws2_32. lib
#pragma comment (lib,"ws2_32.lib")



//default størrelsen  på bafferen 
//og default port 
#define default_buff  5024


const int port = 8080;// the port where the client can reach

int main() {
	//creating the windSock
	WSADATA wsaData;
	int wsOk;
	wsOk = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//sjekker if winsock har errors
	if (wsOk != 0) {
		cerr << "WSAStartup has errors: " << wsOk;
		return 1;
	}

	//creating the Server Socket
	SOCKET ServerSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ServerSock == INVALID_SOCKET) {
		cerr << "The Server sock could not create! Exit# " << endl;
		return 1;
	}

	//identifying the ip adress and port to the socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;


	//binding the sock
	bind(ServerSock, (sockaddr*)&hint, sizeof(hint));

	//winsock listening
	listen(ServerSock, SOMAXCONN);


	//connection from the client
	sockaddr_in client;
	int clientSize = sizeof(client);

	//cleint sock
	SOCKET ClientSock = accept(ServerSock, (sockaddr*)&client, &clientSize);

	char clientHost[NI_MAXHOST];//client's name
	char Service[NI_MAXSERV];//service where the client is connected on

	ZeroMemory(clientHost, NI_MAXHOST);
	ZeroMemory(Service, NI_MAXSERV);

	//get the info if the client has connected to the port
	if (getnameinfo((sockaddr*)&client, sizeof(client), clientHost, NI_MAXHOST, Service, NI_MAXSERV, 0) == 0) {
		cout << "#Connected on Port " << Service << endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, clientHost, NI_MAXHOST);
		cout << clientHost << " Connected on port " << ntohs(client.sin_port) << endl;
	}

	//closing the server sock
	closesocket(ServerSock);


	char accBuff[default_buff];

	while (1) {
		ZeroMemory(accBuff, default_buff);

		//let the client send data

		int byteReceived = recv(ClientSock, accBuff, default_buff, 0);
		if (byteReceived == SOCKET_ERROR) {
			cerr << "error in received: " << endl;
			break;
		}
		else if (byteReceived == 0) {
			cout << "Client has disconnected!" << endl;
			break;
		}

		cout << string(accBuff, 0, byteReceived) << endl;

		//send back message to client
		send(ClientSock, accBuff, byteReceived + 1, 0);
	}

	//close everything
	closesocket(ClientSock);
	WSACleanup();
}