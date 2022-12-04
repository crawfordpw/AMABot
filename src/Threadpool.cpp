/////////////////////////////////////////////////////////////////////
//
// Threadpool.cpp
//
// Implementation file for the ThreadPool
//
/////////////////////////////////////////////////////////////////////

#include <cstring>
#include <amabot/ThreadPool.hpp>
#include <amabot/Logger.hpp>
#include <AmaBotConfig.h>

namespace AMAB
{

//========//
//
// ThreadTask
//
//========//

//--------//
// ThreadTask
//
// Constructor.
//--------//
//
ThreadTask::ThreadTask()
    : mFunction(nullptr), mCallback(nullptr), mMessage(nullptr), mClient(nullptr)
{
}

//--------//
// ThreadTask
//
// Constructor.
// This will make a complete copy of the message since
// it's going to be handed off to another thread, and the original
// message might've been deleted by the time the thread gets to it.
//
// param[in]    lMessage    The message to pass off to the main function.
// param[in]    lSize       Size of the message. 
//--------//
//
ThreadTask::ThreadTask(void * lMessage, int lSize)
    : ThreadTask()
{
    if (lMessage)
    {
        char * lBuffer = new char[lSize];
        mMessage = reinterpret_cast<void *>(lBuffer);
        memcpy(mMessage, lMessage, lSize);
    }
}

//--------//
// ~ThreadTask
//
// Destructor.
// This will delete the memory for the message.  
//--------//
//
ThreadTask::~ThreadTask()
{
    if (mMessage)
    {
        delete [] reinterpret_cast<char *>(mMessage);
    }
}

//========//
//
// ThreadPool
//
//========//

//--------//
// Init
//
// Initializes the thread pool. THIS SHOULD ONLY BE CALLED ONCE.
//
// param[in]    lNumThreads    Number of threads for the pool to create and manage.
// param[in]    lResourcelimit The max limit of resources this pool will service. Default
//                             value is 0, or "infinite". See header for this variable
//                             for more information.                      
//--------//
//
void ThreadPool::Init(int lNumThreads, int lResourceLimit)
{
    mNumThreads = lNumThreads;
    SetResourceLimit(lResourceLimit);
    Start();
#ifdef USE_LOGGER
    AMAB::Logger * lLogger = AMAB::Logger::GetInstance();
    lLogger->Log("Starting Thread Pool with " + std::to_string(mNumThreads) + " threads");
    lLogger->Log("Setting resource limit to " + std::to_string(lResourceLimit));
#endif
}

//--------//
// ~ThreadPool
//
// Destructor.
// Stop all threads and cleanup any pending tasks from the queue.  
//--------//
//
ThreadPool::~ThreadPool(void)
{
    Stop();

    ThreadTask * lTask;
    while (!mTasks.empty())
    {
        lTask = mTasks.front();
        mTasks.pop();
        delete lTask;
    }
}

//--------//
// SetResourceLimit
//
// If our resource limit is 0 or less, that indicates an "infinite" limit. Meaning
// that we always want to return true for the NotAtlimit call. Otherwise, we set
// our limit and assign a lambda to NotAtlimit, indicating that if our queue size
// has reached our limit, we'll return false.
//
// param[in]    lResourcelimit The max limit of resources this pool will service.              
//--------//
//
void ThreadPool::SetResourceLimit(int lResourceLimit)
{
    if (lResourceLimit <= 0)
    {
        mResourceLimit.store(0);
        mNotAtLimit = []() { return true; };
        return;
    }
    else if (lResourceLimit <= mNumThreads)
    {
        mResourceLimit.store(mNumThreads);
    }
    else
    {
        mResourceLimit.store(lResourceLimit);
    }
    mNotAtLimit = [this]()
    {
        int          lSize;
        std::mutex * lMutex = this->GetMutex();

        lMutex->lock();
        lSize = mTasks.size();
        lMutex->unlock();
        return (lSize >= this->GetResourceLimit()) ? false : true;
    };
}

//--------//
// Start
//
// Creates and starts all the threads in the pool and have
// them wait for some work to do.
//--------//
//
void ThreadPool::Start(void)
{
    mThreads.resize(mNumThreads);
    for (int lIndex = 0; lIndex < mNumThreads; lIndex++)
    {
        mThreads.at(lIndex) = std::thread(&ThreadPool::ThreadLoop, this, lIndex);
    }
}

//--------//
// Stop
//
// Notifies and calls join on all threads.
//--------//
//
void ThreadPool::Stop(void)
{
    mStopThreads.store(true);
    mMutexCondition.notify_all();

    for (auto & lThread : mThreads)
    {
        lThread.join();
    }
    mThreads.clear();
}

//--------//
// ThreadLoop
//
// Adds a task to the queue in a thread-safe manner.
// Once a task has been added, the pool owns the memory for it.
//
// returns      Boolean value of whether the task has been added (true)
//              succesffully or not (false).
//--------//
//
bool ThreadPool::AddTask(ThreadTask * lTask)
{
    if (lTask->mFunction == nullptr || lTask->mClient == nullptr)
    {
        return false;
    }

    bool lCanAdd = mNotAtLimit();

    if(lCanAdd)
    {
        mMutex.lock();
        mTasks.push(lTask);
        mMutex.unlock();
        mMutexCondition.notify_one();
    }
    return lCanAdd;
}

//--------//
// ThreadLoop
//
// This is where all the threads will retrieve and perform any
// tasks that need to be executed. If there is a task, a thread will
// acquire the ThreadTask and pop it off the queue. Then it will call
// the function to actually do the work and free up the memory once
// the work is compelete.
//--------//
//
void ThreadPool::ThreadLoop(int lIndex)
{
    ThreadTask * lTask;
    dpp::utility::set_thread_name("AMAB::TPool" + std::to_string(lIndex));

    while (true)
    {
        {
            std::unique_lock<std::mutex> lLock(mMutex);
            mMutexCondition.wait(lLock, [this]() { return !mTasks.empty() || mStopThreads; });
            if (mStopThreads.load())
            {
                return;
            }
            lTask = mTasks.front();
            mTasks.pop();
        }
        lTask->mFunction(lTask, nullptr);
        delete lTask;
    }
}

};
