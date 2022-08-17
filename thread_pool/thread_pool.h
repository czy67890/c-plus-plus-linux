#pragma once
#include<iostream>
#include<mutex>
#include<functional>
#include<queue>
#include<memory>
#include<atomic>
#include<future>
#include<condition_variable>
#include<thread>
#ifdef WIN32
#include<windows.h>
#else
#include<sys/time.h>
#endif
using namespace std;

void getNow(timeval *tv);
int64_t getNowMs();

#define TNOW getNow()
#define TNOWMS getNowMs()

class CZY_ThreadPool{
protected:
    struct TaskFunc{
        TaskFunc(uint64_t expireTime)
            :expire_time_(expireTime)
        {}
        std::function<void()> func_;
        int64_t expire_time_ = 0;
    };
    typedef shared_ptr<TaskFunc> TaskFuncPtr;
public:
    CZY_ThreadPool();

    //虚析构函数,基类的析构函数必须是虚函数
    virtual ~CZY_ThreadPool();

    
    bool init(size_t num);

    size_t getThreadNum(){
        std::unique_lock<std::mutex> lock(mutex_);
        return therads_.size();
    }

    size_t getJobNum(){
        std::unique_lock<std::mutex> lock(mutex_);
        return tasks_.size();
    }

    void stop();

    bool start();

    //可变模板参数

    template<class Func,class ...Args>

    auto exec(Func && f,Args &&... args) ->std::future<decltype(f(args...))>
    {
        return exec(0,f,args...);
    }

    template<class Func,class ...Args>
    
    //exec重载，支持计时操作
    auto exec(int64_t timeoutMs,Func && f,Args&& ... args) ->std::future<decltype(f(args...))>
    {

        int64_t expireTime = (timeoutMs == 0) ? 0 : TNOW + timeoutMs;

        //自动推导返回值类型
        using retType = decltype(f(args...));
        
        //将函数的参数完美转发即可
        //这样可以做到所有的都是无参数的统一形式
        auto task = std::make_shared<std::packaged_task<retType()>>(std::bind(std::forward<F>(f),std::forward<Args>(args)...));

        TaskFuncPtr fPtr = std::make_shared<TaskFunc> (expireTime);

        fPtr->func_ = [](){
            (*task)();
        };

        //互斥访问队列
        std::unique_lock<std::mutex> lock(mutex_);

        tasks_.push(fPtr);

        condition_.notify_one();

        return task->get_future();
    }

    bool waitForAllDone(int millsecond = -1);

protected:
    bool get(TaskFuncPtr &task);

    bool isTerminate(){
        return bterminate_;
    }

    void run();

protected:
    std::mutex mutex_;
    vector<std::thread *> therads_;
    std::condition_variable condition_;
    size_t thread_num;
    bool bterminate_;
    std::queue<TaskFuncPtr> tasks_;
    std::atomic<int> atomic_ {0};
};

int gettimeofday(struct timeval &tv)
{
#if WIN32
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year   = wtm.wYear - 1900;
    tm.tm_mon   = wtm.wMonth - 1;
    tm.tm_mday   = wtm.wDay;
    tm.tm_hour   = wtm.wHour;
    tm.tm_min   = wtm.wMinute;
    tm.tm_sec   = wtm.wSecond;
    tm. tm_isdst  = -1;
    clock = mktime(&tm);
    tv.tv_sec = clock;
    tv.tv_usec = wtm.wMilliseconds * 1000;

    return 0;
#else
    return ::gettimeofday(&tv, 0);
#endif
}

void getNow(timeval *tv)
{
#if TARGET_PLATFORM_IOS || TARGET_PLATFORM_LINUX

    int idx = _buf_idx;
    *tv = _t[idx];
    if(fabs(_cpu_cycle - 0) < 0.0001 && _use_tsc)
    {
        addTimeOffset(*tv, idx);
    }
    else
    {
        TC_Common::gettimeofday(*tv);
    }
#else
    gettimeofday(*tv);
#endif
}

int64_t getNowMs()
{
    struct timeval tv;
    getNow(&tv);

    return tv.tv_sec * (int64_t)1000 + tv.tv_usec / 1000;
}
