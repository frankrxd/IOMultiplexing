#pragma once
#include <sys/epoll.h>
#include "Acceptor.h"
#include "TcpConnection.h"
#include <vector>
#include <unordered_map>
#include <memory>
using std::unordered_map;
using std::vector;
using std::shared_ptr;
class Eventloop
{
public:
    Eventloop(Acceptor &);
    ~Eventloop() {
        ::close(_epollfd);
    }
    void loop();
    void stop();
    
private:
    int createEpollfd();
    void addEpollReadfd(int fd);
    void delEpollReadfd(int fd);
    void waitEpoll();
    void handleNewConnection();
    void handleMessage(int );
private:
    Acceptor & _acceptor;
    int _epollfd;
    unordered_map<int,shared_ptr<TcpConnection>> _connections;
    vector<struct epoll_event> _events;
    bool _islooping;
};

