// FilesClient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#include"initsock.h"
#include<stdio.h>
#include <fstream>
#include<time.h>
#include<string>
#include<map>
using namespace std;

CInitSock initSock;		// 初始化Winsock库
map<int, string> seq2str;

int main()
{
	char sendbuf[1024];   //发送内容
	char recvbuf[1024];   //接收内容
	ofstream outfile;
	char servIP[20] = "";

	printf("请输入服务器IP地址：\n");
	scanf("%s", servIP);

	SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		printf("创建套接字失败\n");
		return 0;
	}

	//服务器地址设置
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(8888);
	servAddr.sin_addr.S_un.S_addr = inet_addr(servIP);
	if (::connect(s, (sockaddr*)&servAddr, sizeof(servAddr)) == -1)
	{
		printf("连接服务器失败！\n");
		return 0;
	}
	time_t temp = time(NULL);
	printf("连接服务器成功，系统时间：%s", ctime(&temp));

	int type = 1;
	while (1)
	{
		printf("要传消息（1）还是请求文件（2）？(default:1)\n");
		int user_type;
		scanf("%d", &user_type);
		type = user_type;
		switch (type)
		{
			case 1:
			{
				printf("请输入消息：\n");
				memset(sendbuf, 0, sizeof(sendbuf));
				memset(sendbuf, 'M', 1);
				scanf("%s", sendbuf + 1);
				if (strcmp(sendbuf, "Mquit") == 0) {
					closesocket(s);
					return 1;
				}

				int ret = ::send(s, sendbuf, strlen(sendbuf), 0);
				if (ret < 0)
					printf("发送失败！\n");

				while (1)
				{
					memset(recvbuf, 0, sizeof(recvbuf));
					int nRecv = ::recv(s, recvbuf, 1024, 0);
					if (nRecv > 0)
					{
						recvbuf[nRecv] = '\0';
					}
					if (strcmp(recvbuf, "OK") == 0) {
						time_t temp = time(NULL);
						printf("服务器已经收到消息,系统时间：%s\n", ctime(&temp));
						break;
					}
					else if (strcmp(recvbuf, "NO") == 0) {
						time_t temp = time(NULL);
						printf("服务器没有收到消息,系统时间：%s\n", ctime(&temp));
						break;
					}
				}
				type = 0;
			}
			break;
			case 2:
			{
				printf("请输入文件名称：\n");
				memset(sendbuf, 0, sizeof(sendbuf));
				memset(sendbuf, 'F', 1);
				scanf("%s", sendbuf + 1);
				if (strcmp(sendbuf, "Fquit") == 0) {
					closesocket(s);
					return 1;
				}

				int ret = ::send(s, sendbuf, strlen(sendbuf), 0);
				if (ret < 0)
					printf("发送失败！\n");

				while (1)
				{
					memset(recvbuf, 0, sizeof(recvbuf));
					int nRecv = ::recv(s, recvbuf, 1024, 0);
					if (nRecv > 0)
					{
						recvbuf[nRecv] = '\0';
					}
					if (strcmp(recvbuf, "ERROR") == 0)
					{
						printf("服务器上不存在文件：%s,请重新输入\n", sendbuf + 1);
						break;
					}
					if (strcmp(recvbuf, "FOUND") == 0) {
						time_t temp = time(NULL);
						printf("文件：%s开始传输,系统时间：%s", sendbuf + 1, ctime(&temp));
					}
					//接收并写入文件
					char path[512];
					const char *tmproute = "";
					strcpy_s(path, tmproute);
					for (int i = 0; i < strlen(sendbuf); i++) {
						path[i + strlen(tmproute)] = sendbuf[i + 1];
					}
					path[strlen(sendbuf) + 32] = '\0';
					outfile.open(path, ios::out | ios::trunc | ios::binary);
					int wantedseq = 1;
					while (1)
					{
						memset(recvbuf, 0, sizeof(recvbuf));
						::recv(s, recvbuf, 1024, 0);
						unsigned short len; int seq;
						memcpy(&len, &recvbuf[1], 2);
						memcpy(&seq, &recvbuf[3], 4);
						len -= 7;
						if (seq == wantedseq) {
							outfile.write(&recvbuf[7], len);
							wantedseq++;
							while (seq2str.find(wantedseq) != seq2str.end()) {
								outfile << seq2str[wantedseq];
								seq2str.erase(wantedseq);
								wantedseq++;
							}
						}
						else {
							string ts(recvbuf + 7, recvbuf + len + 7);
							seq2str[seq] = ts;
						}
						if (recvbuf[0] == 0)
						{
							outfile.close();
							break;
						}
					}
					time_t temp = time(NULL);
					printf("文件%s传输完毕,系统时间:%s", sendbuf + 1, ctime(&temp));
					printf("文件%s已经保存至与该程序同一路径下\n", sendbuf + 1);
					break;
				}
				type = 0;
			}
			break;
			default:
			{
				//printf("输入有误！！，请重新输入\n");
			}
			break;
		}
	}
	::closesocket(s);
	return 1;
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
