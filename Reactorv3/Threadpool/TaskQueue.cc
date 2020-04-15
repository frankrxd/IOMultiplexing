#include "TaskQueue.h"
#include <iostream>

using std::cout;
using std::cin;


TaskQueue::TaskQueue(size_t size)
    :_queSize(size)
     ,_queue()
     ,_mutex()
     ,_notFull(_mutex)
     ,_notEmpty(_mutex)
     ,_isUsed(true)   
{}

bool TaskQueue::full() const
{
    return _queue.size() == _queSize;
}

bool TaskQueue::empty() const
{
    return _queue.empty();
}

//push方法运行在生产者线程
void TaskQueue::push(const TaskCallback & task)
{
    MutexLockGuard autolock(_mutex);
    while(full())   //队列满时阻塞
        _notFull.wait(); //防止notFull被异常唤醒

    _queue.push(task);
    _notEmpty.notify();
}

//pop方法运行在消费者线程
TaskCallback TaskQueue::pop()
{
    MutexLockGuard autolock(_mutex);
    while(_isUsed && empty())
        _notEmpty.wait();

    if(_isUsed)
    {
        TaskCallback ret = _queue.front();
        _queue.pop();
        _notFull.notify();
        return ret;
    }
    else return nullptr;
}

void TaskQueue::wakeup()
{
    if(_isUsed)
        _isUsed = false;
    _notEmpty.notifyall();
}
