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
    //����һ��δ������ȡֵ������
    future<int> result = std::async(get_a_value);
    cout<<"one called"<<endl;
    //get���������ȴ�
    cout<<result.get()<<endl;//be 30
    cout<<"tow called"<<endl; 
    return 0;   
}
