#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Coll/Entity_Coll.h"

Entity_Coll::Entity_Coll(unsigned int id, EntityManager& entityManager, ResourceManager& resourceManager, utl::Vector2f coords):
    Entity_Obj(id, entityManager, resourceManager, coords)
{
    //ctor
}

Entity_Coll::~Entity_Coll()
{
    //dtor
}

void Entity_Coll::attemptMove(utl::Vector2f velocity)
{
    entityManager.attemptMove(*this, velocity);
}

void Entity_Coll::forceMove(utl::Vector2f velocity)
{
    this->coords += velocity;
}

void Entity_Coll::onCollision(Entity_Coll& other)
{
    onCollision(other.properties, other.getID());
}

void Entity_Coll::onCollision(const std::map <pKey, pValue>& properties, unsigned int entityID)
{

}

const Bounds* Entity_Coll::getCollBounds()
{
	return nullptr;
}
const Bounds* Entity_Coll::getHitBounds()
{
	return nullptr;
}

