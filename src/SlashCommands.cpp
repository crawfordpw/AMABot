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
        "whoisup", 
        { 
            AMAB::RunningModels, "Check which AI models are currently running. Returns a list of \"Model Name (command)\"", {}
        }
    },
    {
        "test", 
        { 
            AMAB::test, "test command", 
                { { dpp::command_option(dpp::co_string, "message", "Write your message here", true) } }
        }
    },
};

// All unique running endpoints from the Json config file. Preprocessed for RunningModels function.
std::vector<std::string> gRunningEndpoints;

//--------//
// RunningModels
//
// Does a GET call to to each server to query which models are currently running and returns the
// result back to the user.
//--------//
//
void RunningModels(dpp::cluster * lDiscordBot, const dpp::slashcommand_t * lEvent, Json & lJson)
{
    lEvent->thinking();

    std::string              lReplyMsg;
    std::vector<std::string> lModels;

    // Send a request to each server and push the reply back to a vector
    // to be processed further down.
    for (auto & lUrl : AMAB::gRunningEndpoints)
    {
        std::ostringstream lResponse;
        if (AMAB::EndpointGET(lUrl, &lResponse, 5) == AMAB::HTTP_OK)
        {
            std::string lReply = Json::parse(lResponse.str())["reply"];
            if (!lReply.empty())
            {
                lModels.push_back(lReply);
            }
        }
    }

    // Build the reply message back to discord based on what we received
    // from the REST servers.
    for (auto & lModel : lModels)
    {
        std::vector<std::string> lWords = AMAB::SplitString(lModel);
        for (auto & lWord : lWords)
        {
            if (gSlashCommands.find(lWord) != AMAB::gSlashCommands.end())
            {
                lReplyMsg += std::string(lJson["models"][lWord]["name"]) + " (" + lWord + "), ";
            }
        }
    }

    // If there were no models running, create a unique message. Otherwise, remove the
    // last two characters from the string to make it look nicer.
    if (lReplyMsg.empty())
    {
        lReplyMsg = "Looks like there's no models currently running.";
    }
    else
    {
        lReplyMsg.erase(lReplyMsg.length() - 2);
    }

    // Update the discord message "thinking" state with our message.
    dpp::message lMsg(lEvent->command.channel_id, lReplyMsg, dpp::message_type::mt_application_command);
    lMsg.set_guild_id(lEvent->command.guild_id);
    lDiscordBot->interaction_response_edit(lEvent->command.token, lMsg);
}

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
void TextReplyToUserInput(dpp::cluster * lDiscordBot, const dpp::slashcommand_t * lEvent, Json & lJson)
{
    const std::string & lModel  = lEvent->command.get_command_interaction().name;

    // Send a ping to the server. If no response, no need to go further.
    std::string lUrl = std::string(lJson["models"][lModel]["url"]) + std::string(lJson["models"][lModel]["endpoints"]["ping"]);
    if (AMAB::EndpointGET(lUrl, nullptr, 2) != AMAB::HTTP_OK)
    {
        std::string lName = std::string(lJson["models"][lModel]["name"]);
        lEvent->reply(lName + " could not be reached! They may not be home or are very busy.");
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
    TextReplyToUserInput(lDiscordBot, lEvent, lJson);
}

};