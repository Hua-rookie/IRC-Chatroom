#include <iostream>
#include <string>

using namespace std;

class client
{
private:
    
    
public:
    string userName;
    string nickName;
    int client_num;
    string channelName;
    client *pnext;
    //client(string user = "***", string nick = "***"): userName(user), nickName(nick){};
    //~client();
    int Login(string user_Name, string nick_Name){//��¼��ʱ����������Ϣ

        userName = user_Name;
        nickName = nick_Name;
        if(userName.length()<=0){
            cout<<"�û�������"<<endl;
            return 0;
        }
        if(nickName.length()<=0 || nickName.length()>9){
            cout<<"�ǳƸ�ʽ����"<<endl;
            return -1;
        }
        return 1;
    }
    string get_nickName(){//channel��ѯ�Ƿ�����ʱ����
        return nickName;
    }
};
