#include "QuestiaEng/TileEngine/Tile.h"

Tile::Tile(sf::RenderWindow& _window, ResourceManager& _resourceManager):
    window(_window)
    , resourceManager(_resourceManager)
{

}

void Tile::drawTile()
{
    switch(tileType)
    {
    case TileType::sprite:
        window.draw(tileSprite, moveTransform.combine(tileTransform));
        break;
    case TileType::texture:
        break;
    }
}

void Tile::setTexture(std::string filePath)
{
    switch(tileType)
    {
    case TileType::sprite:
    {
        tileSprite.setTexture(resourceManager.getTexture(filePath));

        tileTransform = tileSprite.getTransform();
        tileSize = utl::Vector2i(tileSprite.getLocalBounds().width / 64, tileSprite.getLocalBounds().height / 64);

        //TODO add multiple nested folder support
        std::vector<std::string> tileSourceDir = utl::separateString(filePath, '/');
        source   = filePath;
        folder   = tileSourceDir[4];
        tileName = tileSourceDir[5];
    }
    break;
    case TileType::texture:
        break;
    }
}
void Tile::setPosition(int x, int y)
{
    switch(tileType)
    {
    case TileType::sprite:
        moveTransform = sf::Transform().translate(x * 64, y * 64);
        tilePosition = utl::Vector2i(x, y);
        break;
    case TileType::texture:
        break;
    }
}
void Tile::setTransparency(int alpha)
{
    switch(tileType)
    {
    case TileType::sprite:
    {
        sf::Color newColor = tileSprite.getColor();
        float trans = static_cast <float> (alpha);
        trans = trans * 255 / 100;
        newColor.a = trans;
        tileSprite.setColor(newColor);
    }
    break;
    case TileType::texture:
        break;
    }
}
void Tile::setSize(unsigned int tilesWidth, unsigned int tilesHeight)
{
    tileSize = utl::Vector2i(tilesWidth, tilesHeight);
    switch(tileType)
    {
    case TileType::sprite:
        tileTransform = tileSprite.getTransform();
        tileTransform.scale((64.f / tileSprite.getLocalBounds().width) * tilesWidth, (64.f / tileSprite.getLocalBounds().height) * tilesHeight);
    case TileType::texture:
        break;
    }
}
void Tile::setRotate(int _degrees)
{
    switch(tileType)
    {
    case TileType::sprite:
    {
        sf::Transform rotateTransform = sf::Transform().rotate(_degrees, tileSprite.getLocalBounds().width/2, tileSprite.getLocalBounds().height/2);
        tileTransform.combine(rotateTransform);
    }
    break;
    case TileType::texture:
        degrees = _degrees;
        break;
    }
}
void Tile::setFlip(char direction)
{
    switch(tileType)
    {
    case TileType::sprite:
    {
        const int& width  = tileSprite.getLocalBounds().width;
        const int& height = tileSprite.getLocalBounds().height;
        switch (direction)
        {
        case 'x':
            tileSprite.setTextureRect(sf::IntRect(width, 0, -width, height));
            break;
        case 'y':
            tileSprite.setTextureRect(sf::IntRect(0, height, width, -height));
            break;
        case 'b':
            tileSprite.setTextureRect(sf::IntRect(width, height, -width, -height));
            break;
        default:
            break;
        }
    }
    break;
    case TileType::texture:
        flip = direction;
        break;
    }
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

void Tile::setCollisionRect(int x, int y, int width, int height)
{
    collisionRect = utl::IntRect(x, y, width, height);
}
utl::IntRect& Tile::getCollisionRect()
{
    return collisionRect;
}
