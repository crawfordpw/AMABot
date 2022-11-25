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
// TextReplyToUserInput
//
// This is a generalized function for every model that has a command for replying
// to some user sent input. This will update the event reply in the text channel
// this command was sent from. It should be called from one of the Slash command
// function pointers in gSlashCommands.
// lModel is the "model" string in the config json.
//--------//
//
void TextReplyToUserInput(dpp::cluster * lDiscordBot, const dpp::slashcommand_t * lEvent, Json & lJson, std::string lModel)
{
    // Send a ping to the server. If no response, no need to go further.
    std::string lUrl  = std::string(lJson["models"][lModel]["url"]) + std::string(lJson["models"][lModel]["endpoints"]["ping"]);
    if (AMAB::PingServer(lUrl) != AMAB::HTTP_OK)
    {
        std::string lName = std::string(lJson["models"][lModel]["name"]);
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
    lTask->mJson        = &lJson["models"][lModel];
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

//--------//
// test
//
// .
//--------//
//
void test(dpp::cluster * lDiscordBot, const dpp::slashcommand_t * lEvent, Json & lJson)
{
    TextReplyToUserInput(lDiscordBot, lEvent, lJson, "test");
}


};