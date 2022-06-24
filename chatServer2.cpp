#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdio.h"
#include "winsock2.h"
#include "stdlib.h"
#include "windows.h"
#include "direct.h"
#include "io.h"
#include <time.h>
#include <string>
#include <iostream>
#include "client.h"
#pragma comment(lib,"ws2_32.lib")

using namespace std;

void communicat(int idx);
SOCKET cSocket[1024];
SOCKET s1Socket;
int Count = 0;
char cIPAddr[1024][15];//存储了各个客户端的ip地址
void listen_to_service();
client Client[1024];

int main(void){
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(LOBYTE(wsaData.wVersion)!=2||HIBYTE(wsaData.wVersion)!=2){
		printf("确定协议版本失败：%d\n",GetLastError());
		return -1;
	}
	printf("确定协议版本成功!\n");

	SOCKET sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//创建套接字
	if (SOCKET_ERROR == sSocket)
	{
		printf("创建套接字失败：%d\n",GetLastError());
		WSACleanup();//清理协议信息
		return -1;
	}

	SOCKADDR_IN sAddr={0};//本地服务器协议地址族
	sAddr.sin_family = AF_INET;
	sAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	sAddr.sin_port = htons(9527);//小端转大端

	int r=bind(sSocket,(sockaddr*)&sAddr,sizeof sAddr);//绑定
	if(r == SOCKET_ERROR){
		printf("绑定失败：%d\n",GetLastError());
		closesocket(sSocket);
		WSACleanup();//清理协议信息
		return -1;
	}
	printf("绑定成功!\n");

	/*此处为连接第一个服务器的尝试*/
	/*SOCKADDR_IN s2Addr={0};//本地服务器协议地址族
	s2Addr.sin_family = AF_INET;
	s2Addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	s2Addr.sin_port = htons(9527);//小端转大端*/
	s1Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//创建连接第一个服务器的套接字
	/*r = bind(s1Socket,(sockaddr*)&s2Addr,sizeof s2Addr);//绑定本地地址族
	if(r == SOCKET_ERROR){
		printf("绑定失败：%d\n",GetLastError());
		closesocket(sSocket);
		WSACleanup();//清理协议信息
		system("pause");
		return -1;
	}
	printf("绑定成功!\n");*/
	SOCKADDR_IN s1Addr={0};//获取第一个服务器协议地址族
	s1Addr.sin_family = AF_INET;
	s1Addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//第一个服务器的ip
	s1Addr.sin_port = htons(8888);//小端转大端，第一个服务器的端口

    r = connect(s1Socket, (sockaddr*)&s1Addr, sizeof s1Addr);//连接服务器
	//system("pause");
	if (SOCKET_ERROR == r)
	{
		printf("连接失败:%s\n",GetLastError());
		closesocket(s1Socket);
		WSACleanup();
		return -1;
	}
	printf("连接服务器成功! ip地址：%s 端口：%d\n",inet_ntoa(s1Addr.sin_addr),ntohs(s1Addr.sin_port));//显示另一个服务器的ip地址和端口
	//system("pause");
	/*为了服务器之间能够进行通信再建立一个收线程*/
	CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)listen_to_service,NULL,NULL,NULL);

	/*开始监听客户端的连接*/
	r = listen(sSocket, 10);//开始监听
	if (SOCKET_ERROR == r)
	{
		printf("监听失败：%d\n",GetLastError());
		closesocket(sSocket);
		WSACleanup();
		return -1;
	}
	printf("正在监听……\n");
	//system("pause");
	SOCKADDR_IN cAddr={0};//客户端协议地址族
	int cAddrLen = sizeof cAddr;

	for(int i=0;i<1024;i++){
		cSocket[i] = accept(sSocket, (sockaddr*)&cAddr, &cAddrLen);//建立客户端与服务器的连接
		strcpy(cIPAddr[i],inet_ntoa(cAddr.sin_addr));//获取客户端的ip地址
		if (SOCKET_ERROR == cSocket[i])
			{
				printf("网络崩溃！\n");
				closesocket(sSocket);
				WSACleanup();
				return -1;
			}
		printf("客户端连接成功：%s\n",inet_ntoa(cAddr.sin_addr));//显示客户端的ip地址
		Count++;

		Client[i].client_num = i;

		CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)communicat,(LPVOID)i,NULL,NULL);//每个客户端都开一个线程
	}
	

	closesocket(sSocket);
	WSACleanup();
}

void communicat(int idx){
	//client *Client;
	char nickName[20];
	char userName[20];//存储各个客户端的用户名和昵称
	int n;
	n=recv(cSocket[idx],userName,19,NULL);
	userName[n]=0;
	n=recv(cSocket[idx],nickName,19,NULL);
	nickName[n]=0;
	printf("Client%d:\nuserName:%s\nnickName:%s\n",idx,userName,nickName);//服务器显示客户端的用户名和昵称
	//Client->login(userName,nickName);//向类里面存数据
	nickName[n]=':';nickName[n+1]=0;
	while(1){
		char buff[1024];//接收来自客户端的数据
		const char *channel_name;
		string str,command,param;//命令行的内容
		size_t pos;
		int r;
		while(1){
		r = recv(cSocket[idx],buff,1023,NULL);
		if(r>0){
			/*```````````````````````````````````````````````````````*/
			buff[r] = 0;
			str = buff;
			if(buff[0] == '/'){//如果输入的是命令行，提取命令名称和参数
				pos = str.find(' ');
				command = str.substr(1,pos-1);
				param = str.substr(pos+1,str.length()-pos-1);
			}
			if(command == "join"){
				Client[idx].channelName = param;
				channel_name = Client[idx].channelName.c_str();
			}
			else if(command == "part"){//退出频道
				Client[idx].channelName = "";
				send(cSocket[idx],"成功退出！\n",sizeof("成功退出！\n"),NULL);
			}
			else if(command == "topic"){//获取频道主题

			}
			else if(command == "nick"){//更改昵称
				memset(nickName,0,20);
				strcpy(nickName,param.c_str());
				Client[idx].Login(userName,nickName);
			}
			/*```````````````````````````````````````````````````````*/
			printf("Client%d:%s\n频道名称：%s\n\n",idx,buff,channel_name);

			//向另一个服务器也发一份channelName+nickname+buff
			send(s1Socket,channel_name,sizeof(channel_name),NULL);
			send(s1Socket,nickName,n+1,NULL);
			send(s1Socket,buff,r+1,NULL);
			
			for(int i=0;i<Count;i++){//向本服务器channel内的客户端广播
			if(i!=idx && Client[i].channelName == Client[idx].channelName){
				send(cSocket[i],nickName,n+1,NULL);
				send(cSocket[i],buff,r+1,NULL);
			}
			}
		}
	}
	}
}

void listen_to_service(){//接收另一个服务器的信息
	char buff[1024],channel_Name[20]={0};
	int r;
	while(1){
		r = recv(s1Socket,buff,1023,NULL);
		if(r>0){
			buff[r] = 0;
			//printf("buff:%s\n",buff);
			if(buff[0] == '#'){
				strcpy(channel_Name,buff);
				//printf("频道名称：%s\n",channel_Name);
			}
			else{
				for(int i=0;i<Count;i++){//广播
					if(Client[i].channelName == channel_Name && buff[0] != '/')
						send(cSocket[i],buff,r+1,NULL);
				}
			}
			
		}
	}
}