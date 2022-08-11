#include <memory>
#include <iostream>
#include <thread>
#include<unistd.h>
#include<string>
#include<vector>
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
}


int main()
{  
    string a = "trs";
    vector<string > b;
    b.push_back(std::move(a));
    cout<<a<<endl;
    return 0;
}
