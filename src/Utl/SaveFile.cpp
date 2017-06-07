#include "QuestiaEng/Utl/SaveFile.h"

SaveFile::SaveFile(std::string fileName):
	fileName(fileName)
{
	sStream.str(std::string());

	fileStream.open(fileName, std::ios::in); // Open to read

	if(fileStream.is_open())
	{
		std::cout << "SAVE: Save file " << fileName << " opened successfully" << std::endl;
	}
	fileStream.close();
}
SaveFile::SaveFile():
	fileName(std::string())
{
}

bool SaveFile::readFile()
{
	fileStream.open(fileName, std::ios::in);

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
				std::getline(sStream, entry, ':');

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
	else
	{
		fileStream.close();
		return false;
	}

	fileStream.close();
	return true;
}

bool SaveFile::readFile(char separator)
{
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

			if(commentList.count(saveList[it].first))
			{
				fileStream << commentList[saveList[it].first];
			}

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

std::vector<std::string> SaveFile::getItemList()
{
	std::vector<std::string> itemList;
	for(unsigned int it = 0; it != saveList.size(); it++)
	{
		saveList[it].first.erase(std::remove(saveList[it].first.begin(), saveList[it].first.end(), '\r'), saveList[it].first.end());
		saveList[it].first.erase(std::remove(saveList[it].first.begin(), saveList[it].first.end(), '\n'), saveList[it].first.end());
		itemList.push_back(saveList[it].first);
	}
	return itemList;
}
