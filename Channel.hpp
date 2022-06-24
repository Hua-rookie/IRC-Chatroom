#include <iostream>
#include <string.h>

#ifndef _BASESMOOTHINGALGORITHM_H_
#define _BASESMOOTHINGALGORITHM_H_
#include "client.h"
#endif

using namespace std;

/*struct client
{
    string username;
    string nickname;
    client(string user = "***", string nick = "***"): username(user), nickname(nick){};
};*/
#define MAX_MEMBER_NUM 5
enum property_Channel {

};

enum member_identity {
    m_operator,
    m_client
};

struct Member_Channel {
    client* m_member;
    member_identity m_identity;
    Member_Channel* next_member;
    Member_Channel(client *client = NULL, member_identity identity = m_client, Member_Channel *next = NULL): m_member(client), m_identity(identity), next_member(next) {};
};

class Channel
{
private:
    
    int c_max_num_member;
    Member_Channel* c_member;
    int num_member;
    string c_password;

public:
    string c_topic;
    Channel(client* client = NULL, const string topic = "###", const string password = "000", const int max_num_member = MAX_MEMBER_NUM);
    ~Channel();
    string topic_get();//获取话题名 （已完成）
    int topic_change(const string topic_new);//更改话题名（已完成）
    string operator_get();//获取管理员名
    int operator_change(const string nickname = "***");//更改管理员
    //operator_change(client *client = NULL);//
    int identity_get(SOCKET Client,const string nickname = "***");//获取成员身份信息
    //identity_get(client *client = NULL);//
    string password_get();//获取密码（已完成）
    int password_change(const string password_new);//修改密码（已完成）
    void member_list(SOCKET Client_SOCK);//罗列全部成员名（已完成）
    void Join(client* client = NULL);//成员申请加入（已完成）
    //Join(string nickname = "***");//
    //Part(client *client = NULL);//
    int Part(const string nickname = "***");//成员申请退出（已完成）
    //Kick(client *client = NULL);//
    int Kick(client* c_operator, const string nickname = "***");//踢出成员（已完成）
    int Dissolve(string nickname = "***");//解散群聊
};

Channel::Channel(client* client, const string topic, const string password, int max_num_member) : c_topic(topic), c_password(password), c_max_num_member(max_num_member) {
    if (client == NULL) {
        num_member = 0;
        cout << "无效用户，未成功建立频道" << endl;
        cout << endl;
    }
    else {
        num_member = 1;
        c_member = new Member_Channel(client, m_operator);
        
        cout << "成功建立频道" << endl;
        cout << "topic: " << c_topic << endl;
        cout << "operator: " << c_member->m_member->nickName << endl;
        cout << "password: " << c_password << endl;
        cout << "max member num: " << c_max_num_member << endl;
        cout << endl;
    }
}
Channel::~Channel() {
    delete[] c_member;
    c_member = NULL;
}
string Channel::topic_get() {//获取话题名
    cout << "当前话题：" << c_topic << endl;
    cout << endl;
    return c_topic;
}

int Channel::topic_change(const string topic_new) {//更改话题名
    if (topic_new.length() > 0) {
        if (topic_new == c_topic) {
            cout << "新话题与原话题相同，无需修改" << endl;
            return 1;
        }
        else {
            cout << "原话题：" << c_topic << endl;
            cout << "新话题：" << topic_new << endl;
            cout << endl;
            c_topic = topic_new;
            return 0;
        }
    }
    else {
        cout << "无效话题名" << endl;
        cout << endl;
        return -1;
    }
}

string Channel::operator_get() {//获取管理员名
    Member_Channel* p = c_member;
    string nickname_operator = "***";
    if (p == NULL) {
        cout << "该频道为空" << endl;
        cout << endl;
        return "###";
    }
    while (p != NULL)
    {
        if (p->m_identity == m_operator) {
            nickname_operator = p->m_member->nickName;
            break;
        }
        else {
            p = p->next_member;
        }
    }
    if (nickname_operator == "***") {
        cout << "该频道无管理员" << endl;
    }
    else {
        cout << "当前管理员：" << nickname_operator << endl;
    }
    cout << endl;
    return nickname_operator;
}

