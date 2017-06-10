#include "QuestiaEng/Utl/Utl.h"

bool utl::isNumber(const std::string& string)
{
    return (string.find_first_not_of("0123456789.-") == std::string::npos);
}

bool utl::isWithAnyCharacter(const std::string& string, const std::string& characters)
{
    return (string.find_first_of(characters) != std::string::npos);
}

int utl::toInt(const std::string& string)
{
    return atoi(string.c_str());
}

float utl::toFloat(const std::string& string)
{
    return atoi(string.c_str());
}

std::u32string utl::toU32(const std::string& string)
{
	std::u32string returnStr;
	for(char c : string)
	{
		returnStr += (char32_t)c;
	}
	return returnStr;
}

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
