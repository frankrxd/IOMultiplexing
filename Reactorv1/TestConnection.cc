#include "Acceptor.h"
#include "TcpConnection.h"
#include "Eventloop.h"
#include <unistd.h>
#include <iostream>
using std::cout;
using std::endl;

void test0(void)
{
    Acceptor acceptor("192.168.2.182",8888);
    acceptor.ready();
    Eventloop eventloop(acceptor);
    eventloop.loop();
}
 
int main(void)
{
	test0();
	return 0;
}
