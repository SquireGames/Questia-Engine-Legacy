#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Obj.h"

Entity_Obj::Entity_Obj(unsigned int id, EntityManager& entityManager, ResourceManager& resourceManager, utl::Vector2f coords):
    Entity(id, entityManager)
	, coords(coords)
    , resourceManager(resourceManager)
{
		
}

Entity_Obj::~Entity_Obj()
{
    //dtor
}

void Entity_Obj::draw(sf::RenderWindow& window, DrawLayer drawLayer)
{

}

int Entity_Obj::getGroundOffset()
{
	return 16;
}
