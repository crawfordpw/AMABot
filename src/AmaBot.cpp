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
// PingServer
//
// Pings an http endpoint at the given url.
//
// param[in]   lUrl     The url to ping.
// returns  HTTP status code.
//--------//
//
int PingServer(std::string & lUrl)
{
    curlpp::Easy lRequest;
    try
    {
        lRequest.setOpt(curlpp::options::Url(lUrl));
        lRequest.setOpt(new curlpp::options::CustomRequest("GET"));
        lRequest.setOpt(new curlpp::options::WriteFunction(DoNothing));
        lRequest.setOpt(new curlpp::options::Timeout(2));
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

};
