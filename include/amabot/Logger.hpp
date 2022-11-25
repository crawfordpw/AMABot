/////////////////////////////////////////////////////////////////////
//
// Logger.hpp
//
// Simple logger that prints to stdout.
//
/////////////////////////////////////////////////////////////////////

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>

namespace AMAB
{

//========//
// Logger
//
// Generic logger
//========//

class Logger
{
    private:

        Logger() {};

    public:

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        static Logger * GetInstance() { static Logger cInstance; return &cInstance; }

        void Log(std::string lMessage) { std::cout << "--> " << lMessage << "\n"; };
};

};

#endif
