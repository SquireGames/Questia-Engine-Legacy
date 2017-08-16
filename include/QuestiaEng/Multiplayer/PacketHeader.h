#ifndef PACKETHEADER_H
#define PACKETHEADER_H

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

enum class PHeader : unsigned int
{
	Error = 0,
	Register = 1,
	Login = 2
};


enum class PARegister : unsigned int
{
	S1_ServerSendRegSeed = 0,
	S2_ClientSendHash = 1,
	S3_ServerSendSuccess = 2,

	E_NotStarted = 10,
	E_UsernameTaken = 11,
	E_InvalidUsername = 12
};
enum class PALogin : unsigned int
{
	S1_ClientSendUsername = 0,						// sf::Packet << std::string username
	S2_ServerSendServerChalAndRegSeed = 1, 			// sf::Packet << utl::RawBytes serverChallenge(64) << utl::RawBytes regSeed(64)
	S3_ClientSendClientChalAndClientHashResult = 2,
	S4_ServerSendServerHashResult = 3,

	E_NotStarted = 10, // just header
	E_InvalidUsername = 11, // just header
	E_InvalidCredentials = 12, // just header
	E_LogicError = 13 // just header
};

#endif // PACKETHEADER_H
