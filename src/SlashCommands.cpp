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
void test(dpp::cluster * lDiscordBot, const dpp::slashcommand_t * lEvent, Json & lJson)
{
    ThreadTask * lTask;
    ThreadPool * lThreadPool = AMAB::ThreadPool::GetInstance();
    std::string  lParameter  = std::get<std::string>(lEvent->get_parameter("message"));

#ifdef USE_LOGGER
    AMAB::Logger::GetInstance()->Log("Parameter is: " + lParameter);
#endif

    // Plus 1 to include the null terminator!
    lTask = new ThreadTask(const_cast<char *>(lParameter.c_str()), lParameter.length() + 1);
    lTask->mClient = lEvent->from;
    lTask->mChannelId = lEvent->command.channel_id;
    lTask->mGuildId = lEvent->command.guild_id;
    lTask->mToken = lEvent->command.token;
    lTask->mJson = &lJson["models"]["test"];
    lTask->mFunction = AMAB::testfunc;
    lTask->mCallback = AMAB::testcallback;

    if (lThreadPool->AddTask(lTask))
    {
        //lEvent->reply("Your correspondence has been sent successfully. Please hold.");
        lEvent->thinking();
    }
    else
    {
        lEvent->reply("Oh, dear. Looks like the mail is backed up. Please try again at a later time.");
    }
}

};