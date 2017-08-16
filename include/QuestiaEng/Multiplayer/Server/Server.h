#ifndef SERVER_H
#define SERVER_H

#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <list>

#include "SFML/Network.hpp"

#include "QuestiaEng/ResourceManager/ResourceManager.h"
#include "QuestiaEng/TileEngine/TileEngine.h"
#include "QuestiaEng/EntityManager/EntityManager.h"

#include "QuestiaEng/Utl/Encryptor.h"

#include "QuestiaEng/Multiplayer/PacketHeader.h"

class Account
{
private:
	std::u32string username;
	utl::RawBytes passwordSeed;
	bool isOnline;
	bool isExisting;
};
class Accounts
{
	//TODO implement loading accounts from file in ctor
private:
	//accounts will never be deleted at runtime, only marked as not-existing anymore
	std::vector<Account> accounts;
};

struct LoginData
{
	PALogin lastLoginHeader = PALogin::E_NotStarted;
	PARegister lastRegisterHeader = PARegister::E_NotStarted;
	utl::RawBytes serverChallenge;
	utl::RawBytes clientChallenge;
};

class Client
{
public:
	Client(sf::TcpSocket* socket):
		linkedAccount(nullptr)
		, socket(socket)
		, isConnected(true)
		, login()
	{
	}
	sf::IpAddress getIP() 	 	{return socket->getRemoteAddress();};
	unsigned short getPort() 	{return socket->getRemotePort();};
	sf::TcpSocket& getSocket()	{return *socket;}

	LoginData& getLoginData() 	{return login;}
private:
	Account* linkedAccount;
	std::unique_ptr<sf::TcpSocket> socket;
	bool isConnected;
	LoginData login;
};
class Clients
{
public:
	void registerNewConnection(sf::TcpSocket* socket)
	{
		accounts.emplace_back(socket);
	}
	std::list<Client>::iterator begin()	{return accounts.begin();}
	std::list<Client>::iterator end()	{return accounts.end();}
private:
	std::list<Client> accounts;
};

class Server
{
public:
	Server(std::string serverName = "Questia Server", int tickRate = 32, int threads = 4, unsigned short hostTCPPort = 7777, unsigned short hostUDPPort = 7778 );
	~Server();
	void startServer();
	void terminate();
private:
	void socketListenerFun();

	void handleLoginPacket(Client& client, sf::Packet& packet, PALogin packetType);
	void handleRegisterPacket(Client& client,  sf::Packet& packet, PARegister packetType);

	template<class ActionEnum>
	sf::Packet generateHeaderOnlyPacket(PHeader header, ActionEnum action)
	{
		sf::Packet responsePacket;
		PacketHeader retHeader(header, action);
		responsePacket << retHeader;
		return responsePacket;
	}

	std::string serverName;
	int tickRate;
	int threads;
	unsigned short hostTCPPort;
	unsigned short hostUDPPort;
	std::atomic<bool> continueRunning;
	std::atomic<bool> serverOnline;

	Accounts accounts;
	Clients clients;

	std::thread socketListenerThread;

	sf::TcpListener tcpListener;
	sf::UdpSocket udpSocket;

	TileEngine tileEngine;
	EntityManager entityManager;
};

#endif // SERVER_H
