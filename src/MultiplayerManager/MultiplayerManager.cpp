#include "QuestiaEng/MultiplayerManager/MultiplayerManager.h"

//{ ctor + dtor

MultiplayerManager::MultiplayerManager(std::string _serverName):
    server(nullptr)
    , thread_client_main  (&MultiplayerManager::client_mainThread,  this)
    , thread_host_send    (&MultiplayerManager::host_sendThread,    this)
    , thread_host_receive (&MultiplayerManager::host_receiveThread, this)
    , host_terminate     (false)
    , client_terminate   (false)
    , host_alive_send    (false)
    , host_alive_receive (false)
    , client_alive       (false)
    , isRunningServer    (false)
    , isRunningClient    (false)
    , isKillingServer    (false)
    , isKillingClient    (false)
    , tickRate(16) // no active thread at this point, 16 default
    , client_udpSocket_send()
    , client_udpSocket_receive()
    , client_tcpSocket_send()
    , client_tcpSocket_receive()
    , client_tempTick(0)
{
    client_udpSocket_send.bind(8002);
    client_udpSocket_receive.bind(8003);
}

MultiplayerManager::~MultiplayerManager()
{
    // just to make sure
    terminateHost();
    terminateClient();
}

//}

//{ host commands

void MultiplayerManager::startHostingServer()
{
    if(!isRunningServer)
    {
        server = new Server();

        thread_host_send   .launch();
        thread_host_receive.launch();
        isRunningServer = true;

        host_aliveMutex.lock();
        host_alive_send =    true;
        host_alive_receive = true;
        host_aliveMutex.unlock();
    }
}

void MultiplayerManager::terminateHost()
{
    if(isRunningServer)
    {
        host_terminateMutex.lock();
        host_terminate = true;
        host_terminateMutex.unlock();
        isKillingServer = true;
    }
}

void MultiplayerManager::host_changeTickRate(float _tickRate)
{
    host_changeTickRateMutex.lock();
    if(_tickRate != tickRate)
    {
        host_tickChange = true;
        tickRate = _tickRate;
    }
    host_changeTickRateMutex.unlock();
}

std::string MultiplayerManager::getPublicIP()
{
    return sf::IpAddress::getPublicAddress().toString();
}

std::string MultiplayerManager::getLocalIP()
{
    return sf::IpAddress::getLocalAddress().toString();
}

//}

//{ client commands

