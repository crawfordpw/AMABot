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
//--------//
//
void test(dpp::cluster * lDiscordBot, const dpp::slashcommand_t * lEvent, Json & lJson)
{
    // Send a ping to the server. If no response, no need to go further.
    std::string lName = std::string(lJson["models"]["test"]["name"]);
    std::string lUrl  = std::string(lJson["models"][lName]["url"]) + std::string(lJson["models"][lName]["endpoints"]["ping"]);
    if (AMAB::PingServer(lUrl) != AMAB::HTTP_OK)
    {
        lEvent->reply(lName + " could not be reached! They may not be home.");
        return;
    }

    // Looks like the server is there, continue creating our thread task.
    ThreadTask * lTask;
    ThreadPool * lThreadPool = AMAB::ThreadPool::GetInstance();
    std::string  lParameter  = std::get<std::string>(lEvent->get_parameter("message"));

    // Plus 1 to include the null terminator!
    lTask = new ThreadTask(const_cast<char *>(lParameter.c_str()), lParameter.length() + 1);
    lTask->mClient      = lEvent->from;
    lTask->mChannelId   = lEvent->command.channel_id;
    lTask->mGuildId     = lEvent->command.guild_id;
    lTask->mToken       = lEvent->command.token;
    lTask->mJson        = &lJson["models"][lName];
    lTask->mFunction    = AMAB::SendUserInput;
    lTask->mCallback    = AMAB::ReplyUserInput;

    if (lThreadPool->AddTask(lTask))
    {
        lEvent->thinking();
    }
    else
    {
        lEvent->reply("Oh, dear. Looks like the mail is backed up. Please try again at a later time.");
        delete lTask;
    }
}

};