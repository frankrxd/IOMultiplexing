#include "Eventloop.h"
#include "Mylogger.h"
#include <memory>
using std::cout;
using std::endl;
using std::shared_ptr;
Eventloop::Eventloop(Acceptor & acceptor)
:_acceptor(acceptor)
,_epollfd(createEpollfd())
,_events(1024)
,_islooping(false){
    addEpollReadfd(_acceptor.fd());
}

void Eventloop::loop()
{
    _islooping = true;
    while(_islooping)
    {
        waitEpoll();
    }
}

void Eventloop::stop()
{
    if(_islooping)
        _islooping = false;
}



int Eventloop::createEpollfd()
{
    int fd = epoll_create1(0);                                       
    if(fd == -1)
        ERROR_LOG("epoll_create1");
    return fd;
}

void Eventloop::addEpollReadfd(int fd)
{
    struct epoll_event temp;
    temp.events = EPOLLIN;
    temp.data.fd = fd;
    if(epoll_ctl(_epollfd,EPOLL_CTL_ADD,temp.data.fd,&temp) == -1 )
        ERROR_LOG("epoll_ctl: listenfd");
}

void Eventloop::delEpollReadfd(int fd)
{
    if(epoll_ctl(_epollfd,EPOLL_CTL_DEL,fd,nullptr) == -1 )
        ERROR_LOG("epoll_ctl: listenfd");
}

void Eventloop::waitEpoll()
{
    int ready;
    do{
        ready = epoll_wait(_epollfd,&*_events.begin(),_events.size(),5000);
    }while(ready == -1 && errno == EINTR);
    cout<<" >>> Epoll retval = "<<ready<<endl;
    if(ready == -1) {  
        ERROR_LOG("epoll");
    }
    else if(ready == 0)
        cout<<" >>> Epoll timeout! "<<endl;
    else {//ready>0
        for(size_t i=0;i<ready;++i)
        {
            if(_events[i].data.fd == _acceptor.fd()&& 
               (_events[i]).events & EPOLLIN)
            {
                //处理新连接
                handleNewConnection();
            }
            else if(_events[i].events & EPOLLIN)
            {
                //处理已建好的连接
                handleMessage(_events[i].data.fd);
            }
        }//end of for
    }//end of if

}

void Eventloop::handleNewConnection()
{
    int peerfd = _acceptor.accept();
    _acceptor.setNonblock(peerfd);
    addEpollReadfd(peerfd);
    shared_ptr<TcpConnection> connect(new TcpConnection(peerfd));
    cout << connect->connectionInfo() <<" has connected!"<<endl;
    _connections[peerfd] = connect;
}

void Eventloop::handleMessage(int peerfd)
{
    auto iter = _connections.find(peerfd);
    if(iter != _connections.end())
    {
        auto x = iter->second;
        if(!x->isClosed())
        {
            string msg = x->receive();
            cout<<"server receive: "<<msg<<endl;
            x->send(msg);
        }
        else
        {
            cout<<x->connectionInfo()<<" has closed!"<<endl;
            delEpollReadfd(peerfd);
            _connections.erase(peerfd);
        }
    }
}
