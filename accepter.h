#include "Headers.h"
#pragma once

#define SERVERPORT 9000
#define BUFSIZE    50

DWORD WINAPI ProcessClient(LPVOID lpParameter);

class Accepter
{
public:
	Accepter();
	~Accepter();
	WSADATA dwsa;
	SOCKADDR_IN serveraddr, clientaddr;
	SOCKET Listen_Sock, Client_Sock;
	LPVOID lpMsgBuf;
	HANDLE hThread[MAX];
	int bAccept;
public:
	void err_quit(const char* msg);
	void err_display(const char* msg);
	int recvn(SOCKET s, char* buf, int len, int flags);
	bool Initialize();
	void func();
	


};

