//////////////////////////////////////////////////////////////////////////////////////////
//
// Main.cpp
//
// Entry point for the bot.
//
//////////////////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <amabot/ApiAmaBot.hpp>

std::condition_variable gMainCondition;
volatile bool gSignalNum  = EXIT_SUCCESS;

//--------//
// SignalCallbackHandler
//
// Catches signals we are interested in so we can signal
// the main event loop to termiante.
//
// param[in]    lSignalNum    Signal that was caught.
//--------//
//
void SignalCallbackHandler(int lSignalNum)
{
    gSignalNum = lSignalNum;
    gMainCondition.notify_one();
}

//--------//
// main
//
// Entry point of the program.
//--------//
//
int main(int argc, char const * argv[])
{
    std::mutex lMainMutex;
    signal(SIGINT, SignalCallbackHandler);
    signal(SIGTERM, SignalCallbackHandler);

#ifdef USE_LOGGER
    AMAB::Logger * lLogger = AMAB::Logger::GetInstance();
    lLogger->Log("Logger has been enabled");
#endif

    Json        lJsonConfig;
    std::string lConfigLocation = "../config.json";
    int         lPoolThreads    = std::thread::hardware_concurrency();
    int         lResources      = 0;

    // Get the new json config location if supplied.
    if (argc == 2)
    {
        lConfigLocation = argv[1];
    }

    // Grab the json file and parse.
    std::ifstream lFile(lConfigLocation);
    if (lFile.fail())
    {
#ifdef USE_LOGGER
        lLogger->Log("Could not find configuration file. Exiting...");
#endif
        return EXIT_FAILURE;
    }
    lJsonConfig = Json::parse(lFile);

    // Grab number of threads for the thread pool.
    if (lJsonConfig["num_pool_threads"] != nullptr)
    {
        lPoolThreads = lJsonConfig["num_pool_threads"];
    }
    // Grab number of threads for the thread pool.
    if (lJsonConfig["max_num_msgs"] != nullptr)
    {
        lResources = lJsonConfig["max_num_msgs"];
    }

    // Initalize curlpp
    curlpp::initialize(CURL_GLOBAL_ALL);

    // Create thread pool for executing the bot's commands.
    AMAB::ThreadPool * lThreadPool = AMAB::ThreadPool::GetInstance();
    lThreadPool->Init(lPoolThreads, lResources);

    // Create and start our bot.
    dpp::cluster * lDiscordBot = AMAB::CreateBot(lJsonConfig);
    lDiscordBot->start(dpp::st_return);

    // Wait for SIGINT or SIGTERM signal to unblock and terminate gracefully.
    std::unique_lock lLock(lMainMutex);
    gMainCondition.wait(lLock);

#ifdef USE_LOGGER
    lLogger->Log("Caught signal " + std::to_string(gSignalNum) + ". Terminating program... ");
#endif

    delete lDiscordBot;
    curlpp::terminate();

    return gSignalNum;
}
