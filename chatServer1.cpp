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
#include "client.h"/*·················*/
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
char cIPAddr[1024][15];//存储了各个客户端的ip地址
char s2_Addr[15];//另一个服务器的ip地址

client Client[1024];

/*
为实现对channel更直观的管理，服务器中存储有两个链表：
1.所有连接到服务器的客户端组成的链表
2.每个channel内的成员组成的链表




*/
Channel *pchannel[10];

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

	SOCKADDR_IN sAddr={0};//服务器协议地址族，本服务器为远端服务器
	sAddr.sin_family = AF_INET;
	sAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//("127.0.0.1");
	sAddr.sin_port = htons(8888);//小端转大端(9827);

	int r=bind(sSocket,(sockaddr*)&sAddr,sizeof sAddr);//绑定
	if(r == SOCKET_ERROR){
		printf("绑定失败：%d\n",GetLastError());
		closesocket(sSocket);
		WSACleanup();//清理协议信息
		return -1;
	}
	printf("绑定成功!\n");

	r = listen(sSocket, 10);//开始监听第二个服务器和后面客户端的连接
	if (SOCKET_ERROR == r)
	{
		printf("监听失败：%d\n",GetLastError());
		closesocket(sSocket);
		WSACleanup();
		return -1;
	}
	printf("正在监听……\n");

	SOCKADDR_IN cAddr={0};//客户端协议地址族
	int cAddrLen = sizeof cAddr;

	/*此处为连接第二个服务器的尝试*/
	SOCKADDR_IN s2Addr={0};//第二个服务器协议地址族
	int s2AddrLen = sizeof s2Addr;
	s2Socket = accept(sSocket, (sockaddr*)&s2Addr, &s2AddrLen);//建立与第二个服务器的连接
	if (SOCKET_ERROR == s2Socket){
		printf("网络崩溃！\n");
		closesocket(sSocket);
		WSACleanup();
		return -1;
	}
	printf("连接服务器成功！ip地址：%s\n",inet_ntoa(s2Addr.sin_addr));//显示另一个服务器的ip地址
	/*为了服务器之间能够进行通信再建立一个收线程*/
	CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)listen_to_service,NULL,NULL,NULL);

	/*此处为连接若干客户端（本服务器最多1024个客户端）*/
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

