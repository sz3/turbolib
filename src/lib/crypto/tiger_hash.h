/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "crypto_base/tiger.h"

namespace turbo {
class tiger_hash
{
public:
	std::string operator()(const std::string& key)
	{
		struct tiger_ctx state;
		tiger_init(&state, 1);
		tiger_update(&state, reinterpret_cast<const uint8_t*>(key.data()), key.size());

		std::string out;
		out.resize(TIGER_SZ_DIGEST);
		tiger_end(&state, reinterpret_cast<uint8_t*>(&out[0]), TIGER_SZ_DIGEST);
		return out;
	}
};
} // namespace turbo