int Channel::operator_change(const string nickname) {//更改管理员
    Member_Channel *p = c_member, *opr = NULL;
    string nickname_client = "***";
    if (p == NULL) {
        cout << "该频道为空" << endl;
        cout << endl;
        return -1;
    }
    if (nickname == "***" || nickname.length() == 0) {
        cout << "无效昵称" << endl;
        cout << endl;
        return -1;
    }

    while (p != NULL) {
        if (p->m_identity == m_operator){
            opr = p;
        }
        if (nickname == p->m_member->nickName){
            if (p->m_identity == m_operator){
                cout << "新管理员与原管理员相同，无需修改" << endl;
                cout << endl;
                return 1;
            }
            else{
                p->m_identity = m_operator;
                nickname_client = nickname;
            }
        }
        p = p->next_member;
    }
    if (nickname_client == "***"){
        cout << "频道不存在该用户" << endl;
        cout << endl;
        return -1;
    }
    else{
        opr->m_identity = m_client;
        cout << "原管理员：" << opr->m_member->nickName << endl;
        cout << "当前管理员：" << nickname_client << endl;
        cout << endl;
        return 0;
    }
}
//operator_change(client *client = NULL);//
int Channel::identity_get(SOCKET Client,const string nickname) {//获取成员身份信息
    Member_Channel* p = c_member;
    //string nickname_client = "***";
    if (p == NULL) {
        cout << "该频道为空" << endl;
        cout << endl;
        send(Client,"该频道为空\n",sizeof("该频道为空\n"),NULL);
        return -1;
    }
    if (nickname == "***" || nickname.length() == 0) {
        cout << "无效昵称" << endl;
        cout << endl;
        send(Client,"无效昵称\n",sizeof("无效昵称\n"),NULL);
        return -1;
    }
    while (p != NULL) {
        if (nickname == p->m_member->nickName) {
            //nickname_client = p->m_member->nickname;
            cout << "该用户的身份是：" << p->m_identity << endl;
            cout << endl;
            switch (p->m_identity)
            {
            case m_client:send(Client,"Client\n",sizeof("Client\n"),NULL);break;
            case m_operator:send(Client,"Operator\n",sizeof("Operator\n"),NULL);break;
            default:break;
            }
            return p->m_identity;
        }
        else {
            p = p->next_member;
        }
    }
    cout << "频道不存在该用户" << endl;
    cout << endl;
    send(Client,"频道不存在该用户\n",sizeof("频道不存在该用户\n"),NULL);
    return -1;
}
//identity_get(client *client = NULL);//
string Channel::password_get() {//获取密码
    cout << "当前密码：" << c_password << endl;
    cout << endl;
    return c_password;
}

