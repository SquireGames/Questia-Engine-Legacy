#include "QuestiaEng/Multiplayer/Client/ClientsideConnection.h"

ClientsideConnection::ClientsideConnection():
	clientSocket()
	, isConnected(false)
{
	clientSocket.setBlocking(true);
}

ClientsideConnection::~ClientsideConnection()
{
}

