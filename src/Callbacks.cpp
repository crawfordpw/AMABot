/////////////////////////////////////////////////////////////////////
//
// Callbacks.cpp
//
// This file contains all the callbacks for each slash command..
//
/////////////////////////////////////////////////////////////////////

#include <amabot/ApiAmaBot.hpp>
#include <unistd.h>

namespace AMAB
{

void UserInputError(ThreadTask * lTask, int lErrorCode);

//--------//
// SendUserInput
//
// This is a function that can be called by a task to send a user
// input string to a rest server so it can be processed. This will
// wait for a response back and execute the callback function for
// further processing of the response.
//--------//
//
void SendUserInput(ThreadTask * lTask, void * lMessage)
{
    Json lJson          = (*lTask->mJson);
    char * lTaskMessage = reinterpret_cast<char *>(lTask->mMessage);

    // Build up our url and request json to send to the server.
    std::string lUrl = std::string(lJson["url"]) + std::string(lJson["endpoints"]["send_input"]["path"]);
    for (auto & lElement : lJson["endpoints"]["send_input"]["path_params"])
    {
        lUrl += std::string(lElement);
    }
    std::string lUserInput = "{\"" + std::string(lJson["endpoints"]["send_input"]["req_body"]) + "\": \"" + std::string(lTaskMessage) + "\"}";

    // Tried using the discord library's built-in http request function, but it seems to use the same thread
    // every time the request endpoint is the same. Doesn't make a whole lof of sense to me as it essentially
    // serializes the output, defeating the whole purpose of the architecture and design goals of this bot...
    // Not possible to decouple that from the dpp library, so using curlpp instead.
    int                     lStatus;
    curlpp::Easy            lRequest;
    std::ostringstream      lResponse;
    std::list<std::string>  lHeader     = { "Content-Type: application/json", "accept: application/json" };

#ifdef USE_LOGGER
    AMAB::Logger * lLogger = AMAB::Logger::GetInstance();
    //lRequest.setOpt(curlpp::options::Verbose(true));
    lLogger->Log("Json url: " + lUrl);
    lLogger->Log("Json body: " + lUserInput);
#endif

    try
    {
        lRequest.setOpt(curlpp::options::Url(lUrl));
        lRequest.setOpt(new curlpp::options::HttpHeader(lHeader));
        lRequest.setOpt(new curlpp::options::PostFields(lUserInput));
        lRequest.setOpt(new curlpp::options::PostFieldSize(lUserInput.length()));
        lRequest.setOpt(new curlpp::options::WriteStream(&lResponse));
        lRequest.perform();

        lStatus = curlpp::infos::ResponseCode::get(lRequest);
    }
    catch (curlpp::RuntimeError & lError)
    {
#ifdef USE_LOGGER
        lLogger->Log(lError.what());
#endif
        lStatus = AMAB::HTTP_SERVICE_UNAVAILABLE;
    }

    if (lStatus == AMAB::HTTP_OK)
    {
        lTask->mCallback(lTask, const_cast<char *>(lResponse.str().c_str()));
    }
    else
    {
        UserInputError(lTask, lStatus);
    }
}

//--------//
// ReplyUserInput
//
// A generic callback function to update the discord bot's "thinking" state
// with a response message from the server when a user sends a message.
//--------//
//
void ReplyUserInput(ThreadTask * lTask, void * lMessage)
{
    Json         lJson   = Json::parse(reinterpret_cast<char *>(lMessage));
    std::string  lReply  = lJson[std::string((*lTask->mJson)["endpoints"]["send_input"]["res_body"])];
    dpp::message lMsg(lTask->mChannelId, lReply, dpp::message_type::mt_application_command);

    lMsg.set_guild_id(lTask->mGuildId);
    lTask->mClient->creator->interaction_response_edit(lTask->mToken, lMsg);

#ifdef USE_LOGGER
    AMAB::Logger * lLogger = AMAB::Logger::GetInstance();
    lLogger->Log("Reply msg: " + lReply);
#endif
}

//--------//
// UserInputError
//
// A generic callback function to update the discord bot's "thinking" state
// with an error message if the server could not fulfill the request for
// some reason.
//--------//
//
void UserInputError(ThreadTask * lTask, int lErrorCode)
{
    std::string lReply = "Error " + std::to_string(lErrorCode) + ": ";
    switch(lErrorCode)
    {
        // Intentional fallthrough for now since idk what to do with them!
        case AMAB::HTTP_SERVICE_UNAVAILABLE:
        case AMAB::HTTP_BAD_REQUEST:
        case AMAB::HTTP_UNAUTHORIZED:
        case AMAB::HTTP_NOT_FOUND:
        default:
            lReply += "Dear me, " + std::string((*lTask->mJson)["name"]) + " appears to be out to lunch!";
            break;
    }
    dpp::message lMsg(lTask->mChannelId, lReply, dpp::message_type::mt_application_command);
    lMsg.set_guild_id(lTask->mGuildId);
    lTask->mClient->creator->interaction_response_edit(lTask->mToken, lMsg);
}

};