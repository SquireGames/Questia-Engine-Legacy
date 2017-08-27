#include "UnitTest++/UnitTest++.h"

#include "QuestiaEng/Utl/Encryptor.h"

//TEST class RawBytes

TEST(TestRawBytes_ctor_cStr)
{
	char testString[] = "Arbitrary String";
	utl::RawBytes stringBytes(testString);
	CHECK_EQUAL(sizeof(testString) - 1, stringBytes.size());
}
