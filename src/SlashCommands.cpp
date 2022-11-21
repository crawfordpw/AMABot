/////////////////////////////////////////////////////////////////////
//
// SlashCommands.cpp
//
// Provides the implementation details for every slash command.
//
/////////////////////////////////////////////////////////////////////

#include <amabot/ApiAmaBot.hpp>

namespace AMAB
{

// A global map of slash commands. Every command, SlashCommand, is mapped to some name.
const std::map<std::string, AMAB::SlashCommand> gSlashCommands = 
{
    {
        "test", 
        { 
            AMAB::test, "test command", 
                { { dpp::command_option(dpp::co_string, "message", "Write your message here", true)} }
        }
    },
};

//--------//
// test
//
// .
//
// param[in]   lEvent   The instance of the discord bot.
// param[in]   lEvent   An event from the slash command.
//--------//
//
void test(dpp::cluster * lDiscordBot, const dpp::slashcommand_t * lEvent)
{
    ThreadTask * lTask;
    ThreadPool * lThreadPool = AMAB::ThreadPool::GetInstance();
    std::string  lParameter  = std::get<std::string>(lEvent->get_parameter("message"));

#ifdef USE_LOGGER
    AMAB::Logger::GetInstance()->Log("Parameter is: " + lParameter);
#endif

    lTask = new ThreadTask(const_cast<char *>(lParameter.c_str()), lParameter.length());
    lTask->mBot = lDiscordBot;
    lTask->mFunction = AMAB::testfunc;
    lTask->mCallback = AMAB::testcallback;

    lThreadPool->AddTask(lTask);

    lEvent->reply("This is a test");
}

};