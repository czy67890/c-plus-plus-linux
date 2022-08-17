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
        //������δ���ҵ���������
        if(therads_[i]->joinable()){
            therads_[i]->join();
        }
        delete therads_[i];
        therads_[i] = nullptr;
    }
    std::unique_lock<mutex> lock(mutex_);
    //����capacity,����size,��������
    //�����threads�������ó���nullptr
    therads_.clear();
}

bool CZY_ThreadPool::start(){
    std::unique_lock<mutex> lock(mutex_);
    if(!therads_.empty()){
        return false;
    }
    for(int i = 0; i < thread_num ; ++i){
        //���ﶼ�������run������run��������������������
        therads_.push_back(new thread(&CZY_ThreadPool:: run,this));
    }
    return true;
}

bool CZY_ThreadPool::get(TaskFuncPtr& task){
    std::unique_lock<std::mutex> lock(mutex_);

    //���������Զ��������ͷ���
    if(tasks_.empty()){
        //��condition������д��wait��������Ա���
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
                //��ʱ���񣬴�ӡ��־
            }
            else{
                //func�����û������ִ�к���
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
