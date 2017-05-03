#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Coll/Test/Entity_Crate.h"

Entity_Crate::Entity_Crate(unsigned int id, EntityManager& entityManager, ResourceManager& resourceManager, const utl::Vector2f& _coords):
    Entity_Coll(id, entityManager, resourceManager
                , Bounds(Rect(utl::Vector2f(CRATE_SIZE_X,CRATE_SIZE_Y/2), utl::Vector2f(0,0)))
                , Bounds(Rect(utl::Vector2f(CRATE_SIZE_X,CRATE_SIZE_Y), utl::Vector2f(CRATE_SIZE_X/2,CRATE_SIZE_Y/2)))
                , utl::Vector2f(0,CRATE_SIZE_Y/2))
{
    //relative coords
    collBounds.rel_coords = utl::Vector2f(-(CRATE_SIZE_X/2), 0);
    hitBounds.rel_coords  = utl::Vector2f(0, 0);

    //center sprite
    sprite.setTexture(resourceManager.getTexture("Media/Image/Game/Entity/Test/Crate.png"));
    sprite.setOrigin(sprite.getLocalBounds().width/2, sprite.getLocalBounds().height/2);
    sprite.setScale(CRATE_SIZE_X/sprite.getLocalBounds().width, CRATE_SIZE_Y/sprite.getLocalBounds().height);

    //set position
    coords = _coords;
    sprite.setPosition(coords.sf());
}

Entity_Crate::~Entity_Crate()
{
    //dtor
}

void Entity_Crate::draw(sf::RenderWindow& window, const DrawLayer& drawLayer)
{
    window.draw(sprite);
}

void Entity_Crate::onCollision(std::map <Type, Type_Field>& atrs, const unsigned int& entityID)
{
    if(atrs.count(Type::test_attack))
    {
       // if(atrs.at(Type::test_attack).atrs.count((int)Type_test_attack::destroyStuff))
        {
         //   if(boost::get<bool>(atrs.at(Type::test_attack).atrs.at((int)Type_test_attack::destroyStuff)))
            {
                entityManager.queueKill(getID());
            }
        }
    }
}
