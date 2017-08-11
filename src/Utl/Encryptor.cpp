#include "QuestiaEng/Utl/Encryptor.h"

#include <iostream>

#include "Cryptopp/osrng.h"
#include "Cryptopp/whrlpool.h"

static CryptoPP::NonblockingRng rng;

utl::RawBytes::RawBytes(std::unique_ptr<char[]> data, int length):
	data(std::move(data)), length(length)
{
}
utl::RawBytes::RawBytes(const utl::RawBytes& other):
	data(std::make_unique<char[]>(other.length)), length(other.length)
{
	char* thisDataIt = data.get();
	const char* otherDataIt = other.data.get();
	for(int i = 0; i < length; i++)
	{
		*thisDataIt = *otherDataIt;
		otherDataIt++;
		thisDataIt++;
	}
}
const utl::RawBytes utl::RawBytes::operator+(const RawBytes& rhs)
{
	int newLength = length + rhs.length;
	utl::RawBytes newBytes{std::make_unique<char[]>(newLength), newLength};
	char* thisDataIt = data.get();
	char* newDataIt = newBytes.data.get();
	char* rhsDataIt = rhs.data.get();
	for(int i = 0; i < length; i++)
	{
		*newDataIt = *thisDataIt;
		newDataIt++;
		thisDataIt++;
	}
	for(int i = 0; i < rhs.length; i++)
	{
		*newDataIt = *rhsDataIt;
		newDataIt++;
		rhsDataIt++;
	}
	return newBytes;
}
void utl::RawBytes::printValue() const
{
	const char* pData = data.get();
	for(int i = 0; i < length; i++)
	{
		std::cout << static_cast<int>(*(pData + i));
		if(i != length - 1)
		{
			std::cout << ",";
		}

	}
}

utl::RandomSalt::RandomSalt(int length):
	RawBytes(std::make_unique<char[]>(length), length)
{
	unsigned char* dest = reinterpret_cast<unsigned char*>(static_cast<char*>(data.get()));
	rng.GenerateBlock(dest, length);
}

utl::StringSalt::StringSalt(const std::u32string& str):
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
utl::StringSalt::StringSalt(const std::string& str):
	RawBytes(std::make_unique<char[]>(str.size()), str.size())
{
	for(unsigned int i = 0; i < str.size(); i++)
	{
		*(data.get() + i) = str.at(i);
	}
}
utl::StringSalt::StringSalt(const char* str):
	RawBytes(std::make_unique<char[]>(strlen(str)), strlen(str))
{
	strcpy(data.get(), str);
}
