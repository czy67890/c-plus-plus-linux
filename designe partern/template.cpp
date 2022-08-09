#include<iostream>
using namespace std;
class ZooShow{
public:
    ZooShow(){

    }
public:
    void show(){
        if(show0()){  
            play_game();
        }
        show1();
        show2();
        show3();  
    }
//这里接口隔离
protected:
    //将变化点改成virtual隔离出去
    virtual void show1(){

    }
    virtual void show2(){

    }
    virtual void show3(){

    }
private:
    void play_game(){

    }
};

bool show0(){
    return true;
}
//这里实现骨架的方法
//里氏替换
class Zooshow_levelup : public ZooShow{

};


int main(){
    return 0;
}