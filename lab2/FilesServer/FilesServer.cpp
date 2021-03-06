// FilesServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#include "methods.h"

int main()
{
	// 创建监听套节字，绑定到本地端口，进入监听模式
	int nPort = 8888;
	SOCKET sListen =
		::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN si;
	si.sin_family = AF_INET;
	si.sin_port = ::ntohs(nPort);
	si.sin_addr.S_un.S_addr = INADDR_ANY;
	::bind(sListen, (sockaddr*)&si, sizeof(si));
	::listen(sListen, 200);

	// 为监听套节字创建一个SOCKET_OBJ对象
	PSOCKET_OBJ pListen = GetSocketObj(sListen);

	// 加载扩展函数AcceptEx
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	DWORD dwBytes;
	WSAIoctl(pListen->s,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx,
		sizeof(GuidAcceptEx),
		&pListen->lpfnAcceptEx,
		sizeof(pListen->lpfnAcceptEx),
		&dwBytes,
		NULL,
		NULL);

	::InitializeCriticalSection(&g_cs);
	for (int i = 0; i < 64; i++) {
		PBUFFER_OBJ pBuffer = GetBufferObj(pListen, BUFFER_SIZE);
		PostAccept(pBuffer);
		AssignToFreeThread(pBuffer);
		g_pThreadList->events[0] = ::WSACreateEvent();
		::WSASetEvent(g_pThreadList->events[0]);
	}
	while (true)
	{

	}
	return 0;
}

