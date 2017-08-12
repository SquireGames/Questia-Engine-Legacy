#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <memory>
#include "SFML/Network/Packet.hpp"

namespace utl
{

class RawBytes
{
public:
	RawBytes(std::unique_ptr<char[]> data, int length);
	RawBytes():RawBytes(std::make_unique<char[]>(1), 1){}
	RawBytes(const utl::RawBytes&);
	RawBytes(utl::RawBytes&&) = default;
	RawBytes& operator= (const utl::RawBytes&);
	RawBytes& operator= (utl::RawBytes&&) = default;
	virtual ~RawBytes() = default;
	
	RawBytes(const std::u32string& str);
	RawBytes(const std::string& str);
	RawBytes(const char* str);
	
	const RawBytes operator+ (const RawBytes& rhs) const;
	bool operator== (const RawBytes& other) const;
	
	void printValue() const;
	void printAsStr() const;
	void printHex() const;
	
	unsigned char* get(); 
	const unsigned char* getConst() const; 
	int size() const;
	
	void restrictPacketExtraction(unsigned int byteCount);
	unsigned int getRestrictedPacketSize();

protected:
	std::unique_ptr<char[]> data;
	unsigned int length;
	unsigned int largestPacketSize = 0;
};

class RandomSalt : public RawBytes
{
public:
	RandomSalt(int length);
};

//hashes
void hash_whirlpool(RawBytes& data);

}

sf::Packet& operator<< (sf::Packet& packet, const utl::RawBytes& bytes);
sf::Packet& operator<< (sf::Packet& packet, const utl::RandomSalt& bytes);
sf::Packet& operator>> (sf::Packet& packet, utl::RawBytes& bytes);

#endif // ENCRYPTOR_H
