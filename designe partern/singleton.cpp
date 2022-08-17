#include <cstdlib>
#include <mutex>
// volitile
#include <mutex>
#include <atomic>
class Singleton_1
{
public:
    //��̬������ʹ�ÿ��Բ���������ֱ�ӷ���
    static Singleton_1 *get_ins()
    {
        if (sig_ == nullptr)
        {
            sig_ = new Singleton_1();
            return sig_;
        }
        return sig_;
    };

private:
    Singleton_1()
    {
    }
    ~Singleton_1()
    {
    }
    Singleton_1(const Singleton_1 &rhs)
    {
    }
    Singleton_1 operator=(const Singleton_1 &rhs)
    {
    }
    static Singleton_1 *sig_;
};

//��̬ȫ�ֳ�ʼ��
//����˵����һ��Ҫ��
Singleton_1 *Singleton_1::sig_ = nullptr;
class Singleton2
{
public:
    static Singleton2 *GetInstance()
    {
        if (_instance == nullptr)
        {
            _instance = new Singleton2();
            atexit(Destructor);
        }
        return _instance;
    }
    ~Singleton2() {}

private:
    static void Destructor()
    {
        if (nullptr != _instance)
        { //
            delete _instance;
            _instance = nullptr;
        }
    }
    Singleton2(); //����
    ~Singleton2() {}
    Singleton2(const Singleton2 &cpy); //��?����
    Singleton2 &operator=(const Singleton2 &other) {}
    static Singleton2 *_instance;
};

//�������������Ķ����̲߳���ȫ��
class Singleton3
{ // ����ģʽ lazy load
public:
    static Singleton3 *GetInstance()
    {
        // std::lock_guard<std::mutex> lock(_mutex); // 3.1 �л��߳�
        if (_instance == nullptr)
        {
            std::lock_guard<std::mutex> lock(_mutex); // 3.2
            if (_instance == nullptr)
            {
                _instance = new Singleton3();
                // 1. �����ڴ�
                // 2. ���ù��캯��
                // 3. ����ָ��
                // ���̻߳����� cpu reorder����
                //����żȻ��
                atexit(Destructor);
            }
        }
        return _instance;
    }

private:
    static void Destructor()
    {
        if (nullptr != _instance)
        {
            delete _instance;
            _instance = nullptr;
        }
    }
    Singleton3() {}                      //����
    Singleton3(const Singleton3 &cpy) {} //��?����
    Singleton3 &operator=(const Singleton3 &) {}
    static Singleton3 *_instance;
    static std::mutex _mutex;
};
Singleton3 *Singleton3::_instance = nullptr; //��̬��Ա��Ҫ��ʼ��
std::mutex Singleton3::_mutex;               //��������ʼ��

class Singleton
{
public:
    static Singleton *GetInstance()
    {
        Singleton *tmp = _instance.load(std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire); //��ȡ�ڴ�����
        if (tmp == nullptr)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            tmp = _instance.load(std::memory_order_relaxed);
            if (tmp == nullptr)
            {
                tmp = new Singleton;
                std::atomic_thread_fence(std::memory_order_release); //�ͷ���
                //������
                _instance.store(tmp, std::memory_order_relaxed);
                atexit(Destructor);
            }
        }
        return tmp;
    }

private:
    static void Destructor()
    {
        Singleton *tmp = _instance.load(std::memory_order_relaxed);
        if (nullptr != tmp)
        {
            delete tmp;
        }
    }
    Singleton() {}
    Singleton(const Singleton &) {}
    Singleton &operator=(const Singleton &) {}
    static std::atomic<Singleton *> _instance;
    static std::mutex _mutex;
};
std::atomic<Singleton *> Singleton::_instance; //��̬��Ա��Ҫ��ʼ��
std::mutex Singleton::_mutex;                  //��������ʼ��
// g++ Singleton.cpp -o singleton -std=c++11

template <typename T>
class Singleton5
{
public:
    static T &GetInstance()
    {
        static T instance; // ����Ҫ��ʼ��DesignPattern����Ҫ����DesignPattern
        //���캯����ͬʱ�������Ĺ��캯����
        return instance;
    }

protected:
    virtual ~Singleton5() {}
    Singleton5() {} // protected���ι��캯���������ñ�?�̳�
    Singleton5(const Singleton &) {}
    Singleton5 &operator=(const Singleton5 &) {}
};
class DesignPattern : public Singleton5<DesignPattern>
{
friend class Singleton5<DesignPattern>; // friend ����Singleton<T> ���ʵ�DesignPattern���캯�� 
public: ~DesignPattern() {}

private:
    DesignPattern() {}
    DesignPattern(const DesignPattern &) {}
    DesignPattern &operator=(const DesignPattern &) {}
};
