//
// Created by 14394 on 2020/2/6.
//

#include "ClientMainCpp.h"
#include <winsock.h>
#include <iostream>

#pragma comment(lib,"ws2_32.lib")

void initialization();
void onClose(SOCKET& s_server);
void onConnect(SOCKET& _sock);
void onActive(SOCKET& s_server);

int main(){
    initialization();
    SOCKET s_server;
    s_server = socket(AF_INET,SOCK_STREAM,0);
     onConnect(s_server);
    onActive(s_server);
    onClose(s_server);
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

//�ͻ��˽��з���������
void onConnect(SOCKET& _sock){
    SOCKADDR_IN server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
    server_addr.sin_port=htons(6666);
    if (connect(_sock,(SOCKADDR *)&server_addr, sizeof(SOCKADDR))==SOCKET_ERROR){
        std::cout<<"����������ʧ��!"<<std::endl;
        WSACleanup();
    } else{
        std::cout<<"���������ӳɹ�!"<<std::endl;
    }
}

//�����������ͨ��
void onActive(SOCKET& s_server){
    char send_buf[100];
    char recv_buf[100];
    int recv_len;
    int send_len;
    while (1){
        std::cout<<"�����뷢����Ϣ:";
        std::cin>>send_buf;
        send_len = send(s_server,send_buf,100,0);
        if (send_len<0){
            std::cout <<"����ʧ��!"<<std::endl;
            break;
        }
        recv_len = recv(s_server,recv_buf,100,0);
        if (recv_len<0){
            std::cout <<"����ʧ��!"<<std::endl;
            break;
        } else{
            std::cout <<"�������Ϣ:"<<recv_buf<<std::endl;
        }
    }
}

void onClose(SOCKET& s_server){
    closesocket(s_server);
    WSACleanup();
}