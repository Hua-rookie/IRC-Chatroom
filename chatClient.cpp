#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdio.h"
#include "winsock2.h"
#include "stdlib.h"
#include "windows.h"
#include "direct.h"
#include "io.h"
#include <time.h>
#include "iostream"
#ifndef _BASESMOOTHINGALGORITHM_H_
#define _BASESMOOTHINGALGORITHM_H_
#include "Aes.hpp"
#endif
//#include <graphics.h>
#pragma comment(lib,"ws2_32.lib")

SOCKET clientSocket;
void recvServerAndShow();
int Recv(SOCKET Socket, char *buff, int len);
void Send(SOCKET Socket, char *buff, int len);
char nickName[20];
char userName[20];
int serverPort;

char *key = "123456";

int main(void){
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(LOBYTE(wsaData.wVersion)!=2||HIBYTE(wsaData.wVersion)!=2){
		printf("确定协议版本失败：%d\n",GetLastError());
		return -1;
	}
	printf("确定协议版本成功!\n");

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//创建套接字
	if (SOCKET_ERROR == clientSocket)
	{
		printf("创建套接字失败：%d\n",GetLastError());
		WSACleanup();//清理协议信息
		return -1;
	}

	SOCKADDR_IN sAddr={0};//获取服务器协议地址族
	sAddr.sin_family = AF_INET;
	sAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    printf("请输入服务器端口：");
    scanf("%d",&serverPort);
    while(serverPort>65535){
        printf("输入错误，请重新输入：");
        scanf("%d",&serverPort);
    }
	sAddr.sin_port = htons(serverPort);//小端转大端

    int r = connect(clientSocket, (sockaddr*)&sAddr, sizeof sAddr);//连接服务器
	if (SOCKET_ERROR == r)
	{
		printf("连接失败:%s\n",GetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return -1;
	}
	printf("连接服务器成功!\n");

    printf("请输入用户名(userName)：");//连接服务器之后开始注册
    fflush(stdin);
    gets(userName);
    fflush(stdin);
    printf("请输入昵称(nickName)：");
    fflush(stdin);
    gets(nickName);
    fflush(stdin);
    //send(clientSocket,userName,strlen(userName),NULL);
    //send(clientSocket,nickName,strlen(nickName),NULL);
    Send(clientSocket,userName,strlen(userName));
    Send(clientSocket,nickName,strlen(nickName));
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)recvServerAndShow, NULL, NULL, NULL);//专门用来接收消息的线程

    char buff[1024];//与服务器进行通信
    while(1){
        memset(buff,0,1024);
        //printf("请输入要发送的信息：\n");
        fflush(stdin);
        gets(buff);
        fflush(stdin);
        //send(clientSocket,buff,strlen(buff),NULL);
        Send(clientSocket,buff,strlen(buff));
    }

	closesocket(clientSocket);
	WSACleanup();
}

void recvServerAndShow(){//接收服务器消息并显示
    char buff[1024]={0};
    int r;
    while(1){
        //r=recv(clientSocket,buff,1023,NULL);
        r=Recv(clientSocket,buff,1023);
        buff[r]=0;
        if(r>0){
            printf("%s\n",buff);
        }
    }
}

int Recv(SOCKET Socket, char *buff, int len){//接收之后解码
	recv(Socket,buff,sizeof(buff),NULL);
	deAes(buff,len,key);
    return strlen(buff);
}
void Send(SOCKET Socket, char *buff, int len){//发送之前加密
	aes(buff,len,key);
	send(Socket,buff,sizeof(buff),NULL);
}