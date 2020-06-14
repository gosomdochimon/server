#include "Accepter.h"
#include "Headers.h"

time_t t = time(NULL);
struct tm tm_c = *localtime(&t);
class Accepter acpt;

Accepter::Accepter()
{
	bAccept = true;
}

Accepter::~Accepter()
{
	WSACleanup();
	closesocket(Client_Sock);
}


//DWORD WINAPI ThreadProc(LPVOID lpvoid)
//{
//	return ((acpt)lpvoid)->ThreadProc();
//}


//socket error print & exit 
void Accepter::err_quit(const char* msg)
{
	
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

//Socket var error print
void Accepter::err_display(const char* msg)
{
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// user default data recv var
int Accepter::recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

bool Accepter::Initialize()
{

	int retval;
	// ���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &dwsa) != 0)
		return 1;

	// socket()
	Listen_Sock = socket(AF_INET, SOCK_STREAM, 0);
	if (Listen_Sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(Listen_Sock, (SOCKADDR*)& serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(Listen_Sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");
	//printf("�ʴ�");
	return true;
}

 DWORD WINAPI ProcessClient(LPVOID lpParameter)
{

	time_t t = time(NULL);
	struct tm tm_c = *localtime(&t);

	SOCKET client_sock = (SOCKET)lpParameter;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)& clientaddr, &addrlen);

	while (1) {
		// ������ �ޱ�
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			acpt.err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// ���� ������ ���
		buf[retval] = '\0';
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), buf);
		printf("�����ð�: %d-%d-%d %d:%d:%d\n",
			tm_c.tm_year + 1900, tm_c.tm_mon + 1, tm_c.tm_mday,
			tm_c.tm_hour, tm_c.tm_min, tm_c.tm_sec);
		// ������ ������
		retval = send(client_sock, buf, retval, 0);
		if (retval == SOCKET_ERROR) {
			acpt.err_display("send()");
			printf("�����ð�: %d-%d-%d %d:%d:%d\n",
				tm_c.tm_year + 1900, tm_c.tm_mon + 1, tm_c.tm_mday,
				tm_c.tm_hour, tm_c.tm_min, tm_c.tm_sec);
			break;
		}
	}

	// closesocket()
	closesocket(client_sock);
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

void Accepter::func()
{
	int retval;
	//SOCKET clientSocket;
	//SOCKADDR_IN clientAddr;
	//printf("�ʴ�");
	while (bAccept) {
		int addrlen;
		// accept()
		addrlen = sizeof(clientaddr);
		Client_Sock = accept(Listen_Sock, (SOCKADDR*)& clientaddr, &addrlen);
		if (Client_Sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		printf("���ӽð�: %d-%d-%d %d:%d:%d\n",
			tm_c.tm_year + 1900, tm_c.tm_mon + 1, tm_c.tm_mday,
			tm_c.tm_hour, tm_c.tm_min, tm_c.tm_sec);

		// ������ ����
		for (int i = 0; i < MAX; i++) {
			hThread[i] = CreateThread(NULL, 0, ProcessClient,
				(LPVOID)Client_Sock, 0, NULL);
			if (hThread[i] == NULL) { closesocket(Client_Sock); }
			else { CloseHandle(hThread[i]); }


		}
	}

}





// 

