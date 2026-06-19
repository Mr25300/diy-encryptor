#pragma once

#include "hash.hpp"

#include <math/utils.hpp>

#include <vector>
#include <cstdint>

namespace hashes {
    struct SHA256 : public Hash {
        std::vector<std::uint8_t> compute(const std::vector<std::uint8_t>& input) const;

        std::size_t blockSize() const { return 64; }
        std::size_t outputSize() const { return 32; }
    };
}
