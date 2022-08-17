#include <cstdlib>
#include <mutex>
// volitile
#include <mutex>
#include <atomic>
class Singleton_1
{
public:
    //静态方法，使得可以不创建对象直接访问
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

//静态全局初始化
//类型说明符一定要有
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
    Singleton2(); //构造
    ~Singleton2() {}
    Singleton2(const Singleton2 &cpy); //拷?构造
    Singleton2 &operator=(const Singleton2 &other) {}
    static Singleton2 *_instance;
};

//上述方法创建的都是线程不安全的
class Singleton3
{ // 懒汉模式 lazy load
public:
    static Singleton3 *GetInstance()
    {
        // std::lock_guard<std::mutex> lock(_mutex); // 3.1 切换线程
        if (_instance == nullptr)
        {
            std::lock_guard<std::mutex> lock(_mutex); // 3.2
            if (_instance == nullptr)
            {
                _instance = new Singleton3();
                // 1. 分配内存
                // 2. 调用构造函数
                // 3. 返回指针
                // 多线程环境下 cpu reorder操作
                //存在偶然性
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
    Singleton3() {}                      //构造
    Singleton3(const Singleton3 &cpy) {} //拷?构造
    Singleton3 &operator=(const Singleton3 &) {}
    static Singleton3 *_instance;
    static std::mutex _mutex;
};
Singleton3 *Singleton3::_instance = nullptr; //静态成员需要初始化
std::mutex Singleton3::_mutex;               //互斥锁初始化

class Singleton
{
public:
    static Singleton *GetInstance()
    {
        Singleton *tmp = _instance.load(std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire); //获取内存屏障
        if (tmp == nullptr)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            tmp = _instance.load(std::memory_order_relaxed);
            if (tmp == nullptr)
            {
                tmp = new Singleton;
                std::atomic_thread_fence(std::memory_order_release); //释放内
                //存屏障
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
std::atomic<Singleton *> Singleton::_instance; //静态成员需要初始化
std::mutex Singleton::_mutex;                  //互斥锁初始化
// g++ Singleton.cpp -o singleton -std=c++11

template <typename T>
class Singleton5
{
public:
    static T &GetInstance()
    {
        static T instance; // 这里要初始化DesignPattern，需要调用DesignPattern
        //构造函数，同时会调用类的构造函数。
        return instance;
    }

protected:
    virtual ~Singleton5() {}
    Singleton5() {} // protected修饰构造函数，才能让别?继承
    Singleton5(const Singleton &) {}
    Singleton5 &operator=(const Singleton5 &) {}
};
class DesignPattern : public Singleton5<DesignPattern>
{
friend class Singleton5<DesignPattern>; // friend 能让Singleton<T> 访问到DesignPattern构造函数 
public: ~DesignPattern() {}

private:
    DesignPattern() {}
    DesignPattern(const DesignPattern &) {}
    DesignPattern &operator=(const DesignPattern &) {}
};
