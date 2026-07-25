#pragma once

#include <utils/bytes.hpp>

#include <cstddef>

namespace hashes {
    template <std::size_t BlockSize, std::size_t OutputSize>
    struct Hash {
        static_assert(BlockSize > OutputSize, "Block size must be larger than output size.");

        virtual bytes::ByteArr<OutputSize> compute(const bytes::ByteVec& input) const = 0;
    };
}
