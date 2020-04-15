#include "TcpServer.h"
#include "TcpConnection.h"
#include "Eventloop.h"

#include "Threadpool/Threadpool.h"
#include <unistd.h>
#include <iostream>
using std::cout;
using std::endl;



void onConnection(const shared_ptr<TcpConnection> & connection)
{
    cout << connection->connectionInfo() <<" has connected!"<<endl;
}

void onMessage(const shared_ptr<TcpConnection> & connection)
{
#if 1
    string msg = connection->receive();
    cout<<"server receive: "<<msg<<endl;
    connection->send(msg);
#endif
}

void onClose(const shared_ptr<TcpConnection> & connection)
{
    cout<<connection->connectionInfo()<<" has closed!"<<endl;  
}
void test0(void)
{
    TcpServer server("192.168.2.182",8888);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.setCloseCallback(onClose);

    server.start();
}
 
int main(void)
{
	test0();
	return 0;
}
