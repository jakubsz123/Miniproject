#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "4445"

#define DEFAULT_ADDRESS "localhost"

using namespace std;

int main(int argc, char* argv[]) // makes it able to input command lines
{
	// start up the winsocket
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	int wsOK = WSAStartup(version, &data);
	if (wsOK != 0) {
		cout << "Cant start socket" << wsOK;
		return;
	}

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(4500);
	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

	int serverLength = sizeof(server);

	// Socket creation
	SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);

	//string s(argv[1]);
	char buf[DEFAULT_BUFLEN];

	bool running = true;
	while (running == true) {
		// Write out that socket
		string s = "";
		cin.get >> s;
		int sendOK = sendto(out, s.c_str(), s.size, 0, (sockaddr*)&server, serverLength);

		if (sendOK == SOCKET_ERROR) {
			cout << "That didn't work" << WSAGetLastError();
		}

		ZeroMemory(buf, 1024);
		int bytesIn = recvfrom(out, buf, 1024, 0, (sockaddr*)&server, &serverLength);
		if (bytesIn == SOCKET_ERROR)
		{
			std::cout << "Error receiving from server " << WSAGetLastError() << std::endl;
			continue;
		}

		if (s == "quit") {
			running = false;
		}
	}

	closesocket(out);

	// Closing down the winsocket
	WSACleanup();

	// Wait for the player to enter one last thing into the console before exiting
	std::cin.get();
	return 0;
}
