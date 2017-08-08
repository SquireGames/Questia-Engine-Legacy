#ifndef SERVER_H
#define SERVER_H

#include "QuestiaEng/ResourceManager/ResourceManager.h"
#include "QuestiaEng/TileEngine/TileEngine.h"
#include "QuestiaEng/EntityManager/EntityManager.h"

class Server
{
public:
	Server();
	~Server();
	
	void startServer();
	void terminate();
	
private:
	TileEngine tileEngine;
	EntityManager entityManager;
};

#endif // SERVER_H