bool MultiplayerManager::joinServer(std::string ipAddress)
{
    if(!isRunningClient)
    {
        if((client_tcpSocket_send.connect   (ipAddress, 8004, sf::Time(sf::seconds(2))) == sf::Socket::Done) &&
                (client_tcpSocket_receive.connect(ipAddress, 8004, sf::Time(sf::seconds(2))) == sf::Socket::Done))
        {
            Packet_Init_Tcp tcpPacket(sf::IpAddress::getLocalAddress().toString(), sf::IpAddress::getPublicAddress().toString(),
                                      client_tcpSocket_send.getLocalPort(), client_tcpSocket_receive.getLocalPort());
            sf::Packet initPacket;
            initPacket << tcpPacket;
            client_tcpSocket_send.send(initPacket);

            thread_client_main.launch();
            isRunningClient = true;

            client_aliveMutex.lock();
            client_alive = true;
            client_aliveMutex.unlock();

            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}
void MultiplayerManager::terminateClient()
{
    if(isRunningClient)
    {
        client_terminateMutex.lock();
        client_terminate = true;
        client_terminateMutex.unlock();
        isKillingClient = true;
    }
}

//}

//{ client send + receive data

void MultiplayerManager::sendData(EntityManager& entityManager)
{
    if(isRunningClient)
    {
        entityManagerMutex.lock();
//        float coords_x = entityManager.getPlayerCoordinates().x;
 //       float coords_y = entityManager.getPlayerCoordinates().y;
        entityManagerMutex.unlock();

        client_tempTick++;
 //       Packet_Player packetObj(client_tempTick, -1, std::make_pair(coords_x, coords_y));
        sf::Packet testPacket;
//        testPacket << packetObj;

        client_udpSocket_send.send(testPacket, sf::IpAddress::getLocalAddress(), 8001);
    }
}
void MultiplayerManager::receiveData(EntityManager& entityManager)
{
    if(isRunningClient)
    {
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

        while(!playerData_queue.empty())
        {
            //std::cout << "Packet: " << playerData_queue.front().packetNumber << ", Coords: (" << playerData_queue.front().coords_x << ", " << playerData_queue.front().coords_y << ")" << std::endl;
            entityManagerMutex.lock();
//            entityManager.setPlayerCoordinates2(sf::Vector2f(playerData_queue.front().coords_x, playerData_queue.front().coords_y));
            entityManagerMutex.unlock();
            playerData_queue.pop();
        }
    }
}

//}

//{ server update

void MultiplayerManager::update()
{
    if(isRunningServer && isKillingServer)
    {
        host_aliveMutex.lock();
        if(!host_alive_send && !host_alive_receive)
        {
            std::cout << "SERVER TERMINATED" << std::endl;
            isRunningServer = false;
            isKillingServer = false;

            //just to make sure, should already be dead
            thread_host_send.terminate();
            thread_host_receive.terminate();

            //threads are dead, should be safe
            host_terminate = false;
        }
        host_aliveMutex.unlock();
    }
    if(isRunningClient && isKillingClient)
    {
        client_aliveMutex.lock();
        if(!client_alive)
        {
            std::cout << "CLIENT TERMINATED" << std::endl;
            isRunningClient = false;
            isKillingClient = false;

            //just to make sure
            thread_client_main.terminate();

            //just in case, should be safe
            client_terminate = false;
        }
        client_aliveMutex.unlock();
    }
}

//}

//{ server + client gets

bool MultiplayerManager::isServerDead()
{
    return !isRunningServer;
}
bool MultiplayerManager::isClientDead()
{
    return !isRunningClient;
}

//}

//{ server threads

void MultiplayerManager::host_sendThread()
{
    std::cout << "host_sendThread() init" << std::endl;
    Server* svr = server; //saved to delete the server

    ///tick rate
    host_changeTickRateMutex.lock();
    float threadTickRate = tickRate;
    host_changeTickRateMutex.unlock();
    sf::Time timePerUpdate = sf::seconds(1.f/144.f);
    sf::Time timePerSend   = sf::seconds(1.f/threadTickRate);
    sf::Clock clock_update;
    sf::Clock clock_send;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    sf::Time timeSinceLastSend = sf::Time::Zero;

    host_terminateMutex.lock();
    while(!host_terminate)
    {
        host_terminateMutex.unlock();

        ///check for changes in tick rate
        host_changeTickRateMutex.lock();
        if(host_tickChange)
        {
            host_tickChange = false;
            threadTickRate = tickRate;
            if(threadTickRate > 144.f)
            {
                threadTickRate = 144.f;
            }
            timePerSend = sf::seconds(1.f/threadTickRate);
        }
        host_changeTickRateMutex.unlock();



        ///update
        sf::Time elapsedTime_update = clock_update.restart();
        timeSinceLastUpdate += elapsedTime_update;
        while (timeSinceLastUpdate > timePerUpdate)
        {
            timeSinceLastUpdate -= timePerUpdate;
            svr->update();
        }


        ///send
        sf::Time elapsedTime_send = clock_send.restart();
        timeSinceLastSend += elapsedTime_send;
        while (timeSinceLastSend > timePerSend)
        {
            timeSinceLastSend -= timePerSend;
            svr->send();
        }

        sf::sleep(sf::Time(sf::milliseconds(sf::Int32(4))));

        host_terminateMutex.lock();
    }
    host_terminateMutex.unlock();

    ///death
    host_aliveMutex.lock();
    if(!host_alive_receive)
    {
        delete svr;
        svr = nullptr;
    }
    host_alive_send = false;
    host_aliveMutex.unlock();
    std::cout << "host_sendThread() ded" << std::endl;
}

void MultiplayerManager::host_receiveThread()
{
    std::cout << "host_recieveThread() init" << std::endl;
    Server* svr = server; //saved to delete the server

    host_terminateMutex.lock();
    while(!host_terminate)
    {
        host_terminateMutex.unlock();
        server->receive();
        host_terminateMutex.lock();
    }
    host_terminateMutex.unlock();


    ///death
    host_aliveMutex.lock();
    if(!host_alive_send)
    {
        delete svr;
        svr = nullptr;
    }
    host_alive_receive = false;
    host_aliveMutex.unlock();
    std::cout << "host_recieveThread() ded" << std::endl;
}

//}

//{ client thread

void MultiplayerManager::client_mainThread()
{
    std::cout << "client_mainThread() init" << std::endl;

    sf::SocketSelector selector;
    selector.add(client_udpSocket_receive);

    client_terminateMutex.lock();

    while(!client_terminate)
    {
        client_terminateMutex.unlock();
        if(selector.wait(sf::Time(sf::milliseconds(sf::Int32(100)))))
        {
            ///UDP for in game
            if(selector.isReady(client_udpSocket_receive))
            {
                sf::IpAddress tempIP = sf::IpAddress::getLocalAddress();
                unsigned short tempPort = 8000;

                sf::Packet packet;
                client_udpSocket_receive.receive(packet,tempIP, tempPort);

                //std::cout << ">>>>>CLIENT<<<<<" << std::endl;

                int header;
                packet >> header;
                switch (header)
                {
                case pkt::Header::player:
                {
                    //std::cout << "player" << std::endl;
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

                //std::cout << "<<<<<CLIENT>>>>>" << std::endl;
            }
            ///TCP
            else
            {
                if(selector.isReady(client_tcpSocket_receive))
                {
                    sf::Packet packet;
                    if(client_tcpSocket_receive.receive(packet) == sf::Socket::Done)
                    {
                        // move data to other thread
                    }
                }
            }
        }

        client_terminateMutex.lock();
    }
    client_terminateMutex.unlock();

    ///death
    client_aliveMutex.lock();
    client_alive = false;
    client_aliveMutex.unlock();
    std::cout << "client_mainThread() ded" << std::endl;
}

//}

//{ client init
bool MultiplayerManager::client_init_testUDP()
{
    sf::Packet testPacket;

//    client_udpSocket_receive.send();
    //  client_udpSocket_send.send();

    return false;
}
//}
