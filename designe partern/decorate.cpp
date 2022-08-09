#include <iostream>
// ��ͨԱ�������۽����ۼƽ��𣬲��ž������֮�⻹���Ŷӽ��𣻺�����ܻ���ӻ�����������ͬʱ���ܲ�����ͬ�Ľ�����ϣ�
// ���۽��� = �������۶� * 4%
// �ۼƽ��� = �ܵĻؿ�� * 0.2%
// ���Ž��� = �Ŷ����۶� * 1%
// ���Ƚ��� = (�������۶�-�������۶�) * 1%
// ���ۺ���Ĳ������ܻ����
using namespace std;
class Context {
public:
    bool isMgr;
    // User user;
    // double groupsale;
};


class CalcBonus {    
public:
    CalcBonus(CalcBonus * c = nullptr) : cc(c) {}
    virtual double Calc(Context &ctx) {
        return 0.0; // ��������
    }
    virtual ~CalcBonus() {}

protected:
    CalcBonus* cc;
};

class CalcMonthBonus : public CalcBonus {
public:
    CalcMonthBonus(CalcBonus * c) : CalcBonus(c) {}
    virtual double Calc(Context &ctx) {
        double mbonus /*= �������̺���*/; 
        return mbonus + cc->Calc(ctx);
    }
};

class CalcSumBonus : public CalcBonus {
public:
    CalcSumBonus(CalcBonus * c) : CalcBonus(c) {}
    virtual double Calc(Context &ctx) {
        double sbonus /*= �������̺���*/; 
        return sbonus + cc->Calc(ctx);
    }
};

class CalcGroupBonus : public CalcBonus {
public:
    CalcGroupBonus(CalcBonus * c) : CalcBonus(c) {}
    virtual double Calc(Context &ctx) {
        double gbnonus /*= �������̺���*/; 
        return gbnonus + cc->Calc(ctx);
    }
};

class CalcCycleBonus : public CalcBonus {
public:
    CalcCycleBonus(CalcBonus * c) : CalcBonus(c) {}
    virtual double Calc(Context &ctx) {
        double gbnonus /*= �������̺���*/; 
        return gbnonus + cc->Calc(ctx);
    }
};

int main() {
    // 1. ��ͨԱ��
    Context ctx1;
    CalcBonus *base = new CalcBonus();
    CalcBonus *cb2 = new CalcSumBonus(base);
    CalcBonus *cb1 = new CalcMonthBonus(cb2);


    cb2->Calc(ctx1);
    // 2. ���ž���
    Context ctx2;
    CalcBonus *cb3 = new CalcGroupBonus(cb2);
    cb3->Calc(ctx2);
}