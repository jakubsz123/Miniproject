#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <thread>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define BACKLOG 10


int main()
{

	WSADATA data;
	WORD version = MAKEWORD(2, 2);

	// Start WinSock
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
	{
		std::cout << "Can't start Winsock! " << wsOk;
		return 0;
	}

	SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);

	sockaddr_in serverHint;
	serverHint.sin_addr.S_un.S_addr = ADDR_ANY; // any IP address
	serverHint.sin_family = AF_INET;
	serverHint.sin_port = htons(54000); // little to big endian

	//bind socket 
	if (bind(in, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
	{
		std::cout << "Can't bind socket! " << WSAGetLastError() << std::endl;
		return 0;
	}

	sockaddr_in client;
	int clientLength = sizeof(client);

	char buf[1024];

	bool gameOver = false;
	while (!gameOver)
	{
		std::cout << "It's opponent's turn, please wait..." << std::endl;
		ZeroMemory(&client, clientLength); // clear the client structure
		ZeroMemory(buf, 1024); // clear the receive buffer

		// wait for message
		int bytesIn = recvfrom(in, buf, 1024, 0, (sockaddr*)&client, &clientLength);
		if (bytesIn == SOCKET_ERROR)
		{
			std::cout << "Error receiving from client " << WSAGetLastError() << std::endl;
			continue;
		}

		std::string selection = "";
		while (selection != "paper" && selection != "rock" && selection != "scissors" && selection != "quit") {
			std::cout << "Choose: 'rock' / 'paper' / 'scissors' (or 'quit' to quit)." << std::endl;
			std::cin >> selection;
		}
		std::string s(selection);

		int sendOK = sendto(in, s.c_str(), s.size() + 1, 0, (sockaddr*)&client, clientLength);

		if (sendOK == SOCKET_ERROR) {
			std::cout << "That didn't work" << WSAGetLastError();
		}

	}

	//clean the socket and terminate the program
	closesocket(in);
	WSACleanup();
	return 0;
}