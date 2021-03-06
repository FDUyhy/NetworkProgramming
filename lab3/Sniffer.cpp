// Sniffer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "initsock.h"
#include "protoinfo.h"

#include <stdio.h>
#include <mstcpip.h>

CInitSock sock;

void DecodeTCPPacket(char *pData)
{
	TCPHeader *pTCPHdr = (TCPHeader *)pData;

	printf("源端口: %d -> 目标端口：%d \n", ntohs(pTCPHdr->sourcePort), ntohs(pTCPHdr->destinationPort));

	// 下面还可以根据目的端口号进一步解析应用层协议
	switch (::ntohs(pTCPHdr->destinationPort))
	{
	case 21:
		printf("应用层协议为：FTP协议!\n");
		break;
	case 80:
		printf("应用层协议为：HTTP协议!\n");
		break;
	case 8080:
		printf("应用层协议为：HTTP协议!\n");
		break;
	}
}

void DecodeUDPPacket(char *pData)
{
	UDPHeader *pUDPHdr = (UDPHeader *)pData;

	printf("源端口：%d -> 目标端口：%d \n", ntohs(pUDPHdr->sourcePort), ntohs(pUDPHdr->destinationPort));

}

void DecodeIPPacket(char *pData)
{
	IPHeader *pIPHdr = (IPHeader*)pData;
	in_addr source, dest;
	char szSourceIp[32], szDestIp[32];

	printf("\n\n-------------------------------\n");

	// 从IP头中取出源IP地址和目的IP地址
	source.S_un.S_addr = pIPHdr->ipSource;
	dest.S_un.S_addr = pIPHdr->ipDestination;
	strcpy(szSourceIp, ::inet_ntoa(source));
	strcpy(szDestIp, ::inet_ntoa(dest));
	
	printf("源地址IP：%s -> 目标地址IP：%s \n", szSourceIp, szDestIp);
	// IP头长度
	int nHeaderLen = (pIPHdr->iphVerLen & 0xf) * sizeof(ULONG);

	switch (pIPHdr->ipProtocol)
	{
	case IPPROTO_TCP: // TCP协议
		printf("网络层协议为：IP协议\n");
		DecodeTCPPacket(pData + nHeaderLen);
		break;
	case IPPROTO_UDP:
		printf("网络层协议为：UDP协议\n");
		DecodeUDPPacket(pData + nHeaderLen);
		break;
	case IPPROTO_ICMP:
		printf("网络层协议为：ICMP协议\n");
		getchar();
		break;
	}
}

int main()
{
	SOCKET sRaw = socket(AF_INET, SOCK_RAW, IPPROTO_IP);

	char szHostName[56];
	SOCKADDR_IN addr_in;
	struct hostent *pHost;
	gethostname(szHostName, 56);
	if ((pHost = gethostbyname((char *)szHostName)) == NULL)
		return -1;

	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(0);
	memcpy(&addr_in.sin_addr.S_un.S_addr, pHost->h_addr_list[0], pHost->h_length);
	printf("Bind to interface:%s\n", ::inet_ntoa(addr_in.sin_addr));
	if (bind(sRaw, (PSOCKADDR)&addr_in, sizeof(addr_in)) == SOCKET_ERROR)
	{
		printf("Fail to bind!\n");
		return -1;
	}

	DWORD dwValue = 1;
	if (ioctlsocket(sRaw, SIO_RCVALL, &dwValue) != 0)
	{
		printf("Fail to set SIO_RCVALL!\n");
		return -1;
	}

	char buff[1024];
	int nRet;
	while (TRUE)
	{
		nRet = recv(sRaw, buff, 1024, 0);
		if (nRet > 0)
		{
			DecodeIPPacket(buff);
			printf("收到的字节数为：%d", nRet);
		}
		else
			printf("Fail to recv data!\n");
	}

	closesocket(sRaw);

	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