void communicat(int idx){//idx也是对象的编号
	//client *Client;
	char nickName[20];
	char userName[20];//存储各个客户端的用户名和昵称
	int n,channel_num;//当前客户端所在的频道数字
	n=recv(cSocket[idx],userName,19,NULL);
	userName[n]=0;
	n=recv(cSocket[idx],nickName,18,NULL);
	nickName[n]=0;
	printf("Client%d:\nuserName:%s\nnickName:%s\n",idx,userName,nickName);//服务器显示客户端的用户名和昵称
	/*```````````````````````````````````````````````````````*/
	Client[idx].Login(userName,nickName);//向类里面存数据
	
	/*```````````````````````````````````````````````````````*/
	nickName[n]=':';nickName[n+1]=0;
	while(1){//接收来自客户端的数据
		char buff[1024];
		client *pclient = &Client[idx];//pclient是指向当前客户端对象的指针
		const char *channel_name;
		string str,command,param;//命令行的内容
		int r,i=0,ii=-1,check;
		size_t pos;
		while(1){
		r = recv(cSocket[idx],buff,1022,NULL);
		if(r>0){
			/*```````````````````````````````````````````````````````*/
			buff[r] = 0;
			str = buff;
			if(buff[0] == '/'){//如果输入的是命令行，提取命令名称和参数
				pos = str.find(' ');//命令和参数之间有一个空格区分
				if(pos != str.npos){//找到了空格，说明有参数
					command = str.substr(1,pos-1);
					param = str.substr(pos+1,str.length()-pos-1);
				}
				else if(pos == str.npos){//没找到空格，说明没有参数
					command = str.substr(1,str.length()-1);
				}
			}
			if(command == "join"){//加入某个频道（功能好）
				Client[idx].channelName = param;
				
				//第一次尝试将客户端信息存储到channel链表中
				//pclient = &Client[idx];
				for(i=0; i<10;i++){
					if(pchannel[i] != NULL && pchannel[i]->c_topic == param){//pchannel[i]是全局变量，指向每一个channel对象
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
					send(cSocket[idx],"正在建立新的频道……请输入频道密码：",sizeof("正在建立新的频道……请输入频道密码："),NULL);
					recv(cSocket[idx],temp,19,NULL);
					cout<<"频道"<<param<<"密码："<<temp<<endl;
					password = temp;
					send(cSocket[idx],"请输入频道容量：",sizeof("请输入频道容量："),NULL);
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
			else if(command == "part"){//退出频道（有问题）
				Client[idx].channelName = "";
				pchannel[channel_num]->Part(Client[idx].nickName);
				send(cSocket[idx],"成功退出！\n",sizeof("成功退出！\n"),NULL);
				cout<<"Client"<<idx<<":"<<Client[idx].nickName<<"已经成功退出！\n";
				command.clear();
				param.clear();
			}
			else if(command == "topic"){//获取频道主题（功能好）
				string topic;
				topic = pchannel[channel_num]->topic_get();
				send(cSocket[idx],topic.c_str(),sizeof(topic.c_str()),NULL);
				cout<<"当前频道主题："<<topic<<endl;
				command.clear();
				param.clear();
			}
			else if(command == "topich"){//更改频道主题（功能好）
				check = pchannel[channel_num]->topic_change(param);
				switch (check)
				{
				case 1 :send(cSocket[idx],"新话题与原话题相同，无需修改\n",sizeof("新话题与原话题相同，无需修改\n"),NULL);break;
				case 0 :send(cSocket[idx],"修改成功！\n",sizeof("修改成功！\n"),NULL);break;
				case -1:send(cSocket[idx],"无效话题名\n",sizeof("无效话题名\n"),NULL);break;
				default:break;
				}
				for(i=0;i<Count;i++){
					Client[i].channelName = param.c_str();
				}
				channel_name = param.c_str();
				command.clear();
				param.clear();
			}
			else if(command == "nick"){//更改昵称（功能好）
				memset(nickName,0,20);
				strcpy(nickName,param.c_str());
				Client[idx].Login(userName,nickName);
				command.clear();
				param.clear();
			}
			else if(command == "kick"){//根据昵称剔除成员（有问题）
				pchannel[channel_num]->Kick(pclient, param);
				command.clear();
				param.clear();
			}
			else if(command == "list"){//罗列当前频道当前服务器上的用户(有问题)
				pchannel[channel_num]->member_list(cSocket[idx]);
				command.clear();
				param.clear();
			}
			else if(command == "pass"){//获取当前密码（功能好）
				string Pass_word;
				Pass_word = pchannel[channel_num]->password_get();
				send(cSocket[idx],Pass_word.c_str(),sizeof(Pass_word.c_str()),NULL);
				cout<<"当前密码为："<<Pass_word<<endl;
				command.clear();
				param.clear();
			}
			else if(command == "passch"){//修改密码（功能好）
				check = pchannel[channel_num]->password_change(param);
				switch (check)
				{
				case 1 :send(cSocket[idx],"新密码与原密码相同，无需修改\n",sizeof("新密码与原密码相同，无需修改\n"),NULL);break;
				case 0 :send(cSocket[idx],"修改成功！\n",sizeof("修改成功！\n"),NULL);break;
				case -1:send(cSocket[idx],"无效密码\n",sizeof("无效密码\n"),NULL);break;
				default:
					break;
				}
				command.clear();
				param.clear();
			}
			else if(command == "operch"){//更改管理员（功能好）
				check = pchannel[channel_num]->operator_change(Client[idx].nickName);
				switch (check)
				{
				case 1 :send(cSocket[idx],"新管理员与原管理员相同或\n",sizeof("新管理员与原管理员相同\n"),NULL);break;
				case 0 :send(cSocket[idx],"修改成功！\n",sizeof("修改成功！\n"),NULL);break;
				case -1:send(cSocket[idx],"该频道为空或无效昵称\n",sizeof("该频道为空或无效昵称\n"),NULL);break;
				default:
					break;
				}
				command.clear();
				param.clear();
			}
			else if(command == "oper"){//获取管理员昵称（功能好）
				string operatorName;
				operatorName = pchannel[channel_num]->operator_get();
				send(cSocket[idx],operatorName.c_str(),sizeof(operatorName.c_str()),NULL);
				command.clear();
				param.clear();
			}
			else if(command == "diss"){//解散频道（有问题）
				check = pchannel[channel_num]->Dissolve(Client[idx].nickName);
				switch (check)
				{
				case 0 :send(cSocket[idx],"解散成功！\n",sizeof("解散成功！\n"),NULL);break;
				case -1:send(cSocket[idx],"解散失败！\n",sizeof("解散失败！\n"),NULL);break;
				default:
					break;
				}
				command.clear();
				param.clear();
			}
			else if(command == "idg"){//获取成员身份id get
				check = pchannel[channel_num]->identity_get(cSocket[idx],param);
				cout<<pclient->nickName<<endl<<Client[idx].nickName;
				//system("pause");
				command.clear();
				param.clear();
			}
			/*```````````````````````````````````````````````````````*/
			printf("Client%d:%s\n频道名称：%s\n\n",idx,buff,channel_name);

			//向另一个服务器也发一份channelName+nickname+buff
			send(s2Socket,channel_name,sizeof(channel_name),NULL);
			send(s2Socket,nickName,n+1,NULL);
			send(s2Socket,buff,r+1,NULL);
			
			for(int i=0;i<Count;i++){//广播
			if(i!=idx && Client[i].channelName == Client[idx].channelName && buff[0] != '/'){
				send(cSocket[i],nickName,n+1,NULL);
				send(cSocket[i],buff,r+1,NULL);
			}
			}
		}

	}
	}
}

void listen_to_service(){//接收另一个服务器的信息
	char buff[1024],channel_Name[20] = {0};
	int r;
	while(1){
		r = recv(s2Socket,buff,1023,NULL);
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