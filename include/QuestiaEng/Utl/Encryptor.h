#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <memory>

namespace utl
{

class RawBytes
{
public:
	RawBytes(std::unique_ptr<char[]> data, int length);
	RawBytes(const utl::RawBytes&);
	RawBytes(utl::RawBytes&&) = default;
	RawBytes& operator= (const utl::RawBytes&);
	RawBytes& operator= (utl::RawBytes&&) = default;
	virtual ~RawBytes() = default;
	
	RawBytes(const std::u32string& str);
	RawBytes(const std::string& str);
	RawBytes(const char* str);
	
	void printValue() const;
	void printAsStr() const;
	void printHex() const;
	
	unsigned char* get(); 
	int size() const;
	
	const RawBytes operator+ (const RawBytes& rhs);

protected:
	std::unique_ptr<char[]> data;
	int length;
};

class RandomSalt : public RawBytes
{
public:
	RandomSalt(int length);
};

//hashes
void hash_whirlpool(RawBytes& data);

}

#endif // ENCRYPTOR_H
