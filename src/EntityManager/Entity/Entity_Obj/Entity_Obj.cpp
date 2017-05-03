#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Obj.h"

Entity_Obj::Entity_Obj(unsigned int id, EntityManager& entityManager, ResourceManager& _resourceManager, utl::Vector2f _origin):
    Entity(id, entityManager)
	, origin(_origin)
    , resourceManager(_resourceManager)
{

}

Entity_Obj::~Entity_Obj()
{
    //dtor
}

void Entity_Obj::draw(sf::RenderWindow& window, const DrawLayer& drawLayer)
{

}
