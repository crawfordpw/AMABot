/////////////////////////////////////////////////////////////////////
//
// Callbacks.cpp
//
// This file contains all the callbacks for each slash command..
//
/////////////////////////////////////////////////////////////////////

#include <math.h>
#include <ctype.h>
#include <amabot/ApiAmaBot.hpp>

namespace AMAB
{

// Simple counting identifier for large messages that
// exceed discords message char limit. Used to help
// identify messages if it's broken up by intermediate
// messages not belonging to the same reply.
static std::atomic<uint32_t> gMessageId = 1;

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
    std::string lUserInput = "{\"" + std::string(lJson["endpoints"]["send_input"]["req_body"]) + "\": \"" + std::string(lTaskMessage) + "\"}";

    // Tried using the discord library's built-in http request function, but it seems to use the same thread
    // every time the request endpoint is the same. Doesn't make a whole lof of sense to me as it essentially
    // serializes the output, defeating the whole purpose of the architecture and design goals of this bot...
    // Not possible to decouple that from the dpp library, so using curlpp instead.
    int                     lStatus;
    curlpp::Easy            lRequest;
    std::ostringstream      lResponse;
    std::list<std::string>  lHeader     = { "Content-Type: application/json", "accept: application/json" };

    try
    {
        lRequest.setOpt(curlpp::options::Url(lUrl));
        lRequest.setOpt(new curlpp::options::HttpHeader(lHeader));
        lRequest.setOpt(new curlpp::options::PostFields(lUserInput));
        lRequest.setOpt(new curlpp::options::PostFieldSize(lUserInput.length()));
        lRequest.setOpt(new curlpp::options::WriteStream(&lResponse));
        lRequest.setOpt(new curlpp::options::Timeout(AMAB::REPLY_TIMEOUT));
        lRequest.perform();

        lStatus = curlpp::infos::ResponseCode::get(lRequest);
    }
    catch (curlpp::RuntimeError & lError)
    {
#ifdef USE_LOGGER
        AMAB::Logger * lLogger = AMAB::Logger::GetInstance();
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
    Json         lJson        = Json::parse(reinterpret_cast<char *>(lMessage));
    std::string  lReply       = lJson[std::string((*lTask->mJson)["endpoints"]["send_input"]["res_body"])];
    int          lReplyLength = lReply.length();

    // Reply message can fit within Discord's character limit.
    if (lReplyLength <= AMAB::DISCORD_CHAR_LIMIT)
    {
        dpp::message lMsg(lTask->mChannelId, lReply, dpp::message_type::mt_application_command);
        lMsg.set_guild_id(lTask->mGuildId);
        lTask->mClient->creator->interaction_response_edit(lTask->mToken, lMsg);
        return;
    }

    // Reply message needs to be broken up into smaller reply messages (chunks).

    int         lStartIndex  = 0;   // Index into the reply message to start each chunk at.
    int         lChunkLength = 0;   // Length of the message chunk, which could be different from MSG_CHUNK_SIZE.
    std::string lMessageId   = std::to_string(gMessageId.fetch_add(1));
    std::vector<std::string> lChunks;

    // Let the user know we are breaking up the message.
    std::string lThinkingString = std::string((*lTask->mJson)["name"]) + " has a rather lengthy reply. Breaking it up into smaller segments to satisfy Discord. Assigning message id: ";
    dpp::message lMsg(lTask->mChannelId, lThinkingString + lMessageId, dpp::message_type::mt_application_command);
    lMsg.set_guild_id(lTask->mGuildId);
    lTask->mClient->creator->interaction_response_edit(lTask->mToken, lMsg);

    // Here we break up the reply from the AI model into chunks that discord can handle.
    // We find the last word within the chunk size to break the chunk at for user convenience,
    // and add the entire chunk into a vector. Once we find all chunks, we send out the discord
    // messages by attaching a message id to them. This could be done within the first while loop
    // without needing a vector, but I wanted to attach messages numbers to each message,
    // and the number of chunks could potentionally grow since the beginning of the loop if some
    // stars aligned.
    while (lStartIndex < lReplyLength)
    {
        // Find the last word within the chunk size.
        lChunkLength = AMAB::MSG_CHUNK_SIZE;
        while (lChunkLength > 0)
        {
            if (isspace(lReply[lStartIndex + lChunkLength]))
            {
                break;
            }
            lChunkLength--;
        }

        // If ChunkLength is 0, that means the entire chunk has no spaces for
        // some reason, so set the length to the default chunk size.
        if (lChunkLength <= 0)
        {
            lChunkLength = AMAB::MSG_CHUNK_SIZE;
        }
        
        lChunks.push_back(lReply.substr(lStartIndex, ++lChunkLength));
        lStartIndex += lChunkLength;
    }

    // Now send all the chunks as discord messages.
    int lNumChunks = lChunks.size();
    std::string lIdString    = "Id: " + lMessageId;
    std::string lChunkString = "/" + std::to_string(lNumChunks) + ")\n";

    for (int i = 0; i < lNumChunks; i++)
    {
        std::string lString = lIdString + " (" + std::to_string(i+1) + lChunkString + lChunks.at(i);
        dpp::message lMsg(lTask->mChannelId, lString, dpp::message_type::mt_application_command);
        lMsg.set_guild_id(lTask->mGuildId);
        lTask->mClient->creator->message_create(lMsg);
    }
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