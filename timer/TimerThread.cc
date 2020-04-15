#include "TimerThread.h"

namespace project1
{

TimerThread::TimerThread(TimerCallBack && cb,int initialTimer,int periodicTime)
    :_timer(std::move(cb),initialTimer,periodicTime)
     ,_thread(std::bind(&Timer::start,&_timer)){}

void TimerThread::start()
{
    _thread.start();
}

void TimerThread::stop()
{
    _timer.stop();
    _thread.join();
}

}//end of namespace project1
