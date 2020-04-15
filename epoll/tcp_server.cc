#include "server.h"

void Server::create_tcp_socket()
{
   _listenfd = socket(AF_INET,SOCK_STREAM,0);
   if(_listenfd == -1)
       ERROR_EXIT("socket");
}

void Server::set_server_addr(const char * ip,unsigned short port)
{
    _saddr.sin_family = AF_INET;
    _saddr.sin_port = htons(port);
    _saddr.sin_addr.s_addr = inet_addr(ip);
}

void Server::bind_inet_addr()
{
    int ret = bind(_listenfd,(const struct sockaddr *)&_saddr,sizeof(_saddr));
    if(ret == -1)
    {
        close(_listenfd);
        ERROR_EXIT("bind");
    }
}

void Server::s_listen()
{
    int ret = listen(_listenfd,10);
    if(ret == -1)
    {
        close(_listenfd);
        ERROR_EXIT("listen");
    }
}

int Server::s_accept()
{
    int _peerfd = accept(_listenfd,nullptr,nullptr);
    if(_peerfd == -1)
    {
        close(_listenfd);
        ERROR_EXIT("accept");
    }
    return _peerfd;
}

void Server::ans_client_addr(int _peerfd)
{
    socklen_t s_len = sizeof(struct sockaddr_in);
    memset(&_caddr,0,s_len);
    int ret = getpeername(_peerfd,(struct sockaddr *)&_caddr,&s_len);
    if(ret == -1)
        perror("getpeername");
    else {
        cout<< " >>> server " << inet_ntoa(_saddr.sin_addr) << ":" << ntohs(_saddr.sin_port) << " --> " 
            << " client " << inet_ntoa(_caddr.sin_addr) << ":" << ntohs(_caddr.sin_port)
			<< " has connected!" << endl; 
    }
}

int Server::setup_connection()
{
    cout<<" >>> Server is about to accept a new link. "<<endl;
    int _peerfd = s_accept();
    cout<<" >>> Server is serving client with peerfd = "<<_peerfd<<endl;
    ans_client_addr(_peerfd);
    return _peerfd;
}

void Server::recv_handle_msg(int _peerfd,int epollfd,struct epoll_event* epl_events)
{
    char buff[1024] = {0};
    cout<<" >>> Server is ready to recv from peerfd = "<<_peerfd<<endl;
    int ret = recv(_peerfd,buff,sizeof(buff),0);
    if(ret < 0)
    {
        if(ret == -1&&errno == EINTR)
            return;
        else perror("recv");
    }
    else if(ret == 0)
    {
        cout<<" >>>Connection with peerfd = "<<_peerfd<<" has closed!"<<endl;
        struct epoll_event temp;
        temp.data.fd = _peerfd;
        temp.events = EPOLLIN;
        if(epoll_ctl(epollfd,EPOLL_CTL_DEL,_peerfd,nullptr))
            perror("epoll_ctl:delete");
        close(_peerfd);
    }
    else {//业务处理
        cout<<" >>> Server gets "<<ret<<" bytes msg.The contents is { "<<buff<<" }"<<endl;
        
        //发送数据
        ret = send(_peerfd,buff,strlen(buff),0);
        if(ret == -1)
        {
            perror("send");
        }
        //断开连接
        sleep(1);
        close(_peerfd);
        cout<<" >>> Connection with peerfd = "<<_peerfd<<" has finished! "<<endl;
    }
}

void Server::set_non_block(int peerfd)
{
    int flag = fcntl(peerfd,F_GETFL,0);
    if(flag == -1){
		ERROR_EXIT("fcntl");
	}
	int ret = ::fcntl(peerfd, F_SETFL, O_NONBLOCK | flag);
	if(ret == -1) {
		ERROR_EXIT("fcntl");
	}
}

void Server::set_reuse_addr(int on)
{
    int reuse = on;
    if(setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&reuse, sizeof(socklen_t))) {
		ERROR_EXIT("setsockopt");
	}
}
void Server::set_reuse_port(int on) 
{
	int reuse = on;
	if(setsockopt(_listenfd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(int))) {
		ERROR_EXIT("setsockopt");
	}
}


int Server::listenfd()
{
    return _listenfd;
}


void test()
{
    Server Myserver;
    Myserver.create_tcp_socket();
    cout<<" >>> Server listenfd = "<<Myserver.listenfd()<<endl;
//    Myserver.set_reuse_addr(1);
    Myserver.set_server_addr("192.168.0.104",8888);
    Myserver.bind_inet_addr();
    Myserver.s_listen();

    //并发服务器之epoll
    struct epoll_event epl_events[1024]={0};
    struct epoll_event temp;
    temp.events = EPOLLIN;
    temp.data.fd = Myserver.listenfd();

    int epollfd = epoll_create1(0);
    if(epollfd == -1)
        ERROR_EXIT("epoll_create1");
    if(epoll_ctl(epollfd,EPOLL_CTL_ADD,temp.data.fd,&temp) == -1 )
        ERROR_EXIT("epoll_ctl: listenfd");

    while(1)
    {
        int ready = epoll_wait(epollfd,epl_events,1024,5000);
        cout<<" >>> Epoll retval = "<<ready<<endl;
        if(ready == -1 && errno == EINTR)
            continue;
        else if(ready == -1)
            ERROR_EXIT("epoll");
        else if(ready == 0)
            cout<<" >>> Epoll timeout! "<<endl;
        else {//ready>0
            for(size_t i=0;i<ready;++i)
            {
                if(epl_events[i].data.fd == Myserver.listenfd() && 
                   (epl_events[i]).events & EPOLLIN)
                {
                    //处理新连接
                    int _peerfd = Myserver.setup_connection();
                    Myserver.set_non_block(_peerfd);
                    //将新连接加入监听队列
                    temp.data.fd = _peerfd;
                    if(epoll_ctl(epollfd,EPOLL_CTL_ADD,_peerfd,&temp) == -1 )
                        ERROR_EXIT("epoll_ctl:add");
                }
                else if(epl_events[i].events & EPOLLIN)
                {
                    //处理已建好的连接
                    Myserver.recv_handle_msg(epl_events[i].data.fd,epollfd,epl_events);
                }
            }//end of for
        }//end of if
    }//end of while 
    close(Myserver.listenfd());
}
int main()
{
    test();
    return 0;
}

