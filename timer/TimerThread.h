#pragma once

#include "Thread.h"
#include "timer.h"

namespace project1
{

class TimerThread
{
    using TimerCallBack = std::function<void()>;
public:
    TimerThread(TimerCallBack && cb,int initialTimer,int periodicTime);

    void start();
    void stop();

private:
    Timer _timer;
    Thread _thread;
};


}
