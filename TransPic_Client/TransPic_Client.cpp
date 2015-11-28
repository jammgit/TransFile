// TransPic_Client.cpp : 定义控制台应用程序的入口点。
/*
	Client.cpp
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

	//----------------------
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup function failed with error: %d\n", iResult);
		return 1;
	}
	//----------------------
	// Create a SOCKET for connecting to server
	SOCKET ConnectSocket;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		wprintf(L"socket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
	clientService.sin_port = htons(8888);

	//----------------------
	// Connect to server.
	iResult = connect(ConnectSocket, (SOCKADDR *)& clientService, sizeof(clientService));
	if (iResult == SOCKET_ERROR) {
		wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
		iResult = closesocket(ConnectSocket);
		if (iResult == SOCKET_ERROR)
			wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	wprintf(L"Connected to server.\n");

	//------------------------
	// Start to recv picture data
	FILE *fp = NULL;
	if ((fp = fopen("popkart.jpg", "wb+")) == NULL)
	{
		closesocket(ConnectSocket);
		wprintf(L"fopen function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	size_t size = 0;
	char buf[BUF];
	do
	{
		size = recv(ConnectSocket, buf, sizeof(buf), MSG_WAITALL);
		// 对端调用了closesock,则返回SOCKET_ERROR
		if (size == SOCKET_ERROR)
		{
			break;
		}
		fwrite(buf, size, 1, fp);
		// 这里，循环条件是读取的字节数是否等于BUF，可能出现文件大小刚好是BUF的整数倍，故shutdown来辅助判断
	} while (size == sizeof(buf));

	fclose(fp);

	printf("read finish.\n");
	// End recv
	//------------------------
	

	iResult = closesocket(ConnectSocket);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	WSACleanup();
	system("pause");
	return 0;
}

