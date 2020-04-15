# Linux 网络编程

### I/O多路复用场景

> （1）当客户处理多个描述字时（一般是交互式输入和网络套接口），必须使用I/O复用。
> （2）当一个客户同时处理多个套接口时，而这种情况是可能的，但很少出现。
> （3）如果一个TCP服务器既要处理监听套接口，又要处理已连接套接口，一般也要用到I/O复用。
> （4）如果一个服务器即要处理TCP，又要处理UDP，一般要使用I/O复用。
> （5）如果一个服务器要处理多个服务或多个协议，一般要使用I/O复用。
>
> 与多进程和多线程技术相比，I/O多路复用技术的最大优势是系统开销小，系统不必创建进程/线程，也不必 维护这些进程/线程，从而大大减小了系统的开销。



### I/O多路复用之select

#### select

```c++
#include <sys/select.h> 
#include <sys/time.h> 

int select(int maxfdp1,fd_set *readset,fd_set *writeset,fd_set *exceptset,struct timeval *timeout) 
    //返回值：就绪描述符的数目，超时返回0，出错返回-1
    //maxfdp1,指定待测试的描述字个数，为待测的最大描述字+1
    //readset,writeset,exceptset 指定让内核测试读、写和异常的描述字
	//timeout,告知内核等待所指定描述字中的任何一个就绪可花多少时间
```



#### fd_set

```c++
void FD_ZERO(fd_set * fdset); //清空集合
void FD_SET(int fd,fd_set * fdset); //添加
void FD_CLR(int fd,fd_set * fdset); //删除
int FD_ISSET(int fd,fd_set * fdset); //检查是否可以读写
```



#### timeout

```c++
struct timeval{
	long tv_sec; //seconds
    long tv_usec; //ms
}
//永远等待：仅在有一个描述字准备好I/O时才返回。参数设置为nullptr
//等待一个固定时间：在有一个描述字准备好I/O时返回，但不超过设定时间
//不等待：检查后立即返回，即为轮询。参数指向一个timeval结构，其中定时器值为0
```



### I/O多路复用之poll

```c++
int poll(struct pollfd *fds, nfds_t nfds, int timeout);

typedef struct pollfd {
        int fd;                         // 需要被检测或选择的文件描述符
        short events;                   // 对文件描述符fd上感兴趣的事件
        short revents;                  // 文件描述符fd上当前实际发生的事件
} pollfd_t;


```



### I/O多路复用之epoll

```c++
int epoll_create(int size);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);

/* 
1.epoll_create 函数创建一个epoll句柄，参数size表明内核要监听的描述符数量。调用成功时返回一个epoll句柄描述符，失败时返回-1。
2.epoll_ctl 函数注册要监听的事件类型。四个参数解释如下：
	epfd 表示epoll句柄
	op 表示fd操作类型，有如下3种 
		EPOLL_CTL_ADD 注册新的fd到epfd中
		EPOLL_CTL_MOD 修改已注册的fd的监听事件
		EPOLL_CTL_DEL 从epfd中删除一个fd
	fd 是要监听的描述符
	event 表示要监听的事件
3.epoll_wait 函数等待事件的就绪，成功时返回就绪的事件数目，调用失败时返回 -1，等待超时返回 0。
	epfd 是epoll句柄
	events 表示从内核得到的就绪事件集合
	maxevents 告诉内核events的大小
	timeout 表示等待的超时事件
*/
struct epoll_event {
    __uint32_t events;  /* Epoll events */
    epoll_data_t data;  /* User data variable */
};

typedef union epoll_data {
    void *ptr;
    int fd;
    __uint32_t u32;
    __uint64_t u64;
} epoll_data_t;


```



### 定时器

#### timerfd

```c++
// timerfd 可被用于selec/poll/epoll
#include <sys/timerfd.h>
int timerfd_create(int clockid,int flags);
int timefd_settime(int fd,int flags,const struct itimerspec * new_value,struct itimerspec * old_value);

struct timespec{
	time_t tv_sec;
    long tv_nsec;
}
struct itimerspec{
    struct timespec it_interval;
    struct timespec it_value;
}
/*
timerfd_create:
	clockid：
		> CLOCK_REALTIME: 相对时间，自1970.1.1（Unix诞生时间）
		> CLOCK_MONOTONIC: 绝对时间，系统重启到现在的时间
	flags:
		> TFD_NONBLOCK： 非阻塞
		> TFD_CLOEXEC: 同O_CLOEXEC
		> Linux 2.6.26 以上版本指定为0
timerfd_settime:
	fd: timerfd对应的文件描述符
	flags: 
		0: 相对定时器
		TFD_TIMER_ABSTIME： 绝对定时器
	new_value: 设置超时时间，为0则停止定时器
	old_value: 一般设置为NULL，不为NULL时则返回定时器上次的超时时间
*/

```



#### eventfd

```c++
//eventfd 自Linux 2.6.27，主要用于进程或者线程之间的通信
#include <sys/eventfd.h>
int eventfd(unsigned int initval,int flags);

/*
initval:初始化计数器值，该值保存在内核
flags：
	> EFD_NONBLOCK
	> EFD_CLOEXEC
return value:
	返回文件描述符

*/
```











