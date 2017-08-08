#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Coll/Entity_Living/Entity_Living.h"

Entity_Living::Entity_Living(unsigned int id, EntityManager& entityManager, ResourceManager* resourceManager, utl::Vector2f coords):
    Entity_Coll(id, entityManager, resourceManager, coords)
{
    //ctor
}

Entity_Living::~Entity_Living()
{
    //dtor
}

void Entity_Living::defaultUses()
{
	useUpdate(this);
	useObj(this);
	useColl(this);
	useLiving(this);
}
