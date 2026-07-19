#pragma once

#include "hash.hpp"

#include <vector>
#include <cstdint>

namespace hashes {
    struct SHA256 : public Hash<64, 32> {
        bytes::ByteArr<32> compute(const std::vector<std::uint8_t>& input) const;
    };
}
