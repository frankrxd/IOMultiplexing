#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <functional>
#include <vector>
using std::cout;
using std::endl;
using std::vector;

#define ERROR_EXIT(msg) do{\
	::perror(msg);\
	::exit(EXIT_FAILURE);\
} while(0)

class Server
{

public:
    Server(){}
    ~Server(){
        cout<<"~Server()"<<endl;
    }

public:
    void create_tcp_socket();
    void set_server_addr(const char * ip,unsigned short port);
    void bind_inet_addr();
    void s_listen();
    int s_accept();
    int setup_connection();
    void ans_client_addr(int );
    void recv_handle_msg(int );
    void set_non_block(int );
    void set_reuse_addr(int );
    void set_reuse_port(int );
    int listenfd();
private:
    int _listenfd;
    struct sockaddr_in _saddr;
    struct sockaddr_in _caddr;

};



