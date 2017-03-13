#include "QuestiaEng/MultiplayerManager/Server.h"

//{ ctor + dtor

Server::Server():
    tick(0)
    , udpSocket_send()
    , selector()
    , clientCount(0)
{
    ///send
    udpSocket_send   .bind(8000);

    ///receive
    udpSocket_receive.bind(8001);
    selector.add(udpSocket_receive);
    listener.listen(8004);
    selector.add(listener);

    ///temp
    registeredClients.push_back(std::make_pair(std::string("admin"), std::string("pass")));
}

Server::~Server()
{
    //dtor
    for(unsigned int it = 0; it != clientVector.size(); it++)
    {
        delete clientVector[it]->tcpSocket_send;
        delete clientVector[it]->tcpSocket_receieve;
        delete clientVector[it];
    }
    for(unsigned int it = 0; it != mapVector.size(); it++)
    {
        delete mapVector[it];
    }
    for(unsigned int it = 0; it != toBeSortedTcp.size(); it++)
    {
        delete toBeSortedTcp[it];
    }
}

//}

//{ game logic

///send thread
void Server::update()
{
    // get player data
    std::vector <Packet_Player> playerData;
    playerDataMutex.lock();
    playerData = playerData_received;
    playerData_received.clear();
    playerDataMutex.unlock();
    std::sort(playerData.begin(), playerData.end());
    for(unsigned int it = 0; it != playerData.size(); it++)
    {
        playerData_queue.push(playerData[it]);
    }

    // set player location
    while(!playerData_queue.empty())
    {
        //std::cout << "Packet: " << playerData_queue.front().packetNumber << ", Coords: (" << playerData_queue.front().coords_x << ", " << playerData_queue.front().coords_y << ")" << std::endl;
        entityManagerMutex.lock();
        //entityManager.setPlayerCoordinates(sf::Vector2f(playerData_queue.front().coords_x, playerData_queue.front().coords_y));
        entityManagerMutex.unlock();
        playerData_queue.pop();
    }

    //entityManager.update(tileEngine, sf::Vector2f (0,0), sf::Vector2f (0,0), 0.f);
}

//}

//{ send data

///send thread
void Server::send()
{
    tick++;

    entityManagerMutex.lock();
    //float coords_x = entityManager.getPlayerCoordinates().x;
    //float coords_y = entityManager.getPlayerCoordinates().y;
    entityManagerMutex.unlock();

    //Packet_Player packetObj(tick, -1, std::make_pair(coords_x, coords_y));
    //sf::Packet testPacket;
    //testPacket << packetObj;

    //sf::IpAddress tempIP = sf::IpAddress::getLocalAddress();
    //unsigned short tempPort = 8003;

    //udpSocket_send.send(testPacket, tempIP, tempPort);
}

//}

//{ receive data

///receive thread
void Server::receive()
{
    if(selector.wait(sf::Time(sf::milliseconds(sf::Int32(100)))))
    {
        ///get udp sockets
        if(selector.isReady(udpSocket_receive))
        {
            unsigned short tempPort2 = 8002;
            sf::IpAddress tempIP = sf::IpAddress::getLocalAddress();

            sf::Packet packet;
            udpSocket_receive.receive(packet,tempIP, tempPort2);

            int header;
            packet >> header;
            switch (header)
            {
            case pkt::Header::player:
            {
                Packet_Player player;
                packet >> player;

                playerDataMutex.lock();
                playerData_received.push_back(player);
                playerDataMutex.unlock();
            }
            break;
            case pkt::Header::playerContainer:
            {
                std::cout << "player container" << std::endl;
            }
            break;
            case pkt::Header::entity:
                std::cout << "entity" << std::endl;
                break;
            }
        }
        ///listen for new connections
        else if(selector.isReady(listener))
        {
            sf::TcpSocket* clientSocket = new sf::TcpSocket;
            if(listener.accept(*clientSocket) == sf::Socket::Done)
            {
                toBeSortedTcp.push_back(clientSocket);
                selector.add(*clientSocket);
            }
            else
            {
                delete clientSocket;
            }
        }
        ///get client tcp
        else
        {
            for(unsigned int it = 0; it != toBeSortedTcp.size(); it++)
            {
                if(toBeSortedTcp[it] != nullptr)
                {
                    sf::TcpSocket& clientSocket = *toBeSortedTcp[it];
                    if(selector.isReady(clientSocket))
                    {
                        sf::Packet packet;
                        if(clientSocket.receive(packet) == sf::Socket::Done)
                        {
                            int header;
                            packet >> header;
                            switch (header)
                            {
                            case pkt::Header::clientTcp:
                            {
                                Packet_Init_Tcp packetObj;
                                packet >> packetObj;

                                sf::TcpSocket* tcp[2] {nullptr, nullptr};

                                for(unsigned int it2 = 0; it2 != toBeSortedTcp.size(); it2++)
                                {
                                    if(toBeSortedTcp[it2]->getRemoteAddress().toString() == packetObj.clientIP_local ||
                                            toBeSortedTcp[it2]->getRemoteAddress().toString() == packetObj.clientIP_public)
                                    {
                                        // packetObj.
                                    }
                                }

                                std::cout << "SERVER: TCP SORT - " << packetObj.clientIP_local << ", " << packetObj.clientIP_public << std::endl;
                            }
                            break;
                            default:
                                break;
                            }
                        }
                    }
                }
            }

            for(unsigned int it = 0; it != clientVector.size(); it++)
            {
                sf::TcpSocket& clientSocket = *clientVector[it]->tcpSocket_receieve;
                if(selector.isReady(clientSocket))
                {
                    sf::Packet packet;
                    if(clientSocket.receive(packet) == sf::Socket::Done)
                    {
                        switch (clientVector[it]->clientStage)
                        {
                        case ClientStage::disconnected:
                            //one of the two sockets are not yet connected
                            break;
                        case ClientStage::connected:
                            //used to send client user and pass
                            int header;
                            packet >> header;
                            switch (header)
                            {
                            case pkt::Header::clientLogin:
                            {
                                Packet_Init_Login loginInfo;
                                packet >> loginInfo;
                                for(const std::pair <std::string, std::string>& login : registeredClients)
                                {
                                    if(login.first == loginInfo.username && login.second == loginInfo.password)
                                    {
                                        std::cout << "SERVER: Login accepted" << std::endl;

                                    }
                                    else
                                    {
                                        std::cout << "SERVER: Login denied" << std::endl;

                                        Packet_Init_SvrResp responsePacketObj(false);
                                        sf::Packet responsePacket;
                                        responsePacket << responsePacketObj;

                                        //clientVector[it]->tcpSocket_receieve->send(responsePacket);
                                    }
                                }

                            }
                            default:
                            {

                            }
                            }

                            break;
                        case ClientStage::choosingCharacter:
                            break;
                        case ClientStage::active:
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }
}

//}
