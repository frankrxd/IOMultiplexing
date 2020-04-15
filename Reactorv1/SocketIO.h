#pragma once

class SocketIO
{
public:
    SocketIO(int fd):_fd(fd){}

    bool isClosed();
    int readn(char * buff,int len);
    int writen(const char * buff,int len);
    int readline(char * buff,int maxlen);
    ~SocketIO() {}
private:
    int _fd;
};

