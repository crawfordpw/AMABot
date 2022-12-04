/////////////////////////////////////////////////////////////////////
//
// QuoteMeBot.cpp
//
// Main implementation file for the discord bot.
//
/////////////////////////////////////////////////////////////////////

#include <amabot/ApiAmaBot.hpp>

namespace AMAB
{

//--------//
// CreateBot
//
// Creates the discord bot, registering all commmands and sets up handling
// of all events that its interested in.
//
// param[in]   lBotToken        Bot id token.
// returns     A bot instance. This expects the caller to delete the instance.
//--------//
//
dpp::cluster * CreateBot(Json & lJson)
{
    // Create the discord bot.
    dpp::cluster * lDiscordBot = new dpp::cluster(lJson["token"]);

#ifdef USE_LOGGER
    // Output log messages to stdout.
    lDiscordBot->on_log(dpp::utility::cout_logger());
#endif

    lDiscordBot->on_ready([lDiscordBot](const dpp::ready_t & lEvent)
    {
        // When the bot is first ran, iterate through the entire map of our commands,
        // and register them with the bot. This will translate our map of commands
        // to a vector of DPP commands.
        if (dpp::run_once<struct bulk_register>())
        {
            // Register all slash type commands.
            std::vector<dpp::slashcommand> lSlashCommands;
            for (auto & lCommandIterator : AMAB::gSlashCommands)
            {
                dpp::slashcommand lCommand;
                lCommand.set_name(lCommandIterator.first).
                    set_description(lCommandIterator.second.mDescription).
                    set_application_id(lDiscordBot->me.id);
                lCommand.options = lCommandIterator.second.mParameters;
                lSlashCommands.push_back(lCommand);
            }
            lDiscordBot->global_bulk_command_create(lSlashCommands);
        }
    });

    // This will handle each of our slash commands and dispatch them to the 
    // correct handle function.
    lDiscordBot->on_slashcommand([lDiscordBot, &lJson](const dpp::slashcommand_t & lEvent)
    {
        dpp::command_interaction lCommandData   = lEvent.command.get_command_interaction();
        auto lCommandIterator                   = AMAB::gSlashCommands.find(lCommandData.name);

        if (lCommandIterator != AMAB::gSlashCommands.end())
        {
            lCommandIterator->second.mFunction(lDiscordBot, &lEvent, lJson);
        }
    });

    return lDiscordBot;
}

//--------//
// DoNothing
//
// For whatever reason, curlpp will always print the response for
// GET requests. No idea why that is the default option, but using
// this callback with a WriteFunction option stops that.
//--------//
size_t DoNothing(char* c , size_t s, size_t ss) { return s * ss; }

//--------//
// EndpointGET
//
// Issues a GET call to the specified Url.
//
// param[in]   lUrl         The url for the GET request.
// param[out]  lResponse    The response from the GET call. Pass in nullptr if
//                          the caller doesn't care what the response is.
// param[in]   lTimeout     How long to wait before timing out in seconds.
// returns  HTTP status code.
//--------//
//
int EndpointGET(std::string & lUrl, std::ostringstream * lResponse, int lTimeout)
{
    curlpp::Easy lRequest;
    try
    {
        if (lResponse != nullptr)
        {
            lRequest.setOpt(new curlpp::options::WriteStream(lResponse));
        }
        else
        {
            lRequest.setOpt(new curlpp::options::WriteFunction(DoNothing));
        }

        lRequest.setOpt(curlpp::options::Url(lUrl));
        lRequest.setOpt(new curlpp::options::CustomRequest("GET"));
        lRequest.setOpt(new curlpp::options::Timeout(lTimeout));
        lRequest.perform();
        return curlpp::infos::ResponseCode::get(lRequest);
    }
    catch (curlpp::RuntimeError & lError)
    {
#ifdef USE_LOGGER
        AMAB::Logger::GetInstance()->Log(lError.what());
#endif
        return AMAB::HTTP_SERVICE_UNAVAILABLE;
    }
}

//--------//
// SplitString
//
// Splits a given string by some delimiter into a vector of smaller
// strings if possible.
//
// param[in]   lString       String to split.
// param[in]   lDelimiter    What to split by, defaults to a space.
// returns  Vector of strings.
//--------//
//
std::vector<std::string> SplitString(std::string & lString, std::string lDelimiter)
{
    size_t lStart  = 0;
    size_t lEnd    = lString.find(lDelimiter);
    std::vector<std::string> lWords;

    // There are no delimiters found in the string, so just add it to the vector.
    if (lEnd == std::string::npos)
    {
        lWords.push_back(lString);
    }
    else
    {
        while (lEnd != std::string::npos)
        {
            lWords.push_back(lString.substr(lStart, lEnd - lStart));
            lStart = lEnd + lDelimiter.size();
            lEnd = lString.find(lDelimiter, lStart);
        }
        lWords.push_back(lString.substr(lStart, lEnd - lStart));
    }
    
    return lWords;
}

};
