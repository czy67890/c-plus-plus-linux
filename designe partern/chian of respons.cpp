#include <string>

class Context {
public:
    std::string name;
    int day;
};

// �ȶ��� ����  �仯�� ��չ
class IHandler {
public:
    virtual ~IHandler() {}
    void SetNextHandler(IHandler *next) { // �����ϵ
        next = next;
    }
    // �����ȶ��㣬����չ����
    // ģ��ģʽ���̶��㷨�Ǽܣ�ͨ������ȥ��չ������
    bool Handle(const Context &ctx) {
        if (CanHandle(ctx)) {
            return HandleRequest(ctx);
        } else if (GetNextHandler()) {
            return GetNextHandler()->Handle(ctx);
        } else {
            // err
        }
        return false;
    }
protected:
    virtual bool HandleRequest(const Context &ctx) = 0;
    virtual bool CanHandle(const Context &ctx) =0;
    IHandler * GetNextHandler() {
        return next;
    }
private:
    IHandler *next;
};

// �ܲ��ܴ����Լ���ô����
class HandleByMainProgram : public IHandler {
protected:
    virtual bool HandleRequest(const Context &ctx){
        //
        return true;
    }
    virtual bool CanHandle(const Context &ctx) {
        //
        if (ctx.day <= 10)
            return true;
        return false;
    }
};

class HandleByProjMgr : public IHandler {
protected:
    virtual bool HandleRequest(const Context &ctx){
        //
        return true;
    }
    virtual bool CanHandle(const Context &ctx) {
        //
        if (ctx.day <= 20)
            return true;
        return false;
    }
};
class HandleByBoss : public IHandler {
protected:
    virtual bool HandleRequest(const Context &ctx){
        //
        return true;
    }
    virtual bool CanHandle(const Context &ctx) {
        //
        if (ctx.day < 30)
            return true;
        return false;
    }
};

class HandleByBeauty : public IHandler {
protected:
    virtual bool HandleRequest(const Context &ctx){
        //
        return true;
    }
    virtual bool CanHandle(const Context &ctx) {
        //
        if (ctx.day <= 3)
            return true;
        return false;
    }
};

int main() {
    // IHandler * h1 = new HandleByMainProgram();
    // IHandler * h2 = new HandleByProjMgr();
    // IHandler * h3 = new HandleByBoss();
    // h1->SetNextHandler(h2);
    // h2->SetNextHandler(h3);
// ���󹤳�
// nginx http ���� 
    IHandler * h0 = new HandleByBeauty();
    IHandler * h1 = new HandleByMainProgram();
    IHandler * h2 = new HandleByProjMgr();
    IHandler * h3 = new HandleByBoss();
    h0->SetNextHandler(h1);
    h1->SetNextHandler(h2);
    h2->SetNextHandler(h3);
    // ������һָ�� 
    Context ctx;
    h0->Handle(ctx);
    
    return 0;
}