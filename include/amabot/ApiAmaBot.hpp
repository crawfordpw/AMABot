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
#include <dpp/nlohmann/json.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <AmaBotConfig.h>
#include "Threadpool.hpp"

#ifdef USE_LOGGER
#include "Logger.hpp"
#endif

typedef nlohmann::json Json;

namespace AMAB
{

// Typedefs
typedef std::function<void(dpp::cluster *, const dpp::slashcommand_t *, Json &)> SlashCommandFunction;

// Forward Declarations
dpp::cluster * CreateBot(Json & lJson);
void test(dpp::cluster * lDiscordBot, const dpp::slashcommand_t * lEvent, Json & lJson);
void testfunc(ThreadTask * lTask, void * lMessage);
void testcallback(ThreadTask * lTask, void * lMessage);

//========//
// SlashCommand
//
// The definition of every slash command for the bot.
//========//

struct SlashCommand
{
    SlashCommandFunction             mFunction;
    const char *                     mDescription;
    std::vector<dpp::command_option> mParameters = {};
};

// Externs
extern const std::map<std::string, SlashCommand> gSlashCommands;

};

#endif
