//
// Created by 14394 on 2020/2/6.
//

#include <winsock.h>
#include <iostream>
#include <list>
#include <windows.h>


#pragma comment(lib,"ws2_32.lib")
using  namespace std;


void initialization();
void bindInfo(SOCKET* _socket);
void changtoListen(SOCKET* _s_server);
int responseAccept(SOCKET& _sock,SOCKET* _s_server);
DWORD onInteract(LPVOID s_accept);
int onClose(SOCKET* s_accept,SOCKET* _s_server);

typedef struct {
    HANDLE thread;
    SOCKET sock;
}AcceptNode;

//保存连接的容器
list<AcceptNode*> acceptList;


int main(){
    //加载Winsock库，初始化socket资源
    initialization();
    //创建套接字
    SOCKET s_server;
    s_server = socket(AF_INET,SOCK_STREAM,0);
    //绑定信息
    bindInfo(&s_server);
    //将socket设置为lisen状态
    changtoListen(&s_server);
    //增加对多线程的支持
    while (1){
        SOCKET commandSock;
        responseAccept(commandSock,&s_server);
        //构造节点
        AcceptNode* _node = new AcceptNode;
        _node->sock=commandSock;
        HANDLE _thread = CreateThread(NULL,0,onInteract,(LPVOID)&_node->sock,0,NULL);
        _node->thread = _thread;
        //将当前接入的节点加入容器
        acceptList.push_back(_node);
        //进入下一轮请求
    }
    return 0;
}

void initialization(){
    //初始化套接字库
    WORD w_req = MAKEWORD(2,2); //版本号
    WSADATA wsadata;
    int err;
    err = WSAStartup(w_req,&wsadata);
    if (err!=0){
        std::cout << "初始化套接字库失败!" <<std::endl;
    } else{
        std::cout << "初始化套接字库成功!" <<std::endl;
    }
    //监测版本号
    if (LOBYTE(wsadata.wVersion)!=2||HIBYTE(wsadata.wHighVersion)!=2){
        std::cout << "套接字版本号不正确!" <<std::endl;
    }
}
//绑定套接字 到 一个ip地址和端口上
void bindInfo(SOCKET* _socket){
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(6666);//本地监听6666端口
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    bind(*_socket,(struct sockaddr *)&serverAddr, sizeof(SOCKADDR));
    std::cout<<"绑定套接字成功!"<<std::endl;
}

//设置套接字为监听状态
void changtoListen(SOCKET* _s_server){
    if (listen(*_s_server,SOMAXCONN)<0){
        std::cout << "设置监听状态失败!"<<std::endl;
        WSACleanup();
    } else{
        std::cout << "设置监听状态成功!"<<std::endl;
    }
    std::cout << "服务端正在监听连接，请稍后....!"<<std::endl;
}

//响应连接状态
int responseAccept(SOCKET& _sock,SOCKET* _s_server){
    int len = sizeof(SOCKADDR);
    SOCKADDR_IN accept_addr;
    SOCKET s_accept= accept(*_s_server,  (SOCKADDR *)&accept_addr, &len);
    if (s_accept==SOCKET_ERROR){
        std::cout << "连接失败!错误码:"<<WSAGetLastError() <<std::endl;
        WSACleanup();
        return 0;
    }
    _sock = s_accept;
    std::cout << "连接建立,准备接受数据!" <<std::endl;
}

//利用返回的套接字进行通信
DWORD onInteract(PVOID param){
    SOCKET* s_accept = static_cast<SOCKET *>(param);
    int recv_len =-1;
    int send_len = -1;
    char recv_buf[100];
    char send_buf[100];
    while (1){
        recv_len = recv(*s_accept,recv_buf,100,0);
        if (recv_len<0){
            std::cout <<"接受失败!错误码:"<<WSAGetLastError()<<std::endl;
            break;
        }else if(recv_len==0){
            std::cout <<"会话结束!"<<std::endl;
        } else{
            std::cout <<"客户端信息:"<<recv_buf<<std::endl;
        }
        std::cout <<"请输入回复信息:";
        std::cin>> send_buf;
        send_len = send(*s_accept,send_buf,100,0);
        if(send_len<0){
            std::cout <<"发送失败!错误码:"<<WSAGetLastError() <<std::endl;
            break;
        }
    }
    return 0;
}

//关闭套接字
int onClose(SOCKET* s_accept,SOCKET* _s_server){
    closesocket(*_s_server);
    closesocket(*s_accept);
    //释放DLL资源
    WSACleanup();
    return 0;
}