#include "QuestiaEng/Multiplayer/Server.h"

Server::Server(std::string serverName, int tickRate, int threads, unsigned short hostUDPPort, unsigned short hostTCPPort):
	serverName(serverName)
	, tickRate(tickRate)
	, threads(threads)
	, hostUDPPort(hostUDPPort)
	, hostTCPPort(hostTCPPort)
	, tileEngine()
	, entityManager()
{
	
}

void Server::startServer()
{
	
}

void Server::terminate()
{
	
}
