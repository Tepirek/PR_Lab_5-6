#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <windows.h>
#include <time.h>
#include <WinUser.h>
#include <iostream>
#include <fstream>

using namespace std;

// link with "ws2_32.lib" library to prevent linking errors
#pragma comment(lib, "ws2_32.lib")

#define _CRT_SECURE_NO_WARNINGS

int main(void) {

	printf("SERVER APPLICATION\n");
	WSADATA wsas;
	int result;
	WORD wersja;
	wersja = MAKEWORD(1, 1);
	result = WSAStartup(wersja, &wsas);
	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in sa;
	memset((void*)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(6969);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	result = bind(s, (struct sockaddr FAR*) &sa, sizeof(sa));
	result = listen(s, 1);

	if (result == SOCKET_ERROR) {
		printf("Socket error!\n");
		return -1;
	}

	SOCKET si = -1;
	struct sockaddr_in sc;
	int lenc = sizeof(sc);
	si = accept(s, (struct sockaddr FAR*) & sc, &lenc);
	printf("Client connected to the server!\n");

	char* accountNumber = (char*)malloc(sizeof(char) * 27);

	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;
	LARGE_INTEGER freq;
	while (1) {
		fstream file;
		file.open("accountNumber.txt", ios::in);
		char ch;
		int cnt = 0;
		while (1) {
			if (file.eof()) {
				break;
			}
			file >> ch;
			accountNumber[cnt] = ch;
			cnt++;
		}
		accountNumber[cnt - 1] = '\0';
		file.close();
		printf("Sending: [%s]\n", accountNumber);
		Sleep(3000);
		QueryPerformanceFrequency(&Frequency);
		QueryPerformanceCounter(&StartingTime);
		send(si, accountNumber, 27, 0);
		QueryPerformanceCounter(&EndingTime);
		ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
		ElapsedMicroseconds.QuadPart *= 1000000;
		ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
		printf("Sending: [%s] time: %uns\n", accountNumber, ElapsedMicroseconds.u);
	}

	free(accountNumber);
	closesocket(si);
	closesocket(s);
	WSACleanup();

	return 0;
}
