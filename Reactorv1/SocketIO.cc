#include "SocketIO.h"
#include "Socket.h"
#include <stdio.h>
#include <iostream>

using std::cout;
using std::endl;

bool SocketIO::isClosed()
{
    char buff[1024] = {0};
    int ret = recv(_fd,buff,sizeof(buff),MSG_PEEK);
    return ret==0;
}
#if 0
int SocketIO::readn(char * buff,int len)
{
    int ret = recv(_fd,buff,len,0);
    if(ret == -1)
        ERROR_LOG("recv");
    return ret;
}

int SocketIO::writen(const char * buff,int len)
{
    int ret = send(_fd,buff,len,0);
    if(ret == -1)
        perror("perror");
}
#endif
#if 1
int SocketIO::readn(char * buff,int len)
{
    char *p = buff;
    int left = len;
    while(left > 0)
    {
        int ret =::read(_fd,p,left);
        if(ret == -1 && errno == EINTR)
            continue;
        else if(ret == -1){
            ERROR_LOG("read");
            return len-left;
        }
        else if(ret == 0)
            return len-left;
        else if(ret > 0){
            left -= ret;
            p += ret;
        }
    }
    return len-left;//已发送数据量
}
int SocketIO::writen(const char * buff,int len)
{
    const char *p = buff;
    int left = len;
    while(len > 0)
    {
        int ret =::write(_fd,p,left);
        if(ret == -1 && errno == EINTR)
            continue;
        else if(ret == -1){
            perror("write");
            return len-left;
        }
        else if(ret == 0)
            return len-left;
        else {
            left -= ret;
            p += ret;
        }
    }
    return len-left;//已发送数据量
}
#endif
int SocketIO::readline(char * buff,int maxlen)
{
    char *p = buff;
    char tem[1024]={0};
    int ret = ::recv(_fd,&tem,1024,MSG_PEEK);
    if(ret <= 0){
        return ret;
    }
    cout<<"recv peek "<<ret<<" bytes datas."<<endl;
    for(size_t idx=0;idx<ret;++idx)
    {
        if(tem[idx]=='\n')
        {
            int size = idx+1;
            readn(buff,size);
            p += size;
            *p = '\0';
            return size;
        }
    }
    return readn(buff,maxlen);
}

