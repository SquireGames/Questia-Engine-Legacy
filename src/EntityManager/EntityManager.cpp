#include "QuestiaEng/EntityManager/EntityManager.h"

/// declaration of Entity Bases
#include "QuestiaEng/EntityManager/Entity/Entity.h"
#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Obj.h"
#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Coll/Entity_Coll.h"
#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Coll/Entity_Living/Entity_Living.h"
#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Coll/Entity_Living/Entity_Player/Entity_Player.h"

/// declaration of derived entities
//Entity_Obj
#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Test/Entity_Orb.h"
//Entity_Coll
#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Coll/Test/Entity_Crate.h"

EntityManager::EntityManager(ResourceManager& _resourceManager):
	resourceManager(_resourceManager)
{
	std::cout<<"DEBUG: Entity Manager Initialized"<<std::endl;

	{
		std::shared_ptr<Entity_Orb> entity = std::make_shared<Entity_Orb> (getNewID(), *this, resourceManager, utl::Vector2f(500,500));

		entities.push_back(entity);
		entities_Obj.push_back(entity);
		ids.push_back(entity->getID());
	}

	{
		std::shared_ptr<Entity_Player> entity = std::make_shared<Entity_Player> (getNewID(), *this, resourceManager);

		entities.push_back(entity);
		entities_Obj.push_back(entity);
		entities_Coll.push_back(entity);
		entities_Living.push_back(entity);
		entities_Player.push_back(entity);
		ids.push_back(entity->getID());
	}

	{
		std::shared_ptr<Entity_Crate> entity = std::make_shared<Entity_Crate> (getNewID(), *this, resourceManager, utl::Vector2f(400,400.01));

		//entities.push_back(entity);
		entities_Obj.push_back(entity);
		entities_Coll.push_back(entity);
		ids.push_back(entity->getID());
	}
	{
		std::shared_ptr<Entity_Crate> entity = std::make_shared<Entity_Crate> (getNewID(), *this, resourceManager, utl::Vector2f(400,430));

		//entities.push_back(entity);
		entities_Obj.push_back(entity);
		entities_Coll.push_back(entity);
		ids.push_back(entity->getID());
	}
	{
		std::shared_ptr<Entity_Crate> entity = std::make_shared<Entity_Crate> (getNewID(), *this, resourceManager, utl::Vector2f(425,400));

		//entities.push_back(entity);
		entities_Obj.push_back(entity);
		entities_Coll.push_back(entity);
		ids.push_back(entity->getID());
	}
}

EntityManager::~EntityManager()
{
	for(auto& id : ids)
	{
		killEntity(id);
	}
	std::cout<<"DEBUG: Destroyed Entity Manager"<<std::endl;
}

void EntityManager::update()
{
	for(auto& entity : entities)
	{
		entity->update();
	}

	//pass collision info
	for(auto& entity : entities_Coll)
	{
		//TODO fix entity matching for collision
		for(auto& entity_other : entities_Coll)
		{
			if(entity->getID() != entity_other->getID())
			{
				if(std::fabs(entity->coords.x - entity_other->coords.x) < 20 && std::fabs(entity->coords.y - entity_other->coords.y) < 20)
				{
					entity->onCollision(*entity_other);
					entity_other->onCollision(*entity);
				}
			}
		}
	}

	//kill
	for(auto& id : deadIDs)
	{
		killEntity(id);
	}
	deadIDs.clear();
}

void EntityManager::draw(sf::RenderWindow& window, const DrawLayer& drawLayer)
{
	//sort entities by y value
	std::sort(entities_Obj.begin(), entities_Obj.end(),
	          [](std::shared_ptr<Entity_Obj> obj_1, std::shared_ptr<Entity_Obj> obj_2)
	{
		return obj_1->coords.y + obj_1->origin.y <= obj_2->coords.y + obj_2->origin.y;
	});

	for(auto& entity : entities_Obj)
	{
		entity->draw(window, drawLayer);
	}
}

Entity_Player& EntityManager::getPlayer(const unsigned int& playerID)
{
	return *entities_Player.front();
}

unsigned int EntityManager::getNewID()
{
	if(idCounter < 4292967295)
	{
		return (idCounter++);
	}
	//game can run a year straight spawning 136 entities per second before need for restart
	//reason why - simplifies networking
	throw std::runtime_error("ENTITYMANAGER: Out of ID's");
}

void EntityManager::killEntity(const unsigned int& id)
{
	removeID(id, entities);
	removeID(id, entities_Obj);
	removeID(id, entities_Coll);
	removeID(id, entities_Living);
	removeID(id, entities_Player);
	ids.remove(id);
	std::cout << "ENTITYMANAGER: Killed ID - " << id << std::endl;
}

