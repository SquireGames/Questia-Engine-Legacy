#include "QuestiaEng/Utl/FileData.h"

namespace fs = std::experimental::filesystem::v1;

bool utl::doesExist(const std::string& fileLocation)
{
	return fs::exists(fileLocation);
}

std::vector<std::string> utl::getFiles(std::string directory, bool isWithDirectory)
{
	std::vector <std::string> fileNames;

	fs::path directoryFolder(directory);
	if(fs::exists(directoryFolder) && fs::is_directory(directoryFolder))
	{
		for(auto& x: fs::directory_iterator(directoryFolder))
		{
			std::stringstream ss;
			std::string st;
			if(isWithDirectory)
			{
				ss<<x.path().string();
			}
			else
			{
				ss<<x.path().filename().string();
			}
			ss>>st;
			std::replace(st.begin(), st.end(),'\\', '/');
			fileNames.push_back(st);
		}
	}

	return fileNames;
}

std::vector<std::string> utl::getAllFiles(std::string directory, bool isWithDirectory)
{
	std::vector <std::string> fileNames;

	fs::path directoryFolder {directory};

	if(fs::exists(directoryFolder) && fs::is_directory(directoryFolder))
	{
		fs::recursive_directory_iterator dir(directoryFolder);

		for(auto& it : dir)
		{
			fileNames.push_back(it.path().string());
			if(fs::is_directory(fs::path(it)))
			{
				std::vector<std::string> files = getAllFiles(it.path().string(), isWithDirectory);
				for(auto& it_r : files)
				{
					fileNames.push_back(it_r);
				}
			}
		}
	}
	return fileNames;
}



bool utl::createDirectory(const std::string& filePath)
{
	fs::path directory(filePath);

	if(!(fs::exists(directory)))
	{
		if(fs::create_directory(directory))
		{
			std::cout << "Directory was made: " << filePath << std::endl;
			return true;
		}
		else
		{
			std::cout << "Directory failed to be made: " << filePath << std::endl;
			return false;
		}
	}
	return true;
}

std::vector<std::string> utl::filterFiles(const std::vector<std::string>& inputFiles, const std::string& extension)
{
	std::vector<std::string> newFiles = std::vector<std::string>();

	for(const std::string& file : inputFiles)
	{
		if(file.find(extension)!= std::string::npos)
		{
			newFiles.push_back(file);
		}
	}

	return newFiles;
}

std::vector<std::string> utl::filterDirs(const std::vector<std::string>& inputFiles)
{
	std::vector<std::string> returnStrs;
	for(const std::string& file : inputFiles)
	{
		std::size_t found = file.find(".");
		if(found == std::string::npos)
		{
			returnStrs.push_back(file);
		}
	}
	return returnStrs;
}
