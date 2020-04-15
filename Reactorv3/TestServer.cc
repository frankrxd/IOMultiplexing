#include "TcpServer.h"
#include "TcpConnection.h"
#include "Eventloop.h"

#include "Threadpool.h"
#include <unistd.h>
#include <iostream>
using std::cout;
using std::endl;

class MyTask
{
public:
	MyTask(const string & request, shared_ptr<TcpConnection> connection)
	: _request(request)
	, _connection(connection)
	{}

	//process方法是线程池中的子线程来执行的
	void process()
	{
		//.....do something;
		//decode compute  encode
		string response = doTask();//业务逻辑处理
		
		//_conn->send(response);//不能直接发送，因为现在处于计算线程，并不在IO线程
		//这里涉及到线程间通信: 计算线程要通知IO线程发送数据
		_connection->sendInLoop(response);
	}

    string doTask(){
        return string("hello");
    }
private:
	string _request;
	shared_ptr<TcpConnection> _connection;
};

unique_ptr <Threadpool> threadpoolPtr(new Threadpool(4,10));

void onConnection(const shared_ptr<TcpConnection> & connection)
{
    cout << connection->connectionInfo() <<" has connected!"<<endl;
}

void onMessage(const shared_ptr<TcpConnection> & connection)
{
#if 0
    string msg = connection->receive();
    cout<<"server receive: "<<msg<<endl;
    connection->send(msg);
#endif
    string msg = connection->receive();
    cout<<"server receive: "<<msg<<endl;

    MyTask task(msg,connection);
    threadpoolPtr->addTask(std::bind(&MyTask::process,task));
    threadpoolPtr->start();
}

void onClose(const shared_ptr<TcpConnection> & connection)
{
    cout<<connection->connectionInfo()<<" has closed!"<<endl;  
}
void test0(void)
{
    TcpServer server("192.168.0.103",8888);
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
