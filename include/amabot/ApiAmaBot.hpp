/////////////////////////////////////////////////////////////////////
//
// ApiAmaBot.hpp
//
// The public API of the discord bot.
//
/////////////////////////////////////////////////////////////////////

#ifndef AMA_BOT_HPP
#define AMA_BOT_HPP

#include <map>
#include <dpp/dpp.h>
#include <AmaBotConfig.h>

#ifdef USE_LOGGER
#include "Logger.hpp"
#endif

namespace AMAB
{

// Typedefs
typedef std::function<void(dpp::cluster *, const dpp::slashcommand_t *)> SlashCommandFunction;

// Forward Declarations
dpp::cluster * CreateBot(std::string lBotToken);
void test(dpp::cluster * lDiscordBot, const dpp::slashcommand_t * lEvent);

//========//
// SlashCommand
//
// The definition of every slash command for the bot.
//========//

struct SlashCommand
{
    SlashCommandFunction             mFunction;
    std::vector<dpp::command_option> mParameters = {};
    const char *                     mDescription;
};

// Externs
extern const std::map<std::string, SlashCommand> gSlashCommands;

};

#endif
