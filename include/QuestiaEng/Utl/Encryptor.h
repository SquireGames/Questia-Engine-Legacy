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
	virtual ~RawBytes() = default;
	void printValue() const;
	
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
class StringSalt : public RawBytes
{
public:
	StringSalt(const std::u32string& str);
	StringSalt(const std::string& str);
	StringSalt(const char* str);
};

}

#endif // ENCRYPTOR_H
