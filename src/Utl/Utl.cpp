#include "QuestiaEng/Utl/Utl.h"

///types
//comparison

bool utl::isNumber(const std::string& _string)
{
    return (_string.find_first_not_of("0123456789.-") == std::string::npos);
}

bool utl::isWithAnyCharacter(const std::string& _string, const std::string& characters)
{
    return (_string.find_first_of(characters) != std::string::npos);
}

//conversion

int utl::asInt(const std::string& _string)
{
    return atoi(_string.c_str());
}

float utl::asFloat(const std::string& _string)
{
    return atoi(_string.c_str());
}

template std::string utl::asString <bool>           (const bool& _item);
template std::string utl::asString <char>           (const char& _item);
template std::string utl::asString <int>            (const int& _item);
template std::string utl::asString <unsigned int>   (const unsigned int& _item);
template std::string utl::asString <unsigned short> (const unsigned short& _item);
template std::string utl::asString <float>          (const float& _item);
template std::string utl::asString <std::string>    (const std::string& _item);
template <class T>
std::string utl::asString (const T& _item)
{
    std::stringstream ss;
    ss << _item;
    std::string item = ss.str();
    return item;
}

//combination

std::vector <std::string> utl::separateString (const std::string& saveString, const char& delimiter)
{
    std::stringstream sStream;
    std::vector <std::string> returnVector;
    sStream << saveString;
    while (sStream.good())
    {
        std::string entry;
        std::getline(sStream, entry, delimiter);
        returnVector.push_back(entry);
    }
    return returnVector;
}

std::vector <std::string> utl::separateString (const std::string& saveString, const std::string& delimiters)
{
    std::vector<std::string> parts;
//    boost::split(parts, saveString, boost::is_any_of(delimiters));
    return parts;
}

std::string utl::conjoinString (const std::vector <std::string>& stringParts, const char& delimiter)
{
    std::stringstream sStream;
    for(unsigned int it = 0; it != stringParts.size(); it++)
    {
        sStream << stringParts[it];
        if(it != stringParts.size()-1)
        {
            sStream << delimiter;
        }
    }
    std::string conjoinedString = sStream.str();
    return conjoinedString;
}

std::string utl::conjoinString (const std::vector <std::string>& stringParts)
{
    std::stringstream sStream;
    for(unsigned int it = 0; it != stringParts.size(); it++)
    {
        sStream << stringParts[it];
    }
    std::string conjoinedString = sStream.str();
    return conjoinedString;
}
