// TransPic_Server.cpp : 定义控制台应用程序的入口点。
/*
	Server.cpp
*/
//

#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define BUF 256
#pragma comment(lib, "Ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData = { 0 };
	int iResult = 0;
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		wprintf(L"WSAStartup failed: %d\n", iResult);
		return 1;
	}

	struct sockaddr_in servaddr;
	int bResult = 0;
	SOCKET sock = INVALID_SOCKET;
	char buf[BUF];

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sock == INVALID_SOCKET) {
		wprintf(L"socket failed with error: %ld\n", GetLastError());
		WSACleanup();
		return 1;
	}

	servaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8888);

	bResult = bind(sock, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));
	if (bResult == INVALID_SOCKET) {
		wprintf(L"bind failed with error: %ld\n", GetLastError());
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	if (listen(sock, 256) == SOCKET_ERROR)
	{
		wprintf(L"listen failed with error: %ld\n", GetLastError());
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	SOCKET acceptsock = INVALID_SOCKET;

	printf("Accepting ...\n");
	if ((acceptsock = accept(sock, NULL, NULL)) == INVALID_SOCKET)
	{
		wprintf(L"accept failed with error: %ld\n", GetLastError());
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	else
		printf("Client connected.\n");

	FILE *fp = NULL;
	// 不能以二进制打开txt耶
	if ((fp = fopen("popkart.jpg", "rb")) == NULL)
	{
		wprintf(L"fopen failed with error: %ld\n", GetLastError());
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	size_t size = 0;
	while (!feof(fp))
	{
		size = fread(buf, sizeof(char), sizeof(buf), fp);
		if (send(acceptsock, buf, size, 0) == SOCKET_ERROR)
		{
			wprintf(L"send failed with error: %ld\n", GetLastError());
			closesocket(sock);
			closesocket(acceptsock);
			WSACleanup();
			return 1;
		}
	}
	printf("send picture finish.\n");
	
	// 将缓冲区的内容全发送出去，调用closesock之前不可少
	shutdown(acceptsock, SD_SEND);
	closesocket(acceptsock);
	closesocket(sock);
	WSACleanup();

	system("pause");
	return 0;
}

