#include "QuestiaEng/Multiplayer/Server.h"

#include "SFML/System/Time.hpp"

Server::Server(std::string serverName, int tickRate, int threads, unsigned short hostUDPPort, unsigned short hostTCPPort):
	serverName(serverName)
	, tickRate(tickRate)
	, threads(threads)
	, hostUDPPort(hostUDPPort)
	, hostTCPPort(hostTCPPort)
	, accounts()
	, clients()
	, socketListenerThread()
	, tcpListener()
	, udpSocket()
	, tileEngine()
	, entityManager()
{
	tcpListener.listen(hostTCPPort);
	udpSocket.bind(hostUDPPort);

	socketListenerThread = std::thread(Server::socketListenerFun, this);
}

Server::~Server()
{
	//TODO gracefully close all threads
}

void Server::startServer()
{
	//TODO verify the server is not already running
}

void Server::terminate()
{

}

void Server::socketListenerFun()
{
	bool continueRunning = true;
	bool serverOnline = true;

	sf::SocketSelector selector;

	while(continueRunning)
	{
		while(serverOnline)
		{
			if(selector.wait(sf::milliseconds(1)))
			{
				//new client
				if(selector.isReady(tcpListener))
				{
					sf::TcpSocket* newClient = new sf::TcpSocket;
					if(tcpListener.accept(*newClient) == sf::Socket::Done)
					{
						selector.add(*newClient);
						clients.registerNewConnection(newClient);
					}
					else
					{
						delete newClient;
					}
				}
				//unreliable data
				else if(selector.isReady(udpSocket))
				{

				}
				//reliable data
				else
				{
					for(auto& client : clients)
					{
						sf::TcpSocket& connectedClient = client.getSocket();
						if(selector.isReady(connectedClient))
						{
							sf::Packet headerPacket;
							connectedClient.receive(headerPacket);

							PacketHeader head;
							headerPacket >> head;
							switch(static_cast<PHeader>(head.header))
							{
							case PHeader::Error:
								//TODO warn server
								break;
							case PHeader::Login:
								handleLoginPacket(client, headerPacket, static_cast<PALogin>(head.action));
								break;
							case PHeader::Register:
								handleRegisterPacket(client, headerPacket, static_cast<PARegister>(head.action));
								break;
							default:
								break;
							}
						}
					}
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void Server::handleLoginPacket(Client& client, sf::Packet& packet, PALogin packetType)
{
	LoginData& clientLogin = client.getLoginData();

	switch(packetType)
	{
	case PALogin::S1_ClientSendUsername:
		{
			std::string username;
			if(!(packet >> username))
			{
				//TODO log error
				return;
			}
			else if(false)//!clients.existsUsername())
			{
				//invalid username
				sf::Packet retPacket = generateHeaderOnlyPacket(PHeader::Login, PALogin::E_InvalidUsername);
				client.getSocket().send(retPacket);
				clientLogin.lastLoginHeader = PALogin::E_NotStarted;
			}
			else
			{
				//valid username
				utl::RandomSalt serverChallenge(64);
				utl::RawBytes clientRegSeed;// = clients.getRegSeed(username);
				
				sf::Packet responsePacket = generateHeaderOnlyPacket(PHeader::Login, PALogin::S2_ServerSendServerChalAndRegSeed);
				responsePacket << serverChallenge << clientRegSeed;
				
				client.getSocket().send(responsePacket);
				clientLogin.lastLoginHeader =  PALogin::S2_ServerSendServerChalAndRegSeed;
			}
		}
		break;
	case PALogin::S3_ClientSendClientChalAndClientHashResult:
		{
			if(clientLogin.lastLoginHeader != PALogin::S2_ServerSendServerChalAndRegSeed)
			{
				//wrong order of sent packets
				sf::Packet retPacket = generateHeaderOnlyPacket(PHeader::Login, PALogin::E_LogicError);
				client.getSocket().send(retPacket);
				clientLogin.lastLoginHeader = PALogin::E_NotStarted;
			}
		}
	default:
		break;
	}
}
void Server::handleRegisterPacket(Client& client,  sf::Packet& packet, PARegister packetType)
{

}
