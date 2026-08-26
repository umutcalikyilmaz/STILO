#pragma once
#include <string>

namespace STILO
{
    inline std::string parseNext(const std::string& str, int& pos, int size)
    {
        int st = pos;

        while(pos < size)
        {
            if(str.at(pos) == ',')
            {                
                return str.substr(st, pos++ - st);
            }

            pos++;
        }

        return "";
    }

    inline std::string parseLast(const std::string& str, int& pos, int size)
    {
        if(pos >= size)
        {
            return "";
        }

        return str.substr(pos, size - pos);
    }
}