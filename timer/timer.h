#pragma once

#include <functional>

namespace project1
{

class Timer
{
public:
    using TimerCallBack = std::function<void()>;
    Timer(TimerCallBack && callback,int initialTime,int periodicTIme);
    ~Timer();

    void start();
    void stop();
private:
    int createTimerfd();
    void setTimerfd(int initialTime,int periodicTIme);
    void handleRead();
private:
    int _fd;
    int _initialTime;
    int _periodicTime;
    TimerCallBack _callback;
    bool _isStarted;
    
};

}//end of namespace project1 
