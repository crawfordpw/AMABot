/////////////////////////////////////////////////////////////////////
//
// Callbacks.cpp
//
// This file contains all the cllback for each slash command..
//
/////////////////////////////////////////////////////////////////////

#include <amabot/ApiAmaBot.hpp>

namespace AMAB
{


void testfunc(ThreadTask * lTask, void * lMessage)
{
#ifdef USE_LOGGER
    AMAB::Logger::GetInstance()->Log("Made it to testfunc!");
#endif
    lTask->mCallback(lTask, nullptr);
}

void testcallback(ThreadTask * lTask, void * lMessage)
{
#ifdef USE_LOGGER
    AMAB::Logger::GetInstance()->Log("Made it to testcallback!");
#endif
}

};