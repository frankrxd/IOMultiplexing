#pragma once

#include "Socket.h"
#include "InetAddress.h"
#include "SocketIO.h"
using std::pair;

class TcpConnection
{
public:
    TcpConnection(int peerfd); 
    ~TcpConnection() {}

    string receive();
    void send(const string & msg);
    void shutdown();
    bool isClosed();
    string connectionInfo() const;
private:
    pair<InetAddress,InetAddress> pairInetAddress();
private:

    int _fd;
    //Socket _sock;
    InetAddress _localAddr;
    InetAddress _peerAddr;
    SocketIO _socketIO;
    bool _isShutdownWrite;
};

