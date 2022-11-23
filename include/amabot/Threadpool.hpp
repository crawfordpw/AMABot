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
#include <dpp/dpp.h>

typedef nlohmann::json Json;

namespace AMAB
{

//========//
// ThreadTask
//
// This is the structure that each thread in the pool 
// will operate on to fulfill a command request. This includes
// everything that a task may need to reply back to discord
// with a message. Since this is happening in a separate thread from
// where we received the message event, some of that information may
// already by gone by the time we actually service the request. Therefore,
// this attempts to copy the relevant informatin in order to properly
// reply back when complete.
//========//

class ThreadTask
{
    typedef std::function<void(ThreadTask *, void *)> ThreadCallback;

    public:

        ThreadTask();
        ThreadTask(void * lMessage, int lSize);
        ~ThreadTask();

        ThreadCallback        mFunction;    // The main work for the task to perform.
        ThreadCallback        mCallback;    // A callback once the main work is complete.
        void *                mMessage;     // Message for main function to operate on.
        dpp::discord_client * mClient;      // Discord  client requesting the work. 
        dpp::snowflake        mChannelId;   // Channel id this request came from.
        dpp::snowflake        mGuildId;     // Guild id this request came from.
        std::string           mToken;       // Interaction token for editing a response once
                                            // the bot is in a "thinking" state.
        Json *                mJson;
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
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

        static ThreadPool * GetInstance() { static ThreadPool cInstance; return &cInstance; }

        void Init(int lNumThreads, int lResourceLimit = 0);
        ~ThreadPool(void);

        bool             AddTask(ThreadTask * lTask);
        void             SetResourceLimit(int lResourceLimit);
        int              GetResourceLimit() { return mResourceLimit.load(); }
        std::mutex *     GetMutex()         { return &mMutex; }

    private:

        ThreadPool() : mStopThreads(false) {}

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
