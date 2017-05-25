#include "QuestiaEng/ResourceManager/ResourceManager.h"

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
	textureMap.clear();
}

sf::Texture& ResourceManager::getTexture(std::string filename)
{
	if(firstTime)
	{
		firstTime = false;
		textureMap.clear();
	}
	if(textureMap.count(filename))
	{
		return textureMap[filename];
	}

	sf::Texture tex;
	if(tex.loadFromFile(filename))
	{
		textureMap.insert(std::make_pair(filename, tex));
		std::cout<< "DEBUG: Texture '" << filename << "' was loaded"<< std::endl;
		return textureMap.at(filename);
	}

	// if not loaded
	std::cout<< "DEBUG: Texture '" << filename << "' was not found"<< std::endl;
	textureMap[filename] = tex;
	return textureMap.at(filename);
}

sf::Texture& ResourceManager::getBlankTexture(std::string textureName)
{
	if(firstTime)
	{
		firstTime = false;
		textureMap.clear();
	}
	for(auto it = textureMap.begin(); it != textureMap.end(); it++)
	{
		if(textureName == it->first)
		{
			return it->second;
		}
	}
	// if not loaded
	textureMap.insert(std::make_pair(textureName, sf::Texture()));
	std::cout<< "DEBUG: Blank Texture '" << textureName << "' was created"<< std::endl;
	return textureMap[textureName];
}

void ResourceManager::kill(std::string filename)
{
	if(textureMap.count(filename))
	{
		textureMap.erase(filename);
	}
}

bool ResourceManager::isTexture(std::string textureName)
{
	for(auto it = textureMap.begin(); it != textureMap.end(); it++)
	{
		if(textureName == it->first)
		{
			return true;
		}
	}
	return false;
}
