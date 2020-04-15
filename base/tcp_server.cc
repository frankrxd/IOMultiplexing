#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>

using std::string;
using std::cout;
using std::endl;

#define ERROR_EXIT(msg){\
    ::perror(msg);\
    ::exit(EXIT_FAILURE);}

void test()
{
    //创建socket，生成套接口描述符
    int listenfd=socket(AF_INET,SOCK_STREAM,0);//ipv4 TCP 
    if(listenfd==-1)//生成失败
        ERROR_EXIT("socket");
    
    //网络地址，采用大端模式
    unsigned short s_port = 1120;
    string s_ip("192.168.112.128");
    struct sockaddr_in s_socket;//服务端socket
    memset(&s_socket,0,sizeof(struct sockaddr_in));//清空结构体
    s_socket.sin_family = AF_INET;
    s_socket.sin_port = htons(s_port);//置为0时，默认选取为占用的端口
    s_socket.sin_addr.s_addr = inet_addr(s_ip.c_str());//INADDR_ANY 本机ip地址

    //绑定网络地址
    if(-1 == bind(listenfd,(const struct sockaddr *)&s_socket,sizeof(s_socket)))
    {
        ::close(listenfd);
        ERROR_EXIT("bind");
    }

    //监听客户机的连接请求
    if(-1 == ::listen(listenfd,10)) //第二个参数设置最大连接数
    {
        ::close(listenfd);
        ERROR_EXIT("listen");
    }

    //并发服务器模型之 循环（迭代）服务器
    while(1)
    {
        //建立新连接
        cout<<" >> server is about to accept a new link."<<endl;
        struct sockaddr_in c_socket;//客户端socket
        memset(&c_socket,0,sizeof(struct sockaddr_in));
        socklen_t len=sizeof(struct sockaddr_in);

        int peerfd=::accept(listenfd,(struct sockaddr*)&c_socket,&len);
        if(peerfd==-1)
        {
            ::close(listenfd);
            ERROR_EXIT("accept");
        }
        //如果peerfd>0，则新连接建议完毕，之后通过peerfd与对端进行通信
        //peerfd即表示一个建立好的连接
        int ret=::getpeername(peerfd,(struct sockaddr*)&c_socket,&len);
        if(ret==-1)
            perror("getpeername");
        else {
            string c_ip(inet_ntoa(c_socket.sin_addr));//32位二进制地址转换为点分十进制
            unsigned short c_port = ntohs(c_socket.sin_port);//网络字节序转主机字节序
            cout<<" >> server "<<s_ip<<":"<<s_port<<" --> "
                <<" client "<<c_ip<<":"<<c_port
                <<" has connected! "<<endl;
        }

        //接受数据
        char buff[1024]={0};//应用层接受缓冲区
        cout<<" >> server is ready to recv data."<<endl;
        ret=::recv(peerfd,buff,sizeof(buff),0); //默认情况下，是阻塞式函数
        cout<<" >> server recv ret = "<<ret<<endl;
        //ret 表示已经接受到了的对端发送数据的长度
        if(ret<0)
        {
            if(ret==-1&&errno==EINTR)
                continue;
            else ERROR_EXIT("recv");
        }
        else if(ret == 0)//连接断开
            ::close(peerfd);
        else
        {
            //对接受数据进行处理（业务逻辑）
            cout<<" >> server gets msg from client: "<<buff<<endl<<endl;

            //数据发送/回显操作
            ret = ::send(peerfd,buff,strlen(buff),0);
            if(ret == -1)
            {
                ERROR_EXIT("send");
                ::close(peerfd);
            }

            //断开连接
            ::sleep(1);
            ::close(peerfd);
        }//end of if
    }//end of while
}
int main()
{
    test();
    return 0;
}

