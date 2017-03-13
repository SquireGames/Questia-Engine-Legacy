#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Coll/Entity_Coll.h"

Entity_Coll::Entity_Coll(unsigned int id, EntityManager& entityManager, ResourceManager& resourceManager, Bounds _collBounds, Bounds _hitBounds, utl::Vector2f origin):
    Entity_Obj(id, entityManager, resourceManager, origin)
    , collBounds(_collBounds)
    , hitBounds(_hitBounds)
{
    //ctor
}

Entity_Coll::~Entity_Coll()
{
    //dtor
}

void Entity_Coll::attemptMove(Entity_Coll& entity, const utl::Vector2f velocity)
{
    entityManager.attemptMove(entity, velocity);
}

void Entity_Coll::forceMove(Entity_Coll& entity, const utl::Vector2f velocity)
{
    entity.coords += velocity;
}

void Entity_Coll::onCollision(Entity_Coll& other)
{
    onCollision(other.atrs, other.getID());
}

void Entity_Coll::onCollision(std::map <Type, Type_Field>& atrs, const unsigned int& entityID)
{

}

