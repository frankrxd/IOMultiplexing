#pragma once

namespace project1
{
class Noncopyable
{
protected:
    Noncopyable() {}
    ~Noncopyable() {}
    Noncopyable(const Noncopyable &) = delete; //C++11，用于禁用某种操作
    Noncopyable & operator = (const Noncopyable &) = delete;
};
}

