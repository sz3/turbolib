#include <string>

// custom base64 encoding that preserves lexicographical ordering.
namespace base64
{
	std::string encode(const std::string& to_encode);
	std::string encode(unsigned char const* to_encode, unsigned int len);
	std::string decode(const std::string& to_decode);
}

