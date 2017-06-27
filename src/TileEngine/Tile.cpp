#include "QuestiaEng/TileEngine/Tile.h"

Tile::Tile(sf::RenderWindow& window, ResourceManager& resourceManager):
	window(window)
	, resourceManager(resourceManager)
{

}

void Tile::setTexture(const std::string& filePath)
{
	//TODO add multiple nested folder support
	std::vector<std::string> tileSourceDir = utl::separateString(filePath, '/');
	texture = filePath;
	display = (filePath.size() > 28) ? filePath.substr(23) : filePath;

	if(tileSourceDir.size() >= 6)
	{
		folder   = tileSourceDir[4];
		tileName = tileSourceDir[5];
	}
}

void Tile::setSource(const std::string& filePath)
{
	source = filePath;
}

void Tile::setDisplay(const std::string& filePath)
{
	display = (filePath.size() > 28) ? filePath.substr(23) : filePath;
}

void Tile::setSize(unsigned int tilesWidth, unsigned int tilesHeight)
{
	tileSize = utl::Vector2i(tilesWidth, tilesHeight);
}
void Tile::setSize_x(unsigned int tilesWidth)
{
	tileSize.x = tilesWidth;
}
void Tile::setSize_y(unsigned int tilesHeight)
{
	tileSize.y = tilesHeight;
}

void Tile::setRotation(int degrees)
{
	this->degrees = degrees;
}

void Tile::setFlip(char direction)
{
	flip = direction;
}

utl::Vector2i Tile::getSize() const {return tileSize;}
int Tile::getSize_x() const {return tileSize.x;}
int Tile::getSize_y() const {return tileSize.y;}
int Tile::getRotate() const {return degrees;}
char Tile::getFlip() const {return flip;}
const std::string& Tile::getTexturePath() const {return texture;}
const std::string& Tile::getSource() const {return source;}
const std::string& Tile::getDisplay() const {return display;}
const std::string& Tile::getFolder() const {return folder;}
const std::string& Tile::getTileName() const {return tileName;}

void Tile::setCollisionRect(int x, int y, int width, int height)
{
	collisionRect = utl::IntRect(x, y, width, height);
}
utl::IntRect& Tile::getCollisionRect()
{
	return collisionRect;
}

void Tile::loadSprite()
{
	//setTexture()
	tileSprite.setTexture(resourceManager.getTexture(texture));
	tileTransform = tileSprite.getTransform();
	//setSize()
	tileTransform.scale((64.f / tileSprite.getLocalBounds().width) * tileSize.x, (64.f / tileSprite.getLocalBounds().height) * tileSize.y);
	//setRotate()
	sf::Transform rotateTransform = sf::Transform().rotate(degrees, tileSprite.getLocalBounds().width/2, tileSprite.getLocalBounds().height/2);
	tileTransform.combine(rotateTransform);
	//setFlip()
	switch(flip)
	{
	case 'x':
		tileSprite.setTextureRect(sf::IntRect(tileSprite.getLocalBounds().width, 0,
		                                      -1 * tileSprite.getLocalBounds().width, tileSprite.getLocalBounds().height));
		break;
	case 'y':
		tileSprite.setTextureRect(sf::IntRect(0, tileSprite.getLocalBounds().height,
		                                      tileSprite.getLocalBounds().width, -1 * tileSprite.getLocalBounds().height));
		break;
	case 'b':
		tileSprite.setTextureRect(sf::IntRect(tileSprite.getLocalBounds().width, tileSprite.getLocalBounds().height,
		                                      -1 * tileSprite.getLocalBounds().width, -1 * tileSprite.getLocalBounds().height));
		break;
	default:
		break;
	}

}
void Tile::setPosition(int x, int y)
{
	moveTransform = sf::Transform().translate(x * 64, y * 64);
	tilePosition = utl::Vector2i(x, y);
}
void Tile::setTransparency(int alpha)
{
	sf::Color newColor = tileSprite.getColor();
	float trans = static_cast <float>(alpha);
	trans = trans * 255 / 100;
	newColor.a = trans;
	tileSprite.setColor(newColor);
}
void Tile::drawTile()
{
	window.draw(tileSprite, moveTransform.combine(tileTransform));
}

bool Tile::isInTile(int x, int y)
{
	if((x >= tilePosition.x && x <= tilePosition.x + (tileSize.x - 1))&&
	        (y >= tilePosition.y && y <= tilePosition.y + (tileSize.y - 1)))
	{
		return true;
	}
	return false;
}
