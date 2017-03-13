#include "QuestiaEng/Utl/Type/Rect.h"

utl::Direction utl::IntRect::getCollision(const IntRect& other)
{
    return utl::Direction::none;
}
bool utl::IntRect::isColliding(const IntRect& other)
{
    if(x < other.x + other.width &&
            x + width > other.x &&
            y < other.y + other.height &&
            y + height > other.y)
    {
        return true;
    }
    return false;
}
utl::Vector2i utl::IntRect::distanceTo(const IntRect& other, utl::Direction& direction)
{
    //TODO implement distanceTo
    return utl::Vector2i(0,0);
}



utl::Direction utl::FloatRect::getCollision(const FloatRect& other)
{
    return utl::Direction::none;
}
bool utl::FloatRect::isColliding(const FloatRect& other)
{
    if(x < other.x + other.width &&
            x + width > other.x &&
            y < other.y + other.height &&
            y + height > other.y)
    {
        return true;
    }
    return false;
}
utl::Vector2f utl::FloatRect::distanceTo(const FloatRect& other, utl::Direction& direction)
{
    //TODO implement distanceTo
    return utl::Vector2f(0,0);
}
