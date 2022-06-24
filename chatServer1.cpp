#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdio.h"
#include "winsock2.h"
#include "stdlib.h"
#include "windows.h"
#include "direct.h"
#include "io.h"
#include <time.h>

#ifndef _BASESMOOTHINGALGORITHM_H_
#define _BASESMOOTHINGALGORITHM_H_
#include "client.h"/*����������������������������������*/
#endif

#include <string>
#include <iostream>
#include "Channel.hpp"
#pragma comment(lib,"ws2_32.lib")

using namespace std;

void communicat(int idx);
void listen_to_service();
SOCKET cSocket[1024];
SOCKET s2Socket;
int Count = 0;
char cIPAddr[1024][15];//�洢�˸����ͻ��˵�ip��ַ
char s2_Addr[15];//��һ����������ip��ַ

client Client[1024];

/*
Ϊʵ�ֶ�channel��ֱ�۵Ĺ����������д洢����������
1.�������ӵ��������Ŀͻ�����ɵ�����
2.ÿ��channel�ڵĳ�Ա��ɵ�����




*/
Channel *pchannel[10];

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

	SOCKADDR_IN sAddr={0};//������Э���ַ�壬��������ΪԶ�˷�����
	sAddr.sin_family = AF_INET;
	sAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//("127.0.0.1");
	sAddr.sin_port = htons(8888);//С��ת���(9827);

	int r=bind(sSocket,(sockaddr*)&sAddr,sizeof sAddr);//��
	if(r == SOCKET_ERROR){
		printf("��ʧ�ܣ�%d\n",GetLastError());
		closesocket(sSocket);
		WSACleanup();//����Э����Ϣ
		return -1;
	}
	printf("�󶨳ɹ�!\n");

	r = listen(sSocket, 10);//��ʼ�����ڶ����������ͺ���ͻ��˵�����
	if (SOCKET_ERROR == r)
	{
		printf("����ʧ�ܣ�%d\n",GetLastError());
		closesocket(sSocket);
		WSACleanup();
		return -1;
	}
	printf("���ڼ�������\n");

	SOCKADDR_IN cAddr={0};//�ͻ���Э���ַ��
	int cAddrLen = sizeof cAddr;

	/*�˴�Ϊ���ӵڶ����������ĳ���*/
	SOCKADDR_IN s2Addr={0};//�ڶ���������Э���ַ��
	int s2AddrLen = sizeof s2Addr;
	s2Socket = accept(sSocket, (sockaddr*)&s2Addr, &s2AddrLen);//������ڶ���������������
	if (SOCKET_ERROR == s2Socket){
		printf("���������\n");
		closesocket(sSocket);
		WSACleanup();
		return -1;
	}
	printf("���ӷ������ɹ���ip��ַ��%s\n",inet_ntoa(s2Addr.sin_addr));//��ʾ��һ����������ip��ַ
	/*Ϊ�˷�����֮���ܹ�����ͨ���ٽ���һ�����߳�*/
	CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)listen_to_service,NULL,NULL,NULL);

	/*�˴�Ϊ�������ɿͻ��ˣ������������1024���ͻ��ˣ�*/
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

