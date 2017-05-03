#include "QuestiaEng/EntityManager/Entity/Entity.h"

Entity::Entity(unsigned int _id, EntityManager& _entityManager):
    entityManager(_entityManager)
	, id(_id)
{
    //ctor
}

Entity::~Entity()
{
    //dtor
}

void Entity::update()
{

}

unsigned int Entity::getID()
{
    return id;
}
