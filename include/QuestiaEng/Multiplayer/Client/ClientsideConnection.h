#ifndef CLIENTSIDECONNECTION_H
#define CLIENTSIDECONNECTION_H

#include <type_traits>

#include "SFML/Network.hpp"

class ClientsideConnection
{
public:
	ClientsideConnection();
	~ClientsideConnection();

	//accepts strings, "102.168.1.1" and "localhost" and
	//numbers e.g. (unsigned int) or (char, char, char, char)
	template<class IpAddress, typename = typename std::enable_if<std::is_constructible<sf::IpAddress, IpAddress>::value>::type>
	bool attemptConnection(IpAddress&& ip, unsigned short port)
	{
		if(isConnected)
		{
			//TODO log warning
			return false;
		}
		sf::IpAddress serverIP(std::forward<IpAddress>(ip));
		return (sf::Socket::Status::Done == clientSocket.connect(serverIP, port, sf::seconds(5)));
	}
private:
	sf::TcpSocket clientSocket;
	bool isConnected;
};

#endif // CLIENTSIDECONNECTION_H
