#pragma once

#include "Noncopyable.h"

#include <pthread.h>
#include <functional>
namespace project1
{

class Thread
:Noncopyable
{
public:
    using ThreadCallBack = std::function<void()>;
    Thread(ThreadCallBack && callback);
    virtual ~Thread();
    void start();//开启子线程
    void join();//回收子线程
    bool _isRUnning() const {     return _isRunning;  }
    pthread_t getThreadid() const { return _pthid;  }

private:
    static void * threadFunc(void *);
private:

    ThreadCallBack _callback;
    pthread_t _pthid;
    bool _isRunning;
};

}//end of namespace project1
