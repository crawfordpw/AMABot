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

};
