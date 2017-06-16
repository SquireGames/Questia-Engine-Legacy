#include "QuestiaEng/EntityManager/Entity/Entity.h"

Entity::Entity(unsigned int id, EntityManager& entityManager):
    entityManager(entityManager)
	, id(id)
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

void Entity::useUpdate(Entity* e)
{
	entityManager.entities_Base.push_back(e);
	inEntity_Base = true;
}

void Entity::useObj(Entity_Obj* e)
{
	entityManager.entities_Obj.push_back(e);
	inEntity_Obj = true;
}

void Entity::useColl(Entity_Coll* e)
{
	entityManager.entities_Coll.push_back(e);
	inEntity_Coll = true;
}

void Entity::useLiving(Entity_Living* e)
{
	entityManager.entities_Living.push_back(e);
	inEntity_Living = true;
}

void Entity::usePlayer(Entity_Player* e)
{
	entityManager.entities_Player.push_back(e);
	inEntity_Player = true;
}
