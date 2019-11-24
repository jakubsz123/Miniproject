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

//function that chooses the winner based on the server's and client's choice; 1: win, 0: draw, -1: lose
int decideWinner(std::string me, const char* opponent) {
	if (me == "rock") {
		if (opponent[0] == 'r') {
			return 0;
		}
		else if (opponent[0] == 'p') {
			return -1;
		}
		else {
			return 1;
		}
	}
	else if (me == "paper") {
		if (opponent[0] == 'p') {
			return 0;
		}
		else if (opponent[0] == 's') {
			return -1;
		}
		else {
			return 1;
		}
	}
	else {
		if (opponent[0] == 's') {
			return 0;
		}
		else if (opponent[0] == 'r') {
			return -1;
		}
		else {
			return 1;
		}
	}
}

int main(int argc, char* argv[]) // makes it able to input command lines
{
	// start up the winsocket
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	int wsOK = WSAStartup(version, &data);
	// Check if the startup was sucessful otherwise print an error message
	if (wsOK != 0) {
		std::cout << "Cant start socket" << wsOK;
		return 0;
	}

	// Getting the client to know about the server
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(4500);
	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

	int serverLength = sizeof(server);

	// Socket creation
	SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);

	//string s(argv[1]);
	char buf[DEFAULT_BUFLEN];

	bool gameOver = false;

	while (!gameOver) {
		// Write out that socket

		std::string selection = "";

		while (selection != "paper" && selection != "rock" && selection != "scissors" && selection != "quit") {
			std::cout << "Choose: 'rock' / 'paper' / 'scissors' (or 'quit' to quit)." << std::endl;
			std::cin >> selection;
		}

		std::string s(selection);

		// Store the code for if the message was sent or not, but also send the message to the server using the port out
		int sendOK = sendto(out, s.c_str(), s.size() + 1, 0, (sockaddr*)&server, serverLength);

		// Checks if sending the message was successful else print error message
		if (sendOK == SOCKET_ERROR) {
			std::cout << "That didn't work" << WSAGetLastError();
		}

		std::cout << "It's opponent's turn, please wait..." << std::endl;

		// Clear out the buffer to make it ready to receive a message from the server
		ZeroMemory(buf, DEFAULT_BUFLEN);
		// Store the code for if a message was successfully received, and stores the message in the buffer
		int bytesIn = recvfrom(out, buf, DEFAULT_BUFLEN, 0, (sockaddr*)&server, &serverLength);
		if (bytesIn == SOCKET_ERROR)
		{
			std::cout << "Error receiving from server " << WSAGetLastError() << std::endl;
			continue;
		}

		if (buf[0] == 'q' || selection == "quit") {
			gameOver = true;
		}
		else {
			int result = decideWinner(selection, buf);
			if (result == -1) {
				std::cout << "The opponent chose " << buf << ". That beats " << selection << ", so you lose." << std::endl;
			}
			else if (result == 1) {
				std::cout << "The opponent chose " << buf << ", but that was wrong, you win. Well done. Good job. You're amazing." << std::endl;
			}
			else {
				std::cout << "It's a draw, because you chose the same thing as the opponent. You draw." << std::endl;
			}
		}
	}

	closesocket(out);

	// Closing down the winsocket
	WSACleanup();

	// Wait for the player to enter one last thing into the console before exiting
	std::cin.get();
	return 0;
}