void communicat(int idx){//idxҲ�Ƕ���ı��
	//client *Client;
	char nickName[20];
	char userName[20];//�洢�����ͻ��˵��û������ǳ�
	int n,channel_num;//��ǰ�ͻ������ڵ�Ƶ������
	n=recv(cSocket[idx],userName,19,NULL);
	userName[n]=0;
	n=recv(cSocket[idx],nickName,18,NULL);
	nickName[n]=0;
	printf("Client%d:\nuserName:%s\nnickName:%s\n",idx,userName,nickName);//��������ʾ�ͻ��˵��û������ǳ�
	/*```````````````````````````````````````````````````````*/
	Client[idx].Login(userName,nickName);//�������������
	
	/*```````````````````````````````````````````````````````*/
	nickName[n]=':';nickName[n+1]=0;
	while(1){//�������Կͻ��˵�����
		char buff[1024];
		client *pclient = &Client[idx];//pclient��ָ��ǰ�ͻ��˶����ָ��
		const char *channel_name;
		string str,command,param;//�����е�����
		int r,i=0,ii=-1,check;
		size_t pos;
		while(1){
		r = recv(cSocket[idx],buff,1022,NULL);
		if(r>0){
			/*```````````````````````````````````````````````````````*/
			buff[r] = 0;
			str = buff;
			if(buff[0] == '/'){//���������������У���ȡ�������ƺͲ���
				pos = str.find(' ');//����Ͳ���֮����һ���ո�����
				if(pos != str.npos){//�ҵ��˿ո�˵���в���
					command = str.substr(1,pos-1);
					param = str.substr(pos+1,str.length()-pos-1);
				}
				else if(pos == str.npos){//û�ҵ��ո�˵��û�в���
					command = str.substr(1,str.length()-1);
				}
			}
			if(command == "join"){//����ĳ��Ƶ�������ܺã�
				Client[idx].channelName = param;
				
				//��һ�γ��Խ��ͻ�����Ϣ�洢��channel������
				//pclient = &Client[idx];
				for(i=0; i<10;i++){
					if(pchannel[i] != NULL && pchannel[i]->c_topic == param){//pchannel[i]��ȫ�ֱ�����ָ��ÿһ��channel����
						//system("pause");
						pchannel[i]->Join(pclient);
						channel_num = i;
						Client[idx].channelName = param;
						break;
					}
					else if(pchannel[i] ==NULL){
						ii = i;
						channel_num = i;
						break;
					}
				}
				if(i==ii){
					char temp[20];
					string password;
					int max;
					send(cSocket[idx],"���ڽ����µ�Ƶ������������Ƶ�����룺",sizeof("���ڽ����µ�Ƶ������������Ƶ�����룺"),NULL);
					recv(cSocket[idx],temp,19,NULL);
					cout<<"Ƶ��"<<param<<"���룺"<<temp<<endl;
					password = temp;
					send(cSocket[idx],"������Ƶ��������",sizeof("������Ƶ��������"),NULL);
					memset(temp,0,20);
					recv(cSocket[idx],temp,19,NULL);
					max = temp[0]-'0';
					//pclient = &Client[idx];
					//Channel channel(pclient,param,password,max);
					//cout<<pclient->nickName;
					//system("pause");
					pchannel[ii] =new Channel(pclient,param,password,max);
				}
				channel_name = Client[idx].channelName.c_str();
				command.clear();
				param.clear();
			}
			else if(command == "part"){//�˳�Ƶ���������⣩
				Client[idx].channelName = "";
				pchannel[channel_num]->Part(Client[idx].nickName);
				send(cSocket[idx],"�ɹ��˳���\n",sizeof("�ɹ��˳���\n"),NULL);
				cout<<"Client"<<idx<<":"<<Client[idx].nickName<<"�Ѿ��ɹ��˳���\n";
				command.clear();
				param.clear();
			}
			else if(command == "topic"){//��ȡƵ�����⣨���ܺã�
				string topic;
				topic = pchannel[channel_num]->topic_get();
				send(cSocket[idx],topic.c_str(),sizeof(topic.c_str()),NULL);
				cout<<"��ǰƵ�����⣺"<<topic<<endl;
				command.clear();
				param.clear();
			}
			else if(command == "topich"){//����Ƶ�����⣨���ܺã�
				check = pchannel[channel_num]->topic_change(param);
				switch (check)
				{
				case 1 :send(cSocket[idx],"�»�����ԭ������ͬ�������޸�\n",sizeof("�»�����ԭ������ͬ�������޸�\n"),NULL);break;
				case 0 :send(cSocket[idx],"�޸ĳɹ���\n",sizeof("�޸ĳɹ���\n"),NULL);break;
				case -1:send(cSocket[idx],"��Ч������\n",sizeof("��Ч������\n"),NULL);break;
				default:break;
				}
				for(i=0;i<Count;i++){
					Client[i].channelName = param.c_str();
				}
				channel_name = param.c_str();
				command.clear();
				param.clear();
			}
			else if(command == "nick"){//�����ǳƣ����ܺã�
				memset(nickName,0,20);
				strcpy(nickName,param.c_str());
				Client[idx].Login(userName,nickName);
				command.clear();
				param.clear();
			}
			else if(command == "kick"){//�����ǳ��޳���Ա�������⣩
				pchannel[channel_num]->Kick(pclient, param);
				command.clear();
				param.clear();
			}
			else if(command == "list"){//���е�ǰƵ����ǰ�������ϵ��û�(������)
				pchannel[channel_num]->member_list(cSocket[idx]);
				command.clear();
				param.clear();
			}
			else if(command == "pass"){//��ȡ��ǰ���루���ܺã�
				string Pass_word;
				Pass_word = pchannel[channel_num]->password_get();
				send(cSocket[idx],Pass_word.c_str(),sizeof(Pass_word.c_str()),NULL);
				cout<<"��ǰ����Ϊ��"<<Pass_word<<endl;
				command.clear();
				param.clear();
			}
			else if(command == "passch"){//�޸����루���ܺã�
				check = pchannel[channel_num]->password_change(param);
				switch (check)
				{
				case 1 :send(cSocket[idx],"��������ԭ������ͬ�������޸�\n",sizeof("��������ԭ������ͬ�������޸�\n"),NULL);break;
				case 0 :send(cSocket[idx],"�޸ĳɹ���\n",sizeof("�޸ĳɹ���\n"),NULL);break;
				case -1:send(cSocket[idx],"��Ч����\n",sizeof("��Ч����\n"),NULL);break;
				default:
					break;
				}
				command.clear();
				param.clear();
			}
			else if(command == "operch"){//���Ĺ���Ա�����ܺã�
				check = pchannel[channel_num]->operator_change(Client[idx].nickName);
				switch (check)
				{
				case 1 :send(cSocket[idx],"�¹���Ա��ԭ����Ա��ͬ��\n",sizeof("�¹���Ա��ԭ����Ա��ͬ\n"),NULL);break;
				case 0 :send(cSocket[idx],"�޸ĳɹ���\n",sizeof("�޸ĳɹ���\n"),NULL);break;
				case -1:send(cSocket[idx],"��Ƶ��Ϊ�ջ���Ч�ǳ�\n",sizeof("��Ƶ��Ϊ�ջ���Ч�ǳ�\n"),NULL);break;
				default:
					break;
				}
				command.clear();
				param.clear();
			}
			else if(command == "oper"){//��ȡ����Ա�ǳƣ����ܺã�
				string operatorName;
				operatorName = pchannel[channel_num]->operator_get();
				send(cSocket[idx],operatorName.c_str(),sizeof(operatorName.c_str()),NULL);
				command.clear();
				param.clear();
			}
			else if(command == "diss"){//��ɢƵ���������⣩
				check = pchannel[channel_num]->Dissolve(Client[idx].nickName);
				switch (check)
				{
				case 0 :send(cSocket[idx],"��ɢ�ɹ���\n",sizeof("��ɢ�ɹ���\n"),NULL);break;
				case -1:send(cSocket[idx],"��ɢʧ�ܣ�\n",sizeof("��ɢʧ�ܣ�\n"),NULL);break;
				default:
					break;
				}
				command.clear();
				param.clear();
			}
			else if(command == "idg"){//��ȡ��Ա���id get
				check = pchannel[channel_num]->identity_get(cSocket[idx],param);
				cout<<pclient->nickName<<endl<<Client[idx].nickName;
				//system("pause");
				command.clear();
				param.clear();
			}
			/*```````````````````````````````````````````````````````*/
			printf("Client%d:%s\nƵ�����ƣ�%s\n\n",idx,buff,channel_name);

			//����һ��������Ҳ��һ��channelName+nickname+buff
			send(s2Socket,channel_name,sizeof(channel_name),NULL);
			send(s2Socket,nickName,n+1,NULL);
			send(s2Socket,buff,r+1,NULL);
			
			for(int i=0;i<Count;i++){//�㲥
			if(i!=idx && Client[i].channelName == Client[idx].channelName && buff[0] != '/'){
				send(cSocket[i],nickName,n+1,NULL);
				send(cSocket[i],buff,r+1,NULL);
			}
			}
		}

	}
	}
}

void listen_to_service(){//������һ������������Ϣ
	char buff[1024],channel_Name[20] = {0};
	int r;
	while(1){
		r = recv(s2Socket,buff,1023,NULL);
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