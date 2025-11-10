#pragma once
#include<sys/socket.h>
#include<string>
#include<cstring>
#include<json/json.h>
using namespace std;
#define SZ 4
#define SEP " "
#define LINE_SEP "\r\n"
#define SEP_LEN strlen(SEP)
#define LINE_SEP_LEN strlen(LINE_SEP)

//数据长度/r/n数据内容/r/n
bool getMessage(string &buf,int sock,string &ret){
    char res[5];
    while(true){
        ssize_t n=recv(sock,res,SZ,0);
        if(n==0)
        return false;
        res[n]='\0';
        buf+=res;
        size_t p;
        if((p=buf.find(LINE_SEP))==string::npos)
        continue;
        ssize_t p1;
        if((p1=buf.find(LINE_SEP,p+1))==string::npos)
        continue;
        ret=buf.substr(0,p1+LINE_SEP_LEN);
        buf.erase(0,p1+LINE_SEP_LEN);
        return true;
    }
    return false;
}

int uncover(std::string text,std::string &ret){
    int p=text.find(LINE_SEP);
    int datalen=stoi(text.substr(0,p));
    ret=text.substr(p+LINE_SEP_LEN,datalen);
    return 0;
}

int package(std::string data,std::string &ret){
    int datalen=data.size();
    string head=to_string(datalen);
    ret.clear();
    ret+=head;
    ret+=LINE_SEP;
    ret+=data;
    ret+=LINE_SEP;
    return datalen;
}
class request{
public:
    request(){}
    request(const int &x,const int &y,const char &op):_x(x),_y(y),_op(op){};
    std::string serialize(){
        string ret;
#ifdef MYSELF
        ret+=to_string(_x);
        ret+=SEP;
        ret+=_op;
        ret+=SEP;
        ret+=to_string(_y);
#else
        Json::Value root;
        root["first"]=_x;
        root["second"]=_y;
        root["oper"]=_op;

        Json::FastWriter writer;
        ret=writer.write(root);
#endif
        return ret;
    }
    void deserialize(std::string &text){
#ifdef MYSELF
        int l=text.find(SEP);
        _x=stoi(text.substr(0,l));
        int r=text.rfind(SEP);
        _y=stoi(text.substr(r+1));
        _op=text[l+1];
#else
        Json::Value root;
        Json::Reader reader;
        reader.parse(text,root);

        _x=root["first"].asInt();
        _y=root["second"].asInt();
        _op=root["oper"].asInt();
#endif
    }
public:
    int _x;
    int _y;
    char _op;
};
class response{
public:
    response(){}
    std::string serialize(){
        string ret;
#ifdef MYSELF
        ret+=to_string(_exitcode);
        ret+=SEP;
        ret+=to_string(_result);
#else
        Json::Value root;
        root["exitcode"] = _exitcode;
        root["result"] =_result;

        Json::FastWriter writer;
        ret = writer.write(root);
#endif
        return ret;
    }
    void deserialize(std::string &text){
#ifdef MYSELF
        int p=text.find(SEP);
        _exitcode=stoi(text.substr(0,p));
        _result=stoi(text.substr(p+1));
#else
        Json::Value root;
        Json::Reader reader;
        reader.parse(text, root);
        
        _exitcode = root["exitcode"].asInt();
        _result = root["result"].asInt();
#endif
    }
public:
    //0为正确，非0为错误
    int _exitcode;
    int _result;
};