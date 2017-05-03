#include "QuestiaEng/Utl/TextureAtlas.h"

TextureAtlas::TextureAtlas(ResourceManager& _resourceManager):
	resourceManager(&_resourceManager)
{

}

TextureAtlas::~TextureAtlas()
{
	//dtor
}

bool TextureAtlas::addTexture(std::string name, std::string fileLocation)
{
	//deleted after, used for getting texture data
	sf::Texture tempTexture;
	if(!tempTexture.loadFromFile(fileLocation))
	{
		return false;
	}

	///check if it will fit horizontally and vertically
	// check fit in current layer
	if(maxWidthCurrentLayer + (tempTexture.getSize().x + 1) < maxTextureSize
	        //check if it would fit if it were smaller than the max size
	        && (maxHeight * (layers-1)) + (tempTexture.getSize().y + 1) < maxTextureSize
	        //check if it would fit if it were bigger than the max size
	        && ((tempTexture.getSize().y + 1) * layers) < maxTextureSize)
	{
		//add width
		maxWidthCurrentLayer += tempTexture.getSize().x + 1;
		//see if layer width is bigger than earlier ones
		if(maxWidthCurrentLayer > maxWidth)
		{
			maxWidth = maxWidthCurrentLayer;
		}

		//check height
		//+1 for space in between
		if(tempTexture.getSize().y + 1> maxHeight)
		{
			maxHeight = tempTexture.getSize().y + 1;
		}

		//add to vector for saving
		textureList.push_back(std::make_pair(name, &resourceManager->getTexture(fileLocation)));
		textureLocations.push_back(fileLocation);
		return true;
	}
	///check if there is vertical space for new layer
	//check if it would fit if it were bigger than the max size
	else if((maxHeight * (layers)) + (tempTexture.getSize().y + 1) < maxTextureSize
	        //check if it would fit if it were bigger than the max size
	        && ((tempTexture.getSize().y + 1) * (layers + 1)) < maxTextureSize
	        //see if it is not too wide for texture sheet
	        && (tempTexture.getSize().x + 1 < maxTextureSize))
	{
		layers++;
		maxWidthCurrentLayer = tempTexture.getSize().x;

		//check height
		if(tempTexture.getSize().y + 1 > maxHeight)
		{
			maxHeight = tempTexture.getSize().y;
		}

		//add to vector for saving
		textureList.push_back(std::make_pair(name, &resourceManager->getTexture(fileLocation)));
		textureLocations.push_back(fileLocation);
		return true;
	}
	return false;
}

TextureAtlasData TextureAtlas::compileTextures(std::string textureName)
{
	TextureAtlasData textureData(&resourceManager->getBlankTexture(textureName));
	textureData.texture->create(maxWidth, maxHeight);

	//used to combine all textures
	sf::RenderTexture resultTexture;
	resultTexture.create(maxWidth - 1, (maxHeight * layers) - 1);

	//tracks how far on the x axis it transversed
	unsigned int currentWidth = 0;
	//tracks layers
	unsigned int currentLayer = 1;

	//iterate through textures in placed order
	for(unsigned int it = 0; it != textureList.size(); it++)
	{
		//get data
		std::string& name    = textureList[it].first;
		sf::Texture* texture = textureList[it].second;

		//check if new layer
		if(currentWidth + texture->getSize().x + 1 > maxWidth)
		{
			currentWidth = 0;
			currentLayer++;
		}

		//storing dimensions of new texture
		utl::IntRect dimensions = utl::IntRect(currentWidth, (currentLayer - 1) * maxHeight, texture->getSize().x, texture->getSize().y);
		textureData.textureCoords[name] = dimensions;
		//+1 for space in between
		currentWidth += texture->getSize().x + 1;

		//create sprite from texture
		sf::Sprite sprite;
		sprite.setTexture(*texture);
		sprite.setPosition(dimensions.x, dimensions.y);

		//draw sprite onto texture
		resultTexture.draw(sprite);
	}
	//finalize texture
	resultTexture.display();

	//set texture
	*textureData.texture = resultTexture.getTexture();

	//delete unneeded textures
	for(std::string& it : textureLocations)
	{
		resourceManager->kill(it);
	}

	return textureData;
}
