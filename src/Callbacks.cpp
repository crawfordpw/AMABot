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

//--------//
// test
//
// .
//
// param[in]   lEvent   The instance of the discord bot.
// param[in]   lEvent   An event from the slash command.
//--------//
//
void testfunc(ThreadTask * lTask, void * lMessage)
{
    Json lJson = (*lTask->mJson);
    char * lTaskMessage = reinterpret_cast<char *>(lTask->mMessage);

    std::string lUrl = std::string(lJson["url"]) + std::string(lJson["endpoints"]["send_input"]["path"]);
    for (auto & lElement : lJson["endpoints"]["send_input"]["path_params"])
    {
        lUrl += std::string(lElement);
    }
    std::string lMyData = "{\"" + std::string(lJson["endpoints"]["send_input"]["req_body"]) + "\": \"" + std::string(lTaskMessage) + "\"}";
#ifdef USE_LOGGER
    AMAB::Logger * lLogger = AMAB::Logger::GetInstance();
    lLogger->Log("Json url: " + lUrl);
    lLogger->Log("Json body: " + lMyData);
#endif

    // Tried using the discord library's built-in http request function, but it seems to use the same thread
    // everytime the request endpoint is the same. Doesn't make a whole lof of sense to me as it essentially
    // serializes the output, defeating the whole purpose of the architecture and design goals of this bot...
    // Not possible to decouple that from the dpp library, so using curlpp instead.
    curlpp::Easy lRequest;
    lRequest.setOpt(curlpp::options::Url(lUrl));
    std::list<std::string> lHeader = 
    {
        "Content-Type: application/json",
        "accept: application/json"
    };
    std::ostringstream lResponse;
    lRequest.setOpt(new curlpp::options::HttpHeader(lHeader));
    lRequest.setOpt(new curlpp::options::PostFields(lMyData));
    lRequest.setOpt(new curlpp::options::PostFieldSize(lMyData.length()));
    lRequest.setOpt(new curlpp::options::WriteStream(&lResponse));
    lRequest.perform();

    lTask->mCallback(lTask, const_cast<char *>(lResponse.str().c_str()));
}

//--------//
// test
//
// .
//
// param[in]   lEvent   The instance of the discord bot.
// param[in]   lEvent   An event from the slash command.
//--------//
//
void testcallback(ThreadTask * lTask, void * lMessage)
{
    Json lJson = Json::parse(reinterpret_cast<char *>(lMessage));

    std::string lStr(lJson["reply"]);
#ifdef USE_LOGGER
    AMAB::Logger * lLogger = AMAB::Logger::GetInstance();
    lLogger->Log("Made it to testcallback!");
    lLogger->Log("Reply msg: " + lStr);
#endif

    dpp::message lMsg(lTask->mChannelId, lStr, dpp::message_type::mt_application_command);
    lMsg.set_guild_id(lTask->mGuildId);
    sleep(5);

    //lTask->mBot->message_create(lMsg);
    lTask->mClient->creator->interaction_response_edit(lTask->mToken, lMsg);
}

};