void EntityManager::queueKill(const unsigned int& id)
{
	deadIDs.push_back(id);
}

///draw coll bounds
void drawCollBounds(sf::RenderWindow& window, Bounds& bounds, utl::Vector2f coords, sf::Color color)
{
	switch(bounds.getShape())
	{
	case Bounds::Shape::circ:
		{
			/*        Circ& area = boost::get<Circ>(bounds.area);
			        sf::CircleShape circ(area.radius);
			        circ.setFillColor(color - sf::Color(0,0,0,140));
			        circ.setPosition(coords.sf() + bounds.rel_coords.sf());
			        circ.setOrigin(circ.getRadius(), circ.getRadius());
			        window.draw(circ);*/
		}
		break;
	case Bounds::Shape::rect:
		{
			/*
			Rect& area = boost::get<Rect>(bounds.area);
			sf::RectangleShape rect(area.dims.sf());
			rect.setFillColor(color - sf::Color(0,0,0,140));
			rect.setPosition(coords.sf() + bounds.rel_coords.sf());
			rect.setOrigin(area.origin.x, area.origin.y);
			window.draw(rect);
			*/
		}
		break;
	case Bounds::Shape::dot:
		{
			/*
			Dot& area = boost::get<Dot>(bounds.area);
			sf::CircleShape circ(3);
			circ.setOrigin(3, 3);
			circ.setFillColor(color);
			circ.setPosition(coords.sf() + bounds.rel_coords.sf() + area.point.sf());
			window.draw(circ);
			*/
		}
	case Bounds::Shape::poly:
		{
			//TODO implement poly collision shape draw
		}

	default:
		break;
	}
}

void EntityManager::draw_coll(sf::RenderWindow& window)
{
	for(auto& entity : entities_Coll)
	{
		drawCollBounds(window, entity->hitBounds, entity->coords, sf::Color::Blue);
		drawCollBounds(window, entity->collBounds, entity->coords, sf::Color::Red);
	}
	for(auto& entity : entities_Obj)
	{
		auto originPoint = Bounds(Dot(entity->origin));
		drawCollBounds(window, originPoint, entity->coords, sf::Color::Yellow);
	}
}

void EntityManager::attemptMove(Entity_Coll& entity, const utl::Vector2f& velocity)
{
	utl::Vector2f allowedMovement = velocity;
	for(auto& other : entities_Coll)
	{
		if(entity.getID() != other->getID())
		{
			if(allowedMovement.x == 0 && allowedMovement.y == 0)
			{
				break;
			}
			//TODO remember to check max range for collision
			if(true)
			{
				if(entity.collBounds.getShape() == Bounds::Shape::rect && other->collBounds.getShape() == Bounds::Shape::rect)
				{
					/*
					utl::Vector2f& entity_coords     = entity.coords;
					Bounds&        entity_collBounds = entity.collBounds;
					utl::Vector2f& other_coords      = other->coords;
					Bounds&        other_collBounds  = other->collBounds;
					Rect& entityRect = boost::get<Rect>(entity_collBounds.area);
					Rect& otherRect  = boost::get<Rect>(other_collBounds.area);

					utl::FloatRect other_coll_rect = utl::FloatRect(other_coords.x + other_collBounds.rel_coords.x - otherRect.origin.x,
					                                 other_coords.y + other_collBounds.rel_coords.y - otherRect.origin.y,
					                                 otherRect.dims.x, otherRect.dims.y);
					utl::FloatRect entity_coll_rect_x = utl::FloatRect(entity_coords.x + allowedMovement.x + entity_collBounds.rel_coords.x - entityRect.origin.x,
					                                    entity_coords.y + entity_collBounds.rel_coords.y - entityRect.origin.y,
					                                    entityRect.dims.x, entityRect.dims.y);
					if(other_coll_rect.isColliding(entity_coll_rect_x))
					{
					    allowedMovement.x = 0;
					}
					utl::FloatRect entity_coll_rect_y = utl::FloatRect(entity_coords.x + allowedMovement.x + entity_collBounds.rel_coords.x - entityRect.origin.x,
					                                    entity_coords.y + allowedMovement.y + entity_collBounds.rel_coords.y - entityRect.origin.y,
					                                    entityRect.dims.x, entityRect.dims.y);
					if(other_coll_rect.isColliding(entity_coll_rect_y))
					{
					    allowedMovement.y = 0;
					}
					*/
				}
			}
		}
	}
	entity.forceMove(entity, allowedMovement);
}
