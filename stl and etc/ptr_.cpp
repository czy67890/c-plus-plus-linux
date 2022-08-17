#include <memory>
#include <iostream>
#include <thread>
#include<unistd.h>
#include<string>
#include<vector>
#include<mutex>
#include<condition_variable>
using namespace std;

template<typename T>
void l_or_r(T &&t){
    cout<< " R be called"<<endl;
}

template<typename T>
void l_or_r(T & t){
    cout<< " L be called"<<endl;
}


template<typename T>
void test(T && t){
    l_or_r(t);//must L be called no matter t L or R
    l_or_r(std::move(t));//R be called no matter t L or R
    //forward 仅仅转发自己的左右值属性
    l_or_r(std::forward<T>(t));//depend on t,just pass the L or R
    this_thread::sleep_for(10s);
}


bool isfull(){
    return true;
}
int main()
{  
    std::mutex mux;
    //获取锁
    std::unique_lock<mutex> lk(mux);
    
    std::condition_variable cond_;
    //阻塞到直到谓词成功
    
    cond_.wait(lk,isfull());

    return 0;
}
