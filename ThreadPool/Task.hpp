#pragma once
#include <iostream>
#include <functional>
class Task
{
public:
    Task() {}
    Task(const int &x, const int &y, const char &op) : _x(x), _y(y), _op(op) {}
    ~Task() {}
    void operator()()
    {
        int ret = 0;
        // std::cout<<_x<<' '<<_y<<' '<<_op<<std::endl;
        switch (_op)
        {
        case '+':
            ret = _x + _y;
            break;
        case '-':
            ret = _x - _y;
            break;
        case '*':
            ret = _x * _y;
            break;
        case '/':
        {
            if (_y == 0)
            {
                std::cout<< "div zero wrong"<<std::endl;
                return;
            }
            else
            {
                ret = _x / _y;
            }
            break;
        }
        case '%':
        {
            if (_y == 0)
            {
                std::cout<< "mod zero wrong"<<std::endl;
                return;
            }
            else
            {
                ret = _x % _y;
            }
            break;
        }
        }
        char buffer[64];
        snprintf(buffer, sizeof buffer, "%d %c %d = %d", _x, _op, _y, ret);
        std::cout << buffer << std::endl;
    }

private:
    int _x, _y;
    char _op;
};