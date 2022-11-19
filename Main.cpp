//////////////////////////////////////////////////////////////////////////////////////////
//
// Main.cpp
//
// Entry point for the bot.
//
//////////////////////////////////////////////////////////////////////////////////////////

#include <amabot/ApiAmaBot.hpp>

volatile bool gRunForever = true;
volatile bool gSignalNum = 0;

void signal_callback_handler(int lSignalNum) {
#ifdef USE_LOGGER
    QMB::Logger::GetInstance()->Log("Caught signal " + lSignalNum);
#endif
    gRunForever = false;
    gSignalNum = lSignalNum;
}

int main(int argc, char const * argv[])
{
    std::string lBotToken = "";

#ifdef USE_LOGGER
   QMB::Logger::GetInstance()->Log("Logger has been enabled");
#endif

    signal(SIGINT, signal_callback_handler);

    dpp::cluster * lDiscordBot = AMAB::CreateBot(lBotToken, lGuildId, lDataBaseFile);

    lDiscordBot->start(dpp::st_return);

    // Stay in this infinite loop until CTRL-C happens, then clean up.
    while (gRunForever) {}

    delete lDiscordBot;

    return gSignalNum;
}
