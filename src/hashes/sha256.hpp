#pragma once

#include "hash.hpp"

#include <vector>
#include <cstdint>

namespace hashes {
    struct SHA256 : public Hash {
        bytes::ByteVec compute(const std::vector<std::uint8_t>& input) const;

        std::size_t blockSize() const { return 64; }
        std::size_t outputSize() const { return 32; }
    };
}
