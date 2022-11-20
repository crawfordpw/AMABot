/////////////////////////////////////////////////////////////////////
//
// Threadpool.cpp
//
// Implementation file for the ThreadPool
//
/////////////////////////////////////////////////////////////////////

#include <cstring>
#include <amabot/ThreadPool.hpp>

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
    : mFunction(nullptr), mCallback(nullptr), mMessage(nullptr)
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
{
    char * lBuffer = new char[lSize];
    mMessage =  reinterpret_cast<void *>(lBuffer);
    memcpy(mMessage, lMessage, lSize);
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
// ThreadPool
//
// Constructor.
//
// param[in]    lNumThreads    Number of threads for the pool to create and manage.
// param[in]    lResourcelimit The max limit of resources this pool will service. Default
//                             value is 0, or "infinite". See header for this variable
//                             for more information.                      
//--------//
//
ThreadPool::ThreadPool(int lNumThreads, int lResourceLimit)
    : mNumThreads(lNumThreads), mStopThreads(false)
{
    SetResourceLimit(lResourceLimit);
    Start();
}

//--------//
// ~ThreadPool
//
// Destructor.
// This will delete the memory for the message.  
//--------//
//
ThreadPool::~ThreadPool(void)
{
    Stop();

    while (!mTasks.empty())
    {
        // This pop should be calling our destructor which would free any memory.
        mTasks.pop();
    }
}

//--------//
// SetResourceLimit
//
// If our resource limit is 0 or less, that indicates an "infinite" limit. Meaning
// that we always want to return false for the Atlimit call. Otherwise, we set
// our limit and assign a lambda to Atlimit indication that if our queue size
// has reached our limit, we'll return true.
//
// param[in]    lResourcelimit The max limit of resources this pool will service.              
//--------//
//
void ThreadPool::SetResourceLimit(int lResourceLimit)
{
    if (lResourceLimit <= 0)
    {
        mResourceLimit.store(0);
        mAtLimit = []() { return false; };
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
    mAtLimit = [this]()
    {
        int          lSize;
        std::mutex * lMutex = this->GetMutex();

        lMutex->lock();
        lSize = mTasks.size();
        lMutex->unlock();
        return (lSize >= this->GetResourceLimit()) ? true : false;
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
        mThreads.at(lIndex) = std::thread(&ThreadPool::ThreadLoop, this);
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

void ThreadPool::ThreadLoop(void)
{
    while (true)
    {
        
    }
}


};
