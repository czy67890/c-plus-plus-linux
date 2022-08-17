#include<thread>
#include<mutex>
#include<iostream>
#include<future>
using namespace std;

int get_a_value(){
    this_thread::sleep_for(5s);
    return 30;
}

int main(){
    //创建一个未来可以取值的整数
    future<int> result = std::async(get_a_value);
    cout<<"one called"<<endl;
    //get方法阻塞等待
    cout<<result.get()<<endl;//be 30
    cout<<"tow called"<<endl; 
    return 0;   
}
