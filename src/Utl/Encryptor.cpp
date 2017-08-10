#include "QuestiaEng/Utl/Encryptor.h"

#include <iostream>
#include "Cryptopp/osrng.h"

static CryptoPP::NonblockingRng rng;

void utl::Salt::printValue() const
{
	const unsigned char* pData = data.get();
	for(int i = 0; i < length; i++)
	{
		std::cout << static_cast<int>(*(pData + i));
		if(i != length - 1)
		{
			std::cout << ",";
		}
		
	}
}

utl::Salt utl::generateSalt(int bytes)
{
	utl::Salt salt {std::make_unique<unsigned char[]>(bytes), bytes};
	rng.GenerateBlock(salt.data.get(), bytes);
	return salt;
}
