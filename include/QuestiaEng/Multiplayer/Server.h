#ifndef SERVER_H
#define SERVER_H

#include <thread>
#include <mutex>
#include <vector>
#include <list>

#include "SFML/Network.hpp"

#include "QuestiaEng/ResourceManager/ResourceManager.h"
#include "QuestiaEng/TileEngine/TileEngine.h"
#include "QuestiaEng/EntityManager/EntityManager.h"

#include "QuestiaEng/Utl/Encryptor.h"

struct PacketHeader
{
	template<class T, class U>
	PacketHeader(T header, U action):
		header(static_cast<unsigned int>(header))
		, action(static_cast<unsigned int>(action))
	{
	}
	PacketHeader() {}
	unsigned int header; //0..255
	unsigned int action; //0..255
};
sf::Packet& operator<<(sf::Packet& packet, const PacketHeader& header)
{
	return packet << header.header << header.action;
}
sf::Packet& operator>>(sf::Packet& packet, PacketHeader& header)
{
	unsigned int head; //0..255
	unsigned int action; //0..255
	if(packet >> head && packet >> action)
	{
		header.header = head;
		header.action = action;
	}
	else
	{
		header.header = 0;
		header.action = 0;
	}
	return packet;
}
enum class PHeader : int
{
	Error = 0,
	Register = 1,
	Login = 2
};
enum class PARegister : int
{
	S1_ServerSendRegSeed = 0,
	S2_ClientSendHash = 1,
	S3_ServerSendSuccess = 2,

	E_NotStarted = 10,
	E_UsernameTaken = 11,
	E_InvalidUsername = 12
};
enum class PALogin : int
{
	S1_ClientSendUsername = 0,	// sf::Packet << std::string username
	S2_ServerSendServerChalAndRegSeed = 1, // sf::Packet << utl::RawBytes serverChallenge(64) << utl::RawBytes regSeed(64)
	S3_ClientSendClientChalAndClientHashResult = 2,
	S4_ServerSendServerHashResult = 3,

	E_NotStarted = 10, // just header
	E_InvalidUsername = 11, // just header
	E_InvalidCredentials = 12, // just header
	E_LogicError = 13 // just header
};

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
	Server(std::string serverName = "Questia Server", int tickRate = 32, int threads = 4, unsigned short hostUDPPort = 7777, unsigned short hostTCPPort = 7778);
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
	unsigned short hostUDPPort;
	unsigned short hostTCPPort;

	Accounts accounts;
	Clients clients;

	std::thread socketListenerThread;

	sf::TcpListener tcpListener;
	sf::UdpSocket udpSocket;

	TileEngine tileEngine;
	EntityManager entityManager;
};

#endif // SERVER_H
