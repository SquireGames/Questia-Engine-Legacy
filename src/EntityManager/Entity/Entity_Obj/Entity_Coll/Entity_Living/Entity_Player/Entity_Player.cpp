#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Coll/Entity_Living/Entity_Player/Entity_Player.h"

Entity_Player::Entity_Player(unsigned int id, EntityManager& entityManager, ResourceManager& resourceManager, utl::Vector2f coords):
	Entity_Living(id, entityManager, resourceManager, coords)
{

}
