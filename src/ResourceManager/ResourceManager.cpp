#include "QuestiaEng/ResourceManager/ResourceManager.h"

ResourceManager::ResourceManager()
{
	textureMap.clear();
}

sf::Texture& ResourceManager::getTexture(const std::string& filename)
{
	if(textureMap.count(filename))
	{
		return textureMap.at(filename);
	}

	sf::Texture tex;
	if(tex.loadFromFile(filename))
	{
		textureMap.emplace(filename, tex);
		std::cout<< "DEBUG: Texture '" << filename << "' was loaded"<< std::endl;
		return textureMap.at(filename);
	}

	std::cout<< "DEBUG: Texture '" << filename << "' was not able to be loaded"<< std::endl;
	textureMap.emplace(filename, tex);
	return textureMap.at(filename);
}

sf::Texture& ResourceManager::getBlankTexture(const std::string& textureName)
{
	if(textureMap.count(textureName))
	{
		return textureMap.at(textureName);
	}

	textureMap.emplace(textureName, sf::Texture());
	std::cout<< "DEBUG: Blank Texture '" << textureName << "' was created"<< std::endl;
	return textureMap.at(textureName);
}

void ResourceManager::kill(const std::string& filename)
{
	if(textureMap.count(filename))
	{
		textureMap.erase(filename);
	}
}

bool ResourceManager::isTexture(const std::string& textureName)
{
	return textureMap.count(textureName);
}
