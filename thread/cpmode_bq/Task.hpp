#pragma once
#include<functional>
class Task{
public:
    using func_t=std::function<int(int,int)>;
    Task(){}
    Task(const int &x,const int &y,func_t func):m_x(x),m_y(y),m_callback(func){}
    ~Task(){}
    int operator()(){
        return m_callback(m_x,m_y);
    }
private:
    func_t m_callback;
    int m_x;
    int m_y;
};