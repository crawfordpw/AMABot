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
#include <curlpp/Infos.hpp>
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
void SendUserInput(ThreadTask * lTask, void * lMessage);
void ReplyUserInput(ThreadTask * lTask, void * lMessage);

// Generic bot related stuff.
enum
{
    DISCORD_CHAR_LIMIT          = 2000,
    // 10 for the message number and total up to 3 digits each "(XXX/YYY) "
    // 5 for id string "id: "; followed by
    // 10 for max number of digits in unsigned int
    MSG_CHUNK_SIZE              = DISCORD_CHAR_LIMIT - 25,
};

// HTTP related stuff.
enum
{
    REPLY_TIMEOUT               = 120,
    HTTP_OK                     = 200,
    HTTP_BAD_REQUEST            = 400,
    HTTP_UNAUTHORIZED           = 401,
    HTTP_NOT_FOUND              = 404,
    HTTP_SERVICE_UNAVAILABLE    = 503,
};
int PingServer(std::string & lUrl);

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
