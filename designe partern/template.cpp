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
//����ӿڸ���
protected:
    //���仯��ĳ�virtual�����ȥ
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
//����ʵ�ֹǼܵķ���
//�����滻
class Zooshow_levelup : public ZooShow{

};


int main(){
    return 0;
}