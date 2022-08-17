#include "thread_pool.h"

CZY_ThreadPool::CZY_ThreadPool()
    :thread_num(1),bterminate_(false)
{}

CZY_ThreadPool::~CZY_ThreadPool(){
    stop();
}

bool CZY_ThreadPool::init(size_t num){
    std::unique_lock<mutex> lock(mutex_);
    if(!therads_.empty()){
        return false;
    }
    thread_num = num;
    return true;
}

void CZY_ThreadPool::stop(){
    {
        std::unique_lock<mutex> lock(mutex_);
        bterminate_ = true;
        condition_.notify_all();
    }

    for(size_t i = 0;i < therads_.size() ; ++i){
        //将所有未悬挂的任务运行
        if(therads_[i]->joinable()){
            therads_[i]->join();
        }
        delete therads_[i];
        therads_[i] = nullptr;
    }
    std::unique_lock<mutex> lock(mutex_);
    //保留capacity,重置size,调用析构
    //这里的threads都被设置成了nullptr
    therads_.clear();
}

bool CZY_ThreadPool::start(){
    std::unique_lock<mutex> lock(mutex_);
    if(!therads_.empty()){
        return false;
    }
    for(int i = 0; i < thread_num ; ++i){
        //这里都是推入的run函数，run函数阻塞读任务并且运行
        therads_.push_back(new thread(&CZY_ThreadPool:: run,this));
    }
    return true;
}

bool CZY_ThreadPool::get(TaskFuncPtr& task){
    std::unique_lock<std::mutex> lock(mutex_);

    //条件变量自动加锁，释放锁
    if(tasks_.empty()){
        //把condition的条件写在wait里，这样可以避免
        condition_.wait(lock,[this] {return bterminate_ || !tasks_.empty();});
    }

    if(bterminate_){
        return false;
    }

    if(!tasks_.empty()){
        task = std::move(tasks_.front());
        tasks_.pop();
        return true;
    }
    return false;
}

void CZY_ThreadPool::run(){
    while(!isTerminate()){
        TaskFuncPtr task;
        bool ok = get(task);
        if(ok){
            ++atomic_;
            try{
            if(task->expire_time_ != 0 && task->expire_time_ < TNOWMS){
                //超时任务，打印日志
            }
            else{
                //func是用用户传入的执行函数
                task->func_();
            }
            }
            catch(...){
                
            }
            --atomic_;
            std::unique_lock<std::mutex> lock(mutex_);
            if(atomic_ == 0 && tasks_.empty()){
                condition_.notify_all();
            }

        }
    }
}

bool CZY_ThreadPool::waitForAllDone(int millssecond){
    std::unique_lock<std::mutex> lock(mutex_);
    if(tasks_.empty()){
        return true;
    }
    if(millssecond < 0){
        condition_.wait(lock,[this] {return tasks_.empty();});
        return true;
    }
    else{
        return condition_.wait_for(lock, std::chrono::milliseconds(millssecond), [this] { return tasks_.empty(); });
    }
}
