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

//�������ӵ�����
list<AcceptNode*> acceptList;


int main(){
    //����Winsock�⣬��ʼ��socket��Դ
    initialization();
    //�����׽���
    SOCKET s_server;
    s_server = socket(AF_INET,SOCK_STREAM,0);
    //����Ϣ
    bindInfo(&s_server);
    //��socket����Ϊlisen״̬
    changtoListen(&s_server);
    //���ӶԶ��̵߳�֧��
    while (1){
        SOCKET commandSock;
        responseAccept(commandSock,&s_server);
        //����ڵ�
        AcceptNode* _node = new AcceptNode;
        _node->sock=commandSock;
        HANDLE _thread = CreateThread(NULL,0,onInteract,(LPVOID)&_node->sock,0,NULL);
        _node->thread = _thread;
        //����ǰ����Ľڵ��������
        acceptList.push_back(_node);
        //������һ������
    }
    return 0;
}

void initialization(){
    //��ʼ���׽��ֿ�
    WORD w_req = MAKEWORD(2,2); //�汾��
    WSADATA wsadata;
    int err;
    err = WSAStartup(w_req,&wsadata);
    if (err!=0){
        std::cout << "��ʼ���׽��ֿ�ʧ��!" <<std::endl;
    } else{
        std::cout << "��ʼ���׽��ֿ�ɹ�!" <<std::endl;
    }
    //���汾��
    if (LOBYTE(wsadata.wVersion)!=2||HIBYTE(wsadata.wHighVersion)!=2){
        std::cout << "�׽��ְ汾�Ų���ȷ!" <<std::endl;
    }
}
//���׽��� �� һ��ip��ַ�Ͷ˿���
void bindInfo(SOCKET* _socket){
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(6666);//���ؼ���6666�˿�
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    bind(*_socket,(struct sockaddr *)&serverAddr, sizeof(SOCKADDR));
    std::cout<<"���׽��ֳɹ�!"<<std::endl;
}

//�����׽���Ϊ����״̬
void changtoListen(SOCKET* _s_server){
    if (listen(*_s_server,SOMAXCONN)<0){
        std::cout << "���ü���״̬ʧ��!"<<std::endl;
        WSACleanup();
    } else{
        std::cout << "���ü���״̬�ɹ�!"<<std::endl;
    }
    std::cout << "��������ڼ������ӣ����Ժ�....!"<<std::endl;
}

//��Ӧ����״̬
int responseAccept(SOCKET& _sock,SOCKET* _s_server){
    int len = sizeof(SOCKADDR);
    SOCKADDR_IN accept_addr;
    SOCKET s_accept= accept(*_s_server,  (SOCKADDR *)&accept_addr, &len);
    if (s_accept==SOCKET_ERROR){
        std::cout << "����ʧ��!������:"<<WSAGetLastError() <<std::endl;
        WSACleanup();
        return 0;
    }
    _sock = s_accept;
    std::cout << "���ӽ���,׼����������!" <<std::endl;
}

//���÷��ص��׽��ֽ���ͨ��
DWORD onInteract(PVOID param){
    SOCKET* s_accept = static_cast<SOCKET *>(param);
    int recv_len =-1;
    int send_len = -1;
    char recv_buf[100];
    char send_buf[100];
    while (1){
        recv_len = recv(*s_accept,recv_buf,100,0);
        if (recv_len<0){
            std::cout <<"����ʧ��!������:"<<WSAGetLastError()<<std::endl;
            break;
        }else if(recv_len==0){
            std::cout <<"�Ự����!"<<std::endl;
        } else{
            std::cout <<"�ͻ�����Ϣ:"<<recv_buf<<std::endl;
        }
        std::cout <<"������ظ���Ϣ:";
        std::cin>> send_buf;
        send_len = send(*s_accept,send_buf,100,0);
        if(send_len<0){
            std::cout <<"����ʧ��!������:"<<WSAGetLastError() <<std::endl;
            break;
        }
    }
    return 0;
}

//�ر��׽���
int onClose(SOCKET* s_accept,SOCKET* _s_server){
    closesocket(*_s_server);
    closesocket(*s_accept);
    //�ͷ�DLL��Դ
    WSACleanup();
    return 0;
}