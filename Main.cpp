//////////////////////////////////////////////////////////////////////////////////////////
//
// Main.cpp
//
// Entry point for the bot.
//
//////////////////////////////////////////////////////////////////////////////////////////

#include <amabot/ApiAmaBot.hpp>

volatile bool gRunForever = true;
volatile bool gSignalNum = EXIT_SUCCESS;

void signal_callback_handler(int lSignalNum) {
#ifdef USE_LOGGER
    AMAB::Logger::GetInstance()->Log("Caught signal " + lSignalNum);
#endif
    gRunForever = false;
    gSignalNum = lSignalNum;
}

int main(int argc, char const * argv[])
{
    signal(SIGINT, signal_callback_handler);

#ifdef USE_LOGGER
    AMAB::Logger * lLogger = AMAB::Logger::GetInstance();
    lLogger->Log("Logger has been enabled");
#endif

    AMAB::Json  lJsonConfig;
    std::string lConfigLocation = "../config.json";

    if (argc == 2)
    {
        lConfigLocation = argv[1];
    }

	std::ifstream lFile(lConfigLocation);
    if (lFile.fail())
    {
#ifdef USE_LOGGER
        lLogger->Log("Could not find configuration file. Exiting...");
#endif
        return EXIT_FAILURE;
    }

	lJsonConfig = AMAB::Json::parse(lFile);

    // Create and start our bot.
    dpp::cluster * lDiscordBot = AMAB::CreateBot(lJsonConfig["token"]);
    lDiscordBot->start(dpp::st_return);

    // Stay in this infinite loop until CTRL-C happens, then clean up.
    while (gRunForever) {}

    delete lDiscordBot;

    return gSignalNum;
}
