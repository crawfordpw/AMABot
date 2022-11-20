/////////////////////////////////////////////////////////////////////
//
// Threadpool.hpp
//
// Simple threadpool for executing tasks of the bot.
//
/////////////////////////////////////////////////////////////////////

#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

namespace AMAB
{

// Typedefs
typedef std::function<void(void * )>                 ThreadCallback;
typedef std::function<void(void *, ThreadCallback )> ThreadFunction;

//========//
// ThreadTask
//
// This is the structure that each thread in the pool 
// will operate on.
//========//

class ThreadTask
{
    public:

        ThreadTask();
        ThreadTask(void * lMessage, int lSize);
        ~ThreadTask();

        ThreadFunction mFunction;   // The main work for the task to perform.
        ThreadCallback mCallback;   // A callback once the main work is complete.
        void *         mMessage;    // Parameter to the main callback.
};

//========//
// ThreadPool
//
// Pool of threads designed to work with ThreadTasks to perform
// some work.
//========//

class ThreadPool
{
    public:

        ThreadPool(int lNumThreads, int lNumResources = 0);
        ~ThreadPool(void);

        bool             AddTask(ThreadTask * lTask);
        void             SetResourceLimit(int lResourceLimit);
        int              GetResourceLimit() { return mResourceLimit.load(); }
        std::mutex *     GetMutex()         { return &mMutex; }

    private:

        void ThreadLoop(void);
        void Start(void);
        void Stop(void);
        
        int                      mNumThreads;
        std::atomic<int>         mResourceLimit;
        std::atomic<bool>        mStopThreads;      // Flag indication to stop all threads.
        std::mutex               mMutex;            // Mutex for queue manipulation.
        std::condition_variable  mMutexCondition;   // Condition to allow threads to wait without hogging resources.
        std::vector<std::thread> mThreads;          // Our pool of threads.
        std::queue<ThreadTask *> mTasks;            // Tasks for the threads to perform.
        std::function<bool()>    mNotAtLimit;       // Function pointer to check if we reached the resource limit.
};

//
// @ mResourceLimit
//          This indicates how many resources are allowed to be occupied by the pool
//          while servicing tasks. It essentially sets the upper limit of the queue.
//          0 indicates "infinite", meaning you can put as many tasks on the queue
//          as there is available memory. Having a resource limit < the num of threads
//          doesn't make much sense, so the minimum value will be set to mNumThreads if
//          set to a value 0 > mResourceLimit < mNumthreads.
//          Note: Theres probably a better way to handle this notion, but doing it more
//                as an exercise to learn rather than optimization.

};

#endif
