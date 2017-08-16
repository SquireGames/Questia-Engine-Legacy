#include "QuestiaEng/Multiplayer/Server/Server.h"

#include "SFML/System/Time.hpp"

Server::Server(std::string serverName, int tickRate, int threads, unsigned short hostTCPPort, unsigned short hostUDPPort):
	serverName(serverName)
	, tickRate(tickRate)
	, threads(threads)
	, hostTCPPort(hostTCPPort)
	, hostUDPPort(hostUDPPort)
	, continueRunning(true)
	, serverOnline(false)
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
	
	tcpListener.setBlocking(true);
	udpSocket.setBlocking(true);
	
	std::cout << "Server Started!"<< std::endl; 
	std::cout << "Local IP  = " << sf::IpAddress::getLocalAddress().toString() << ":" << tcpListener.getLocalPort() << std::endl;
	std::cout << "Remote IP = " << sf::IpAddress::getPublicAddress(sf::seconds(5)).toString() << ":" << tcpListener.getLocalPort() << std::endl;

	socketListenerThread = std::thread(Server::socketListenerFun, this);
}

Server::~Server()
{
	serverOnline = false;
	continueRunning = false;
	//TODO save state of server or something
	

	//kill the thread
	socketListenerThread.join();
}

void Server::startServer()
{
	serverOnline = true;
}

void Server::terminate()
{
	//TODO save state of server or something
	serverOnline = false;
}

void Server::socketListenerFun()
{
	sf::SocketSelector selector;
	selector.add(tcpListener);
	selector.add(udpSocket);

	while(continueRunning)
	{
		while(serverOnline)
		{
			if(selector.wait(sf::seconds(1)))
			{
				//new client
				if(selector.isReady(tcpListener))
				{
					sf::TcpSocket* newClient = new sf::TcpSocket;
					if(tcpListener.accept(*newClient) == sf::Socket::Done)
					{
						newClient->setBlocking(true);
						selector.add(*newClient);
						clients.registerNewConnection(newClient);
						std::cout << "New Client connected! IP = " << newClient->getRemoteAddress().toString() << ":" << newClient->getRemotePort() << std::endl;
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
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
