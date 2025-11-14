#pragma once
#include<iostream>
#include<cstdio>
// va_list：一种类型，用于声明一个 “可变参数指针”（保存可变参数的地址信息）。
// va_start：宏，初始化 va_list 变量，使其指向第一个可变参数。
// va_arg：宏，从 va_list 中提取下一个参数（需指定类型），并自动移动指针到下一个参数。
// va_end：宏，结束可变参数的访问，释放相关资源（避免内存泄漏）。
#include<cstdarg>
#include<ctime>
#include<unistd.h>
enum {
    DEBUG,
    NORMAL,
    WARNING,
    ERROR,
    FATAL
};
const char *getLogLevel(const int &level){
    switch(level){
    case DEBUG:return "DEBUG";
        break;
    case NORMAL:return "NORMAL";
        break;
    case WARNING:return "WARNING";
        break;
    case ERROR:return "ERROR";
        break;
    case FATAL:return "FATAL";
        break;
    default:return nullptr;
    }
}

std::string getLocaltime(){
    time_t now = time(nullptr);
    struct tm now_tm;
    localtime_r(&now,&now_tm);
    char time_str[1024];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &now_tm);
    std::string ret=time_str;
    return ret;
}
void logMessage(const int &level,const char *format, ...){
#define NUM 1024
    char buffer[NUM];
    snprintf(buffer,sizeof(buffer),"[%s][%s][pid:%d]",getLogLevel(level),getLocaltime().c_str(),getpid());
    va_list args;
    va_start(args,format);
    char logcontent[NUM];
    vsnprintf(logcontent,sizeof(logcontent),format,args);
    va_end(args);
    std::cout<<buffer<<logcontent<<std::endl;
}