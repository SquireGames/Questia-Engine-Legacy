#include "QuestiaEng/Utl/SpriteSheet.h"

SpriteSheet::SpriteSheet(ResourceManager& _resourceManager, std::string imageLocation, unsigned int slides_x, unsigned int slides_y):
    res(_resourceManager)
    , spriteMap()
{
    loadSprite(imageLocation, slides_x, slides_y);
}

void SpriteSheet::loadSprite(std::string imageLocation, unsigned int slides_x, unsigned int slides_y)
{
    //spriteSheet texture
    sf::Texture* texture = nullptr;

    //see if sheet already exists
    if(res.isTexture(imageLocation))
    {
        texture = &res.getTexture(imageLocation);

        const sf::Vector2<unsigned int>& textureSize = texture->getSize();

        size_x = (textureSize.x - (slides_x - 1)) / slides_x;
        size_y = (textureSize.y - (slides_y - 1)) / slides_y;
    }
    else
    {
        //loading texture
        texture = &res.getTexture(imageLocation);

        //get sheet dimensions
        const sf::Vector2<unsigned int>& textureSize = texture->getSize();
        size_x = (textureSize.x - (slides_x - 1)) / slides_x;
        size_y = (textureSize.y - (slides_y - 1)) / slides_y;

        //only image can modify pixels
        sf::Image tempImage = texture->copyToImage();

        //get rid of vertical lines
        for(unsigned int it_x = 0; it_x != (slides_x - 1); it_x++)
        {
            for(unsigned int it_y = 0; it_y != textureSize.y; it_y++)
            {
                tempImage.setPixel(it_x * (size_x + 1) + size_x, it_y, sf::Color(0,0,0,0));
            }
        }
        //get rid of horizontal lines
        for(unsigned int it_y = 0; it_y != (slides_y - 1); it_y++)
        {
            for(unsigned int it_x = 0; it_x != textureSize.x; it_x++)
            {
                tempImage.setPixel(it_x, it_y * (size_y + 1) + size_y, sf::Color(0,0,0,0));
            }
        }
        //convert back to texture
        texture->update(tempImage);
    }

    //getting the sprites
    for(unsigned int it_1 = 0; it_1 < slides_x; it_1++)
    {
        for(unsigned int it_2 = 0; it_2 < slides_y; it_2++)
        {
            sf::Sprite sheet;
            sheet.setTexture(res.getTexture(imageLocation));
            sheet.setTextureRect(sf::IntRect((it_1 * size_x) + it_1,
                                             (it_2 * size_y) + it_2,
                                             size_x,
                                             size_y));
            spriteMap[it_1] [it_2] = sheet;
        }
    }
}


SpriteSheet::SpriteSheet(ResourceManager& _resourceManager):
    res(_resourceManager)
    , spriteMap()
{

}

void SpriteSheet::setSize(float width, float height)
{
    for(auto& it1 : spriteMap)
    {
        for(auto& it2 : it1.second)
        {
            it2.second.setScale(width/size_x, height/size_y);
        }
    }
}

void SpriteSheet::setOrigin(float x, float y)
{
    for(auto& it1 : spriteMap)
    {
        for(auto& it2 : it1.second)
        {
            it2.second.setOrigin(x,y);
        }
    }
}

sf::Sprite& SpriteSheet::getSprite(unsigned int map_x, unsigned int map_y)
{
    return spriteMap[map_x][map_y];
}

unsigned int SpriteSheet::getSheetWidth()
{
    return size_x;
}
unsigned int SpriteSheet::getSheetHeight()
{
    return size_y;
}
