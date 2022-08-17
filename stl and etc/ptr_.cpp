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
    //forward ����ת���Լ�������ֵ����
    l_or_r(std::forward<T>(t));//depend on t,just pass the L or R
    this_thread::sleep_for(10s);
}


bool isfull(){
    return true;
}
int main()
{  
    std::mutex mux;
    //��ȡ��
    std::unique_lock<mutex> lk(mux);
    
    std::condition_variable cond_;
    //������ֱ��ν�ʳɹ�
    
    cond_.wait(lk,isfull());

    return 0;
}
