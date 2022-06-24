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
char cIPAddr[1024][15];//�洢�˸����ͻ��˵�ip��ַ
void listen_to_service();
client Client[1024];

int main(void){
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(LOBYTE(wsaData.wVersion)!=2||HIBYTE(wsaData.wVersion)!=2){
		printf("ȷ��Э��汾ʧ�ܣ�%d\n",GetLastError());
		return -1;
	}
	printf("ȷ��Э��汾�ɹ�!\n");

	SOCKET sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//�����׽���
	if (SOCKET_ERROR == sSocket)
	{
		printf("�����׽���ʧ�ܣ�%d\n",GetLastError());
		WSACleanup();//����Э����Ϣ
		return -1;
	}

	SOCKADDR_IN sAddr={0};//���ط�����Э���ַ��
	sAddr.sin_family = AF_INET;
	sAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	sAddr.sin_port = htons(9527);//С��ת���

	int r=bind(sSocket,(sockaddr*)&sAddr,sizeof sAddr);//��
	if(r == SOCKET_ERROR){
		printf("��ʧ�ܣ�%d\n",GetLastError());
		closesocket(sSocket);
		WSACleanup();//����Э����Ϣ
		return -1;
	}
	printf("�󶨳ɹ�!\n");

	/*�˴�Ϊ���ӵ�һ���������ĳ���*/
	/*SOCKADDR_IN s2Addr={0};//���ط�����Э���ַ��
	s2Addr.sin_family = AF_INET;
	s2Addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	s2Addr.sin_port = htons(9527);//С��ת���*/
	s1Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//�������ӵ�һ�����������׽���
	/*r = bind(s1Socket,(sockaddr*)&s2Addr,sizeof s2Addr);//�󶨱��ص�ַ��
	if(r == SOCKET_ERROR){
		printf("��ʧ�ܣ�%d\n",GetLastError());
		closesocket(sSocket);
		WSACleanup();//����Э����Ϣ
		system("pause");
		return -1;
	}
	printf("�󶨳ɹ�!\n");*/
	SOCKADDR_IN s1Addr={0};//��ȡ��һ��������Э���ַ��
	s1Addr.sin_family = AF_INET;
	s1Addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//��һ����������ip
	s1Addr.sin_port = htons(8888);//С��ת��ˣ���һ���������Ķ˿�

    r = connect(s1Socket, (sockaddr*)&s1Addr, sizeof s1Addr);//���ӷ�����
	//system("pause");
	if (SOCKET_ERROR == r)
	{
		printf("����ʧ��:%s\n",GetLastError());
		closesocket(s1Socket);
		WSACleanup();
		return -1;
	}
	printf("���ӷ������ɹ�! ip��ַ��%s �˿ڣ�%d\n",inet_ntoa(s1Addr.sin_addr),ntohs(s1Addr.sin_port));//��ʾ��һ����������ip��ַ�Ͷ˿�
	//system("pause");
	/*Ϊ�˷�����֮���ܹ�����ͨ���ٽ���һ�����߳�*/
	CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)listen_to_service,NULL,NULL,NULL);

	/*��ʼ�����ͻ��˵�����*/
	r = listen(sSocket, 10);//��ʼ����
	if (SOCKET_ERROR == r)
	{
		printf("����ʧ�ܣ�%d\n",GetLastError());
		closesocket(sSocket);
		WSACleanup();
		return -1;
	}
	printf("���ڼ�������\n");
	//system("pause");
	SOCKADDR_IN cAddr={0};//�ͻ���Э���ַ��
	int cAddrLen = sizeof cAddr;

	for(int i=0;i<1024;i++){
		cSocket[i] = accept(sSocket, (sockaddr*)&cAddr, &cAddrLen);//�����ͻ����������������
		strcpy(cIPAddr[i],inet_ntoa(cAddr.sin_addr));//��ȡ�ͻ��˵�ip��ַ
		if (SOCKET_ERROR == cSocket[i])
			{
				printf("���������\n");
				closesocket(sSocket);
				WSACleanup();
				return -1;
			}
		printf("�ͻ������ӳɹ���%s\n",inet_ntoa(cAddr.sin_addr));//��ʾ�ͻ��˵�ip��ַ
		Count++;

		Client[i].client_num = i;

		CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)communicat,(LPVOID)i,NULL,NULL);//ÿ���ͻ��˶���һ���߳�
	}
	

	closesocket(sSocket);
	WSACleanup();
}

void communicat(int idx){
	//client *Client;
	char nickName[20];
	char userName[20];//�洢�����ͻ��˵��û������ǳ�
	int n;
	n=recv(cSocket[idx],userName,19,NULL);
	userName[n]=0;
	n=recv(cSocket[idx],nickName,19,NULL);
	nickName[n]=0;
	printf("Client%d:\nuserName:%s\nnickName:%s\n",idx,userName,nickName);//��������ʾ�ͻ��˵��û������ǳ�
	//Client->login(userName,nickName);//�������������
	nickName[n]=':';nickName[n+1]=0;
	while(1){
		char buff[1024];//�������Կͻ��˵�����
		const char *channel_name;
		string str,command,param;//�����е�����
		size_t pos;
		int r;
		while(1){
		r = recv(cSocket[idx],buff,1023,NULL);
		if(r>0){
			/*```````````````````````````````````````````````````````*/
			buff[r] = 0;
			str = buff;
			if(buff[0] == '/'){//���������������У���ȡ�������ƺͲ���
				pos = str.find(' ');
				command = str.substr(1,pos-1);
				param = str.substr(pos+1,str.length()-pos-1);
			}
			if(command == "join"){
				Client[idx].channelName = param;
				channel_name = Client[idx].channelName.c_str();
			}
			else if(command == "part"){//�˳�Ƶ��
				Client[idx].channelName = "";
				send(cSocket[idx],"�ɹ��˳���\n",sizeof("�ɹ��˳���\n"),NULL);
			}
			else if(command == "topic"){//��ȡƵ������

			}
			else if(command == "nick"){//�����ǳ�
				memset(nickName,0,20);
				strcpy(nickName,param.c_str());
				Client[idx].Login(userName,nickName);
			}
			/*```````````````````````````````````````````````````````*/
			printf("Client%d:%s\nƵ�����ƣ�%s\n\n",idx,buff,channel_name);

			//����һ��������Ҳ��һ��channelName+nickname+buff
			send(s1Socket,channel_name,sizeof(channel_name),NULL);
			send(s1Socket,nickName,n+1,NULL);
			send(s1Socket,buff,r+1,NULL);
			
			for(int i=0;i<Count;i++){//�򱾷�����channel�ڵĿͻ��˹㲥
			if(i!=idx && Client[i].channelName == Client[idx].channelName){
				send(cSocket[i],nickName,n+1,NULL);
				send(cSocket[i],buff,r+1,NULL);
			}
			}
		}
	}
	}
}

void listen_to_service(){//������һ������������Ϣ
	char buff[1024],channel_Name[20]={0};
	int r;
	while(1){
		r = recv(s1Socket,buff,1023,NULL);
		if(r>0){
			buff[r] = 0;
			//printf("buff:%s\n",buff);
			if(buff[0] == '#'){
				strcpy(channel_Name,buff);
				//printf("Ƶ�����ƣ�%s\n",channel_Name);
			}
			else{
				for(int i=0;i<Count;i++){//�㲥
					if(Client[i].channelName == channel_Name && buff[0] != '/')
						send(cSocket[i],buff,r+1,NULL);
				}
			}
			
		}
	}
}