int Channel::password_change(const string password_new) {//修改密码
    if (password_new.length() > 0) {
        if (password_new == c_password) {
            cout << "新密码与原密码相同，无需修改" << endl;
            cout << endl;
            return 1;
        }
        else {
            cout << "原密码：" << c_password << endl;
            cout << "新密码：" << password_new << endl;
            //system("pause");
            cout << endl;
            //system("pause");
            c_password = password_new;
            return 0;
        }
    }
    else {
        cout << "无效密码" << endl;
        cout << endl;
        return -1;
    }
}
void Channel::member_list(SOCKET Client_SOCK) {//罗列全部成员名
    Member_Channel* p = c_member;
    if (p == NULL) {
        cout << "该频道为空" << endl;
        send(Client_SOCK,"该频道为空！\n",sizeof("该频道为空！\n"),NULL);
    }
    else {
        cout << "昵称     " << "身份" << endl;
        send(Client_SOCK,"昵称     身份\n",sizeof("昵称     身份\n"),NULL);
        while (p != NULL)
        {
            cout << p->m_member->nickName << "     " << p->m_identity << endl;
            send(Client_SOCK,p->m_member->nickName.c_str(),sizeof(p->m_member->nickName.c_str()),NULL);
            send(Client_SOCK,"     ",sizeof("     "),NULL);
            switch (p->m_identity)
            {
            case 0:send(Client_SOCK,"Operator\n",sizeof("Operator\n"),NULL);break;
            case 1:send(Client_SOCK,"Client\n",sizeof("Client\n"),NULL);break;
            default:
                break;
            }
            p = p->next_member;
        }
    }
    cout << endl;
}
void Channel::Join(client* client) {//成员申请加入
    if (client == NULL) {
        cout << "无效成员" << endl;
        cout << endl;
        //return -1;
    }
    Member_Channel* p = c_member;
    if (num_member == c_max_num_member){
        cout << "该频道已满员" << endl;
        cout << endl;
        //return -1;
    }
    else {
        if (p == NULL) {
            c_member = new Member_Channel(client, m_operator);
        }
        else {
            while (p->next_member != NULL)
            {
                if (client->nickName == p->m_member->nickName){
                    cout << "成员之前已加入该频道" << endl;
                    //return -1;
                }
                p = p->next_member;
            }
            p->next_member = new Member_Channel(client);;
        }
        cout << "频道当前用户数量：" << ++num_member << endl;
       // system("pause");
        //cout << "新用户昵称：" << p->m_member->nickName << endl;
        //system("pause");
        cout << endl;
        //return 0;
    }
}
//Join(string nickname = "***");//
//Part(client *client = NULL);//
int Channel::Part(const string nickname) {//成员申请退出
    Member_Channel* p = c_member, * p_last = c_member;
    if (nickname == "***" || nickname.length() == 0) {
        cout << "无效昵称" << endl;
        cout << endl;
        return -1;
    }
    if (p == NULL) {
        cout << "该频道为空" << endl;
        cout << endl;
        return -1;
    }
    else {
        while (p != NULL) {
            if (nickname == p->m_member->nickName) {
                if (p == c_member) {
                    if (p->next_member == NULL){
                        c_member = NULL;
                        num_member--;
                        cout << "最后一位频道成员退出，频道解散" << endl;
                        cout << endl;
                        return 0;
                    }
                    else{
                        c_member = c_member->next_member;
                    }
                }
                else {
                    p_last->next_member = p->next_member;
                }
                cout << p->m_member->nickName << " 已退出频道" << endl;
                if (m_operator == p->m_identity) {
                    c_member->m_identity = m_operator;
                    cout << "管理员已退出频道" << endl;
                    cout << "新管理员：" << c_member->m_member->nickName << endl;
                }
                num_member--;
                cout << endl;
                return 0;
            }
            else {
                p_last = (p == c_member) ? p_last : p;
                p = p->next_member;
            }
        }
        cout << "频道不存在该用户" << endl;
        cout << endl;
        return -1;
    }
}
//Kick(client *client = NULL);//
int Channel::Kick(client* c_operator, const string nickname) {//踢出成员
    Member_Channel* p = c_member, * p_last = c_member;
    if (nickname == "***" || nickname.length() == 0) {
        cout << "无效昵称" << endl;
        cout << endl;
        return -1;
    }
    if (p == NULL) {
        cout << "该频道为空" << endl;
        cout << endl;
        return -1;
    }

    while (p != NULL) {
        if (c_operator->nickName == p->m_member->nickName) {
            if (m_client == p->m_identity) {
                cout << "该用户无权限踢出成员" << endl;
                cout << endl;
                return -1;
            }
            else
                break;
        }
        else
            p = p->next_member;
    }

    if (p == NULL) {
        cout << "频道不存在该管理员" << endl;
        cout << endl;
        return -1;
    }
    else
        p = c_member;

    while (p != NULL) {
        if (nickname == p->m_member->nickName) {
            if (p == c_member) {
                if (p->next_member == NULL){
                    c_member = NULL;
                    cout << "最后一位频道成员退出，频道解散" << endl;
                    cout << endl;
                    return 0;
                }
                else{
                    c_member = c_member->next_member;
                }
            }
            else {
                p_last->next_member = p->next_member;
            }
            cout << p->m_member->nickName << " 已被踢出频道" << endl;
            if (m_operator == p->m_identity) {
                c_member->m_identity = m_operator;
                cout << "管理员已退出频道" << endl;
                cout << "新管理员：" << c_member->m_member->nickName << endl;
            }
            cout << endl;
            return 0;
        }
        else {
            p_last = (p == c_member) ? p_last : p;
            p = p->next_member;
        }
    }
    cout << "频道不存在该用户" << endl;
    cout << endl;
    return -1;

}
int Channel::Dissolve(string nickname) {//解散群聊
    Member_Channel* p = c_member;
    if (p == NULL) {
        cout << "该频道为空" << endl;
        cout << endl;
        return -1;
    }
    while (p != NULL) {
        if (nickname == p->m_member->nickName) {
            if (m_client == p->m_identity) {
                cout << "该用户无权限解散频道" << endl;
                cout << endl;
                return -1;
            }
            else
                break;
        }
        else
            p = p->next_member;
    }

    if (p == NULL) {
        cout << "频道不存在该管理员" << endl;
        cout << endl;
        return -1;
    }
    else{
        delete[] c_member;
        c_member = NULL;
        cout << "频道已解散" << endl;
        cout << endl;
        return 0;
    }
}
/*
int main()
{
    client *hua = new client("hua", "messi");

    string topic = "football";
    string password = "123456";
    int max_member_num = 3;
    Channel channel(hua, topic, password, max_member_num);

    string topic_new = "basketball";
    channel.topic_get();
    channel.topic_change(topic_new);
    channel.topic_get();

    string password_new = "654321";
    channel.password_get();
    channel.password_change(password_new);
    channel.password_get();

    client* deng = new client("deng", "lion");
    client* shi = new client("shi", "tiger");
    client* zhang = new client("zhang", "zoro");
    channel.Join(deng);
    channel.Join(shi);
    channel.Join(zhang);
    channel.member_list();

    channel.Part("xxx");
    channel.member_list();
    
    channel.Part("lion");
    channel.member_list();

    channel.Part("messi");
    channel.member_list();

    channel.Join(deng);
    channel.Join(shi);
    channel.Join(zhang);
    channel.member_list();

    channel.Kick(hua, "xxx");
    channel.member_list();

    channel.Kick(shi, "xxx");
    channel.member_list();

    channel.Kick(shi ,"lion");
    channel.member_list();

    channel.Kick(shi, "tiger");
    channel.member_list();

    channel.Join(deng);
    channel.Join(shi);
    channel.Join(zhang);
    channel.member_list();

    channel.identity_get("deng");
    channel.identity_get("tiger");
    channel.identity_get("lion");

    channel.operator_get();

    channel.operator_change("deng");
    channel.operator_get();

    channel.operator_change("tiger");
    channel.operator_get();

    channel.operator_change("lion");
    channel.operator_get();

    channel.member_list();

    
    channel.Dissolve("messi");
    channel.member_list();
    channel.Dissolve("zoro");
    channel.member_list();
    channel.Dissolve("lion");
    channel.member_list();

    system("pause");
}*/
