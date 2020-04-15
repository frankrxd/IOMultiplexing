#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using std::string;

#define ERROR_EXIT(msg){\
    ::perror(msg);\
    ::exit(EXIT_FAILURE);}
#define SOCK_PTR (const struct sockaddr *)

void client()
{
    //创建客户端socket
    int clientfd=::socket(AF_INET,SOCK_STREAM,0);
    if(clientfd == -1)
    {
        ERROR_EXIT("socket");
        return;
    }

    //设置服务端地址和端口号
    string s_ip("192.168.112.128");
    unsigned short s_port = 1120;
    struct sockaddr_in s_socket;
    s_socket.sin_family = AF_INET;
    s_socket.sin_port = htons(s_port);
    s_socket.sin_addr.s_addr = ::inet_addr(s_ip.c_str());

    //客户端发起连接请求
    cout<<" >> client is about to connect server..."<<endl;
    int ret = ::connect(clientfd,SOCK_PTR&s_socket,sizeof(s_socket));
    if(ret == -1)
    {
        ERROR_EXIT("connect");
        ::close(clientfd);
    }
    else
    {
        cout<<" >> client <---> server("
            <<s_ip<<":"<<s_port
            <<") has connected! "<<endl;
    }

    //准备数据
    cout<<" >> pls input some data: "<<endl;
    string data;
    cin>>data;

    //向服务器发送业务请求
    ret =::send(clientfd,data.c_str(),data.size(),0);
    if(ret == -1)
    {
        ERROR_EXIT("send");
        ::close(clientfd);
    }

    //接收服务器返回的数据
    char buff[1024]={0};
    ret = ::recv(clientfd,buff,sizeof(buff),0);
    if(ret == -1)
    {
        ERROR_EXIT("recv");
        ::close(clientfd);
        return;
    }
    else if(ret == 0)
        ::close(clientfd);
    else
    {
        cout<<" >> client has received "<<ret<<" bytes datas"<<endl
            <<" >> received datas: "<<buff<<endl;
    }
}

int main()
{
    client();
    return 0;
}

