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
		printf("ȷ��Э��汾ʧ�ܣ�%d\n",GetLastError());
		return -1;
	}
	printf("ȷ��Э��汾�ɹ�!\n");

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//�����׽���
	if (SOCKET_ERROR == clientSocket)
	{
		printf("�����׽���ʧ�ܣ�%d\n",GetLastError());
		WSACleanup();//����Э����Ϣ
		return -1;
	}

	SOCKADDR_IN sAddr={0};//��ȡ������Э���ַ��
	sAddr.sin_family = AF_INET;
	sAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    printf("������������˿ڣ�");
    scanf("%d",&serverPort);
    while(serverPort>65535){
        printf("����������������룺");
        scanf("%d",&serverPort);
    }
	sAddr.sin_port = htons(serverPort);//С��ת���

    int r = connect(clientSocket, (sockaddr*)&sAddr, sizeof sAddr);//���ӷ�����
	if (SOCKET_ERROR == r)
	{
		printf("����ʧ��:%s\n",GetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return -1;
	}
	printf("���ӷ������ɹ�!\n");

    printf("�������û���(userName)��");//���ӷ�����֮��ʼע��
    fflush(stdin);
    gets(userName);
    fflush(stdin);
    printf("�������ǳ�(nickName)��");
    fflush(stdin);
    gets(nickName);
    fflush(stdin);
    //send(clientSocket,userName,strlen(userName),NULL);
    //send(clientSocket,nickName,strlen(nickName),NULL);
    Send(clientSocket,userName,strlen(userName));
    Send(clientSocket,nickName,strlen(nickName));
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)recvServerAndShow, NULL, NULL, NULL);//ר������������Ϣ���߳�

    char buff[1024];//�����������ͨ��
    while(1){
        memset(buff,0,1024);
        //printf("������Ҫ���͵���Ϣ��\n");
        fflush(stdin);
        gets(buff);
        fflush(stdin);
        //send(clientSocket,buff,strlen(buff),NULL);
        Send(clientSocket,buff,strlen(buff));
    }

	closesocket(clientSocket);
	WSACleanup();
}

void recvServerAndShow(){//���շ�������Ϣ����ʾ
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

int Recv(SOCKET Socket, char *buff, int len){//����֮�����
	recv(Socket,buff,sizeof(buff),NULL);
	deAes(buff,len,key);
    return strlen(buff);
}
void Send(SOCKET Socket, char *buff, int len){//����֮ǰ����
	aes(buff,len,key);
	send(Socket,buff,sizeof(buff),NULL);
}