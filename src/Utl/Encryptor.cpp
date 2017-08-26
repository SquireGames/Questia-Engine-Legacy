#include "QuestiaEng/Utl/Encryptor.h"

#include <iostream>

#include "Cryptopp/osrng.h"
#include "Cryptopp/filters.h"
#include "Cryptopp/whrlpool.h"
#include "Cryptopp/hex.h"

static CryptoPP::NonblockingRng rng;
static CryptoPP::Whirlpool whirlpool;

utl::RawBytes::RawBytes(std::unique_ptr<char[]> data, int length):
	data(std::move(data)), length(length)
{
}
utl::RawBytes::RawBytes(const utl::RawBytes& other):
	data(std::make_unique<char[]>(other.length)), length(other.length)
{
	char* thisDataIt = data.get();
	const char* otherDataIt = other.data.get();
	for(unsigned int i = 0; i < length; i++)
	{
		*thisDataIt = *otherDataIt;
		otherDataIt++;
		thisDataIt++;
	}
}
utl::RawBytes& utl::RawBytes::operator=(const utl::RawBytes& rhs)
{
	if(length != rhs.length)
	{
		data.reset(new char[rhs.length]);
		length = rhs.length;
	}
	char* thisDataIt = data.get();
	const char* otherDataIt = rhs.data.get();
	for(unsigned int i = 0; i < length; i++)
	{
		*thisDataIt = *otherDataIt;
		otherDataIt++;
		thisDataIt++;
	}
	return *this;
}

const utl::RawBytes utl::RawBytes::operator+(const RawBytes& rhs) const
{
	int newLength = length + rhs.length;
	utl::RawBytes newBytes{std::make_unique<char[]>(newLength), newLength};
	char* thisDataIt = data.get();
	char* newDataIt = newBytes.data.get();
	char* rhsDataIt = rhs.data.get();
	for(unsigned int i = 0; i < length; i++)
	{
		*newDataIt = *thisDataIt;
		newDataIt++;
		thisDataIt++;
	}
	for(unsigned int i = 0; i < rhs.length; i++)
	{
		*newDataIt = *rhsDataIt;
		newDataIt++;
		rhsDataIt++;
	}
	return newBytes;
}

bool utl::RawBytes::operator==(const RawBytes& other) const
{
	if(length != other.length)
	{
		return false;
	}
	
	unsigned char* thisData = reinterpret_cast<unsigned char*>(static_cast<char*>(data.get()));
	unsigned char* otherData = reinterpret_cast<unsigned char*>(static_cast<char*>(other.data.get()));
	
	for(unsigned int i = 0; i < length; i++)
	{
		if(*thisData != *otherData)
		{
			return false;
		}
		thisData++;
		otherData++;
	}
	return true;
}

std::string utl::RawBytes::toCharValueString() const
{
	const char* pData = data.get();
	std::string retStr;
	for(unsigned int i = 0; i < length; i++)
	{
		retStr = retStr + std::to_string(static_cast<int>(*(pData + i)));
		if(i != length - 1)
		{
			retStr = retStr + ",";
		}
	}
	return retStr;
}
std::string utl::RawBytes::toString() const
{
	const char* pData = data.get();
	std::string retStr;
	retStr.reserve(length + 1);
	for(unsigned int i = 0; i < length; i++)
	{
		retStr = retStr + (*(pData + i));
	}
	return retStr;
}
std::string utl::RawBytes::toHexString() const
{
	std::string output;
	const unsigned char* input = reinterpret_cast<const unsigned char*>(static_cast<char*>(data.get()));
	CryptoPP::StringSource ss(input, length, true,
	                          new CryptoPP::HexEncoder(
	                              new CryptoPP::StringSink(output)));
	return output;
}
unsigned char* utl::RawBytes::get()
{
	return reinterpret_cast<unsigned char*>(static_cast<char*>(data.get()));
}

const unsigned char* utl::RawBytes::getConst() const
{
	return reinterpret_cast<unsigned char*>(static_cast<char*>(data.get()));
}

int utl::RawBytes::size() const
{
	return length;
}

void utl::RawBytes::restrictPacketExtraction(unsigned int byteCount)
{
	largestPacketSize = byteCount;
}

unsigned int utl::RawBytes::getRestrictedPacketSize()
{
	return largestPacketSize;
}

utl::RandomSalt::RandomSalt(int length):
	RawBytes(std::make_unique<char[]>(length), length)
{
	unsigned char* dest = reinterpret_cast<unsigned char*>(static_cast<char*>(data.get()));
	rng.GenerateBlock(dest, length);
}

utl::RawBytes::RawBytes(const std::u32string& str):
	RawBytes(std::make_unique<char[]>(str.size()*4), str.size()*4)
{
	for(unsigned int i = 0; i < str.size(); i++)
	{
		char* input = (data.get() + i * 4);
		char32_t c = str.at(i);
		*input = c & 0xff;
		*(input + 1) = (c >> 8) & 0xff;
		*(input + 2) = (c >> 16) & 0xff;
		*(input + 3) = (c >> 24) & 0xff;
	}
}
utl::RawBytes::RawBytes(const std::string& str):
	RawBytes(std::make_unique<char[]>(str.size()), str.size())
{
	for(unsigned int i = 0; i < str.size(); i++)
	{
		*(data.get() + i) = str.at(i);
	}
}
utl::RawBytes::RawBytes(const char* str):
	RawBytes(std::make_unique<char[]>(strlen(str)), strlen(str))
{
	strcpy(data.get(), str);
}

void utl::hash_whirlpool(utl::RawBytes& data)
{
	utl::RawBytes output(std::make_unique<char[]>(64), 64);

	CryptoPP::ArraySink sink(output.get(), output.size());

	CryptoPP::StringSource ss(data.get(), data.size(), true,
	                          new CryptoPP::HashFilter(
	                              whirlpool, new CryptoPP::Redirector(sink)));
	data = output;
}

sf::Packet& operator<< (sf::Packet& packet, const utl::RawBytes& bytes)
{
	packet << sf::Int32(bytes.size());
	for(const unsigned char* it = bytes.getConst(); it < (bytes.getConst() + bytes.size());  it++)
	{
		packet << *it;
	}
	return packet;
}

sf::Packet& operator<< (sf::Packet& packet, const utl::RandomSalt& bytes)
{
	packet << sf::Int32(bytes.size());
	for(const unsigned char* it = bytes.getConst(); it < (bytes.getConst() + bytes.size());  it++)
	{
		packet << *it;
	}
	return packet;
}

sf::Packet& operator>> (sf::Packet& packet, utl::RawBytes& bytes)
{
	unsigned int length;
	if(!(packet >> length) || (bytes.getRestrictedPacketSize() && (length > bytes.getRestrictedPacketSize())))
	{
		//TODO give warning
		return packet;
	}
	
	utl::RawBytes data(std::make_unique<char[]>(length), length);
	for(unsigned char* it = data.get(); it < (data.get() + bytes.size());  it++)
	{
		unsigned char currentByte;
		if(!(packet >> currentByte))
		{
			//TODO give warning
			return packet;
		}
		*it = currentByte;
	}
	bytes = data;
	return packet;
}