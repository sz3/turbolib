#include <string>

// custom base64 encoding that preserves lexicographical ordering.
namespace base64
{
	std::string encode(const std::string& to_encode);
	std::string encode(unsigned char const* to_encode, unsigned int len);
	std::string decode(const std::string& to_decode);

	template <typename Integer>
	inline std::string encode_bin(const Integer& number)
	{
		// TODO: endian awareness?
		return encode(reinterpret_cast<unsigned char const*>(&number), sizeof(Integer));
	}

	template <typename Integer>
	inline Integer decode_as(const std::string& to_decode)
	{
		std::string decoded = decode(to_decode);
		if (decoded.size() < sizeof(Integer))
			return 0;
		// TODO: endian awareness?
		return *reinterpret_cast<const Integer*>(decoded.data());
	}
}

