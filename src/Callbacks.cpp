/////////////////////////////////////////////////////////////////////
//
// Callbacks.cpp
//
// This file contains all the cllback for each slash command..
//
/////////////////////////////////////////////////////////////////////

#include <amabot/ApiAmaBot.hpp>
#include <unistd.h>

namespace AMAB
{

void testfunc(ThreadTask * lTask, void * lMessage)
{
    char * lTaskMessage = reinterpret_cast<char *>(lTask->mMessage);
#ifdef USE_LOGGER
    AMAB::Logger * lLogger = AMAB::Logger::GetInstance();
    lLogger->Log("Made it to testfunc!");
    lLogger->Log(lTaskMessage);
#endif

    const char * lNewMessage = "This is a message from testfunc!";
    lTask->mCallback(lTask, const_cast<char *>(lNewMessage));
}

void testcallback(ThreadTask * lTask, void * lMessage)
{
#ifdef USE_LOGGER
    AMAB::Logger * lLogger = AMAB::Logger::GetInstance();
    lLogger->Log("Made it to testcallback!");
    lLogger->Log(reinterpret_cast<char *>(lMessage));
#endif
    std::string lStr(reinterpret_cast<char *>(lMessage));
    dpp::message lMsg(lTask->mChannelId, lStr, dpp::message_type::mt_application_command);
    lMsg.set_guild_id(lTask->mGuildId);
    sleep(5);

    //lTask->mBot->message_create(lMsg);
    lTask->mClient->creator->interaction_response_edit(lTask->mToken, lMsg);
}

};