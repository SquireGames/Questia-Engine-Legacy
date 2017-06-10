#include "QuestiaEng/Utl/SaveFile.h"

SaveFile::SaveFile(std::string fileName, bool isUnicode):
	fileName(fileName)
	, isUnicode(isUnicode)

{
	sStream.str(std::string());

#ifdef DEBUGMODE
	setFilePath(fileName);
#endif

}

void SaveFile::setFilePath(const std::string& fileName)
{
	this->fileName = fileName;
#ifdef DEBUGMODE
	fileStream.open(fileName, std::ios::in); // Open to read
	if(fileStream.is_open())
	{
		std::cout << "SAVE: Save file " << fileName << " opened successfully" << std::endl;
	}
	else
	{
		LOG("File with name: '" + fileName + "' failed to open");
	}
	fileStream.close();
#endif
}



SaveFile::SaveFile():
	fileName(std::string())
{
}

bool SaveFile::readFile()
{
	if(isUnicode)
	{
		return readFile_uni();
	}
	return readFile(':');;
}

bool SaveFile::readFile(char separator)
{
	if(isUnicode)
	{
		return readFile_uni(separator);
	}

	fileStream.open(fileName, std::ios::in); // Open to read

	if(fileStream.is_open())
	{
		saveList.clear();
		std::string line;
		while(std::getline(fileStream,line))
		{
			std::map <int, std::string> lineSeperator;
			int iter = 0;
			sStream << line;
			while(sStream.good())
			{
				std::string entry;
				std::getline(sStream, entry, separator);

				char firstCharacter = ' ';
				if(entry.length() > 0)
				{
					firstCharacter = entry.at(0);
				}

				if(firstCharacter != '/' || firstCharacter != '(')
				{
					if(firstCharacter != ' ')
					{
						lineSeperator[iter] = entry;
						iter++;
					}
				}
				else
				{
					break;
				}
			}
			lineSeperator[0].erase(std::remove(lineSeperator[0].begin(), lineSeperator[0].end(), '\r'), lineSeperator[0].end());
			lineSeperator[0].erase(std::remove(lineSeperator[0].begin(), lineSeperator[0].end(), '\n'), lineSeperator[0].end());
			lineSeperator[1].erase(std::remove(lineSeperator[1].begin(), lineSeperator[1].end(), '\r'), lineSeperator[1].end());
			lineSeperator[1].erase(std::remove(lineSeperator[1].begin(), lineSeperator[1].end(), '\n'), lineSeperator[1].end());
			saveList.push_back(std::make_pair(lineSeperator[0], lineSeperator[1]));
			sStream.str(std::string());
			sStream.clear();
		}
	}
	else //file does not exist
	{
		fileStream.close();
		return false;
	}

	fileStream.close();
	return true;
}

void SaveFile::writeFile()
{
	fileStream.open(fileName, std::ios::out | std::ios::trunc); // deletes and writes

	if(fileStream.is_open())
	{
		for(unsigned int it = 0; it != saveList.size(); it++)
		{
			fileStream << saveList[it].first << ":" << saveList[it].second;

			//if(commentList.count(saveList[it].first))
			//{
			//	fileStream << commentList[saveList[it].first];
			//}

			if(it != saveList.size()-1)
			{
				fileStream << std::endl;
			}
		}
	}
	fileStream.close();
}

void SaveFile::clearSave()
{
	fileStream.open(fileName, std::ios::out | std::ios::trunc); // deletes and writes
	fileStream.close();
	saveList.clear();
	saveList_uni.clear();
}
std::string SaveFile::getItem(const std::string& itemName)
{
	for(unsigned int it = 0; it != saveList.size(); it++)
	{
		if(saveList[it].first == itemName)
		{
			return saveList[it].second;
		}
	}
	return std::string("nil");
}

std::vector<std::string> SaveFile::getItemKeyList()
{
	std::vector<std::string> itemList;
	for(unsigned int it = 0; it != saveList.size(); it++)
	{
		itemList.push_back(saveList[it].first);
	}
	return itemList;
}

std::vector<std::u32string> SaveFile::getItemKeyList_uni()
{
	std::vector<std::u32string > itemList;
	for(unsigned int it = 0; it != saveList_uni.size(); it++)
	{
		itemList.push_back(saveList_uni[it].first);
	}
	return itemList;
}

std::u32string SaveFile::getItem_uni(const std::u32string& itemName)
{
	for(unsigned int it = 0; it != saveList_uni.size(); it++)
	{
		if(saveList_uni[it].first == itemName)
		{
			return saveList_uni[it].second;
		}
	}
	return U"nil";
}

bool SaveFile::readFile_uni()
{
	return readFile_uni(':');
}

bool SaveFile::readFile_uni(char separator)
{
	//local filestream and sStream gets referenced rather than class ones
	std::basic_ifstream<char32_t> fileStream;
	std::basic_stringstream<char32_t> sStream;

	fileStream.open(fileName, std::ios::binary);

	if(fileStream.is_open())
	{
		saveList_uni.clear();

		fileStream.imbue(std::locale(fileStream.getloc(), new std::codecvt_utf8<char32_t, 0x10ffff, std::consume_header>));

		std::vector<std::u32string> lines;
		//get all lines from file
		{
			//read file into string
			std::u32string bufStr;
			for(char32_t c; fileStream.get(c);)
			{
				bufStr += c;
			}
			if(bufStr.size() < 0)
			{
				fileStream.close();
				return false;
			}
			
			//separate string into lines
			std::u32string line;
			for(char32_t c : bufStr)
			{
				if(c != U'\r' && c != U'\n' && c != U'\0')
				{
					line += c;
				}
				else
				{
					lines.push_back(line);
					line.clear();
				}
			}
			lines.push_back(line);
		}
		//separate lines into delimiters
		for(const std::u32string& line : lines)
		{
			if(line.size() && line.at(0) == U'/')
			{
				continue;
			}
			
			std::u32string first, rest;
			bool reachedDelimiter = false;
			for(char32_t c : line)
			{
				if(!reachedDelimiter && c == (char32_t)separator)
				{
					reachedDelimiter = true;
					continue;
				}
				std::u32string& str = reachedDelimiter ? rest : first;
				str += c;
			}
			saveList_uni.push_back(std::make_pair(first, rest));
		}
	}
	else
	{
		fileStream.close();
		return false;
	}

	fileStream.close();
	return true;
}
