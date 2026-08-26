#pragma once
#include <exception>

namespace STILO
{
    class InvalidParameterException : public std::exception
    {
    public:
    
        InvalidParameterException(char* msg);

        const char* what();

    private:

        const char* errmsg_;    
    };
}