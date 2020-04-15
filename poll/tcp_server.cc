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

void Server::recv_handle_msg(int _peerfd)
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
    Myserver.set_server_addr("192.168.112.128",1120);
    Myserver.bind_inet_addr();
    Myserver.s_listen();

    //并发服务器之poll
    vector<struct pollfd> pfds;
    struct pollfd temp;
    temp.fd = Myserver.listenfd();
    temp.events = POLLIN;
    temp.revents = 0;

    pfds.push_back(temp);

    //unordered_set<int> client_fds;
    while(1)
    {

        int ready = poll(&*pfds.begin(),pfds.size(),5000);
        cout<<" >>> Poll retval = "<<ready<<endl;
        if(ready == -1 && errno == EINTR)
            continue;
        else if(ready == -1)
            ERROR_EXIT("poll");
        else if(ready == 0)
            cout<<" >>> Poll timeout! "<<endl;
        else {//ready>0 
            if(pfds[0].revents & POLLIN)
            {
                //处理新连接
                int _peerfd = Myserver.setup_connection();
                Myserver.set_non_block(_peerfd);//设置成非阻塞
                //将新链接加入监听队列
                temp.fd = _peerfd;
                pfds.push_back(temp);
            }
            for(size_t i=1;i<pfds.size();++i)
            {
                if(pfds[i].revents & POLLIN)
                {
                    Myserver.recv_handle_msg(pfds[i].fd);
                    pfds.erase(pfds.begin()+i);
                }
            }



        }//end of if
    }//end of while 
    close(Myserver.listenfd());
}
int main()
{
    test();
    return 0;
}

