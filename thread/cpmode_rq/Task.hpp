#include<functional>
class Task{
public:
    using func_t=std::function<std::string(int,int,char)>;
    Task(){}
    Task(const func_t &fun,const int &x,const int &y,const char &op):\
    _fun(fun),_x(x),_y(y),_op(op){}
    ~Task(){}
    std::string operator()(){
        return _fun(_x,_y,_op);
    }
private:
    func_t _fun;
    int _x,_y;
    char _op;
};