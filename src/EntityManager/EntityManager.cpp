#include "QuestiaEng/EntityManager/EntityManager.h"

/// declaration of Entity Bases
#include "QuestiaEng/EntityManager/Entity/Entity.h"
#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Obj.h"
#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Coll/Entity_Coll.h"
#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Coll/Entity_Living/Entity_Living.h"
#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Coll/Entity_Living/Entity_Player/Entity_Player.h"

#ifdef DEBUGMODE
#include "QuestiaEng/Utl/Logger.h"
#endif

EntityManager::EntityManager(ResourceManager& resourceManager, sf::RenderWindow& window):
	resourceManager(&resourceManager)
	, window(&window)
#ifdef DEBUGMODE
	, displayTextures(true)
#endif
{
	std::cout<<"DEBUG: <Client> Entity Manager Initialized"<<std::endl;
}

EntityManager::EntityManager():
	resourceManager(nullptr)
	, window(nullptr)
#ifdef DEBUGMODE
	, displayTextures(false)
#endif
{
	std::cout<<"DEBUG: <Server> Entity Manager Initialized"<<std::endl;
}


EntityManager::~EntityManager()
{
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

void EntityManager::draw(DrawLayer drawLayer)
{
#ifdef DEBUGMODE
	if(!displayTextures)
	{
		LOG("EntityManager - Tried drawing with Server mode constructor");
		return;
	}
#endif

	//sort entities by y value
	std::sort(entities_Obj.begin(), entities_Obj.end(),
	          [](Entity_Obj* obj_1, Entity_Obj* obj_2)
	{
		return obj_1->coords.y + obj_1->getGroundOffset() <= obj_2->coords.y + obj_2->getGroundOffset();
	});

	for(auto& entity : entities_Obj)
	{
		entity->draw(*window, drawLayer);
	}
}

Entity_Player& EntityManager::getPlayer(unsigned int playerID)
{
	return *entities_Player.front();
}

unsigned int EntityManager::getNewID()
{
	if(idCounter < 4292967295)
	{
		return (idCounter++);
	}
	throw std::runtime_error("ENTITYMANAGER: Out of ID's");
}

Entity* EntityManager::getEntity(unsigned int id)
{
	for(std::unique_ptr<Entity>& e : entities)
	{
		if(e->getID() == id)
		{
			return &*e;
		}
	}
	return nullptr;
}

void EntityManager::killEntity(unsigned int id)
{
	Entity* e = getEntity(id);
	if(e->inEntity_Base)
	{
		removeID(id, entities_Base);
	}
	if(e->inEntity_Obj)
	{
		removeID(id, entities_Obj);
	}
	if(e->inEntity_Coll)
	{
		removeID(id, entities_Coll);
	}
	if(e->inEntity_Living)
	{
		removeID(id, entities_Living);
	}
	if(e->inEntity_Player)
	{
		removeID(id, entities_Player);
	}
	removeID(id, entities);
	ids.remove(id);
	std::cout << "ENTITYMANAGER: Killed ID - " << id << std::endl;
}

void EntityManager::queueKill(unsigned int id)
{
	deadIDs.push_back(id);
}

void EntityManager::drawCollBounds(sf::RenderWindow& window, const Bounds* bounds, utl::Vector2f coords, sf::Color color)
{
	switch(bounds->getShape())
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

void EntityManager::draw_coll()
{
#ifdef DEBUGMODE
	if(!displayTextures)
	{
		LOG("EntityManager - Tried drawing with Server mode constructor");
		return;
	}
#endif

	for(auto& entity : entities_Coll)
	{
		drawCollBounds(*window, entity->getHitBounds(), entity->coords, sf::Color::Blue);
		drawCollBounds(*window, entity->getCollBounds(), entity->coords, sf::Color::Red);
	}
	for(auto& entity : entities_Obj)
	{
		auto originPoint = Bounds(Dot(utl::Vector2f(16, entity->getGroundOffset())));
		drawCollBounds(*window, &originPoint, entity->coords, sf::Color::Yellow);
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
				if(entity.getCollBounds()->getShape() == Bounds::Shape::rect && other->getCollBounds()->getShape() == Bounds::Shape::rect)
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
	entity.forceMove(allowedMovement);
}

void EntityManager::reg(const std::string& name, std::function<Entity*(unsigned int id, EntityManager&)> entity)
{
	entityRegistry[name] = entity;
}
void EntityManager::reg(const std::string& name, std::function<Entity*(unsigned int id, EntityManager&, ResourceManager*, utl::Vector2f coords)> entity)
{
	entityRegistry_Obj[name] = entity;
}

int EntityManager::spawn(const std::string& name)
{
	int id = getNewID();
	std::unique_ptr<Entity> entity(entityRegistry.at(name)(id, *this));
	entity->defaultUses();
	entities.push_back(std::move(entity));
	ids.push_back(id);
	return id;
}

int EntityManager::spawn(const std::string& name, utl::Vector2f coords)
{
	int id = getNewID();
	std::unique_ptr<Entity> entity(entityRegistry_Obj.at(name)(id, *this, resourceManager, coords));
	entity->defaultUses();
	entities.emplace_back(std::move(entity));
	ids.push_back(id);
	return id;
}
