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
    int Login(string user_Name, string nick_Name){//登录的时候存入基本信息

        userName = user_Name;
        nickName = nick_Name;
        if(userName.length()<=0){
            cout<<"用户名错误"<<endl;
            return 0;
        }
        if(nickName.length()<=0 || nickName.length()>9){
            cout<<"昵称格式错误"<<endl;
            return -1;
        }
        return 1;
    }
    string get_nickName(){//channel查询是否重名时调用
        return nickName;
    }
};
