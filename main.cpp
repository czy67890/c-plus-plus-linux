#include<stdio.h>
#include<iostream>
#include"inc.h"
using namespace std;
int main(){
    struct a a1;
    int a = 4,b = 8,c = 5;
    int d_1,d_2,d1,d2;
    d_1 =(a<b)||(++a == 5)||(c>b--);
    d_2 = (a>b)&&(++a == 5)||(c>b--);
    
    cout<<"  "<<a<<"  "<<b<<"  "<<c<<endl;
    d1 = (a<b)||(++a == 5)&&(c>b--);
    
    cout<<d1<<"  "<<a<<"  "<<b<<"  "<<c<<endl;
    d2 = (a> b)&&(++a == 5)&&(c > b--);
    cout<<d2<<"  "<<a<<"  "<<b<<"  "<<c<<endl;;
    return 0;
}