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
        { AMAB::test, {}, "test command" }
    },
};

//--------//
// HandleGimmieAQuote
//
// Selects a random database row and returns the result to discord.
//
// param[in]   lEvent   The instance of the discord bot.
// param[in]   lEvent   An event from the slash command.
//--------//
//
void test(dpp::cluster * lDiscordBot, const dpp::slashcommand_t * lEvent)
{
    lEvent->reply("This is a test");
}

};