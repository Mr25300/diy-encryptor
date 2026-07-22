#pragma once

#include <utils/bytes.hpp>

#include <cstddef>

namespace hashes {
    template <std::size_t BlockSize, std::size_t OutputSize>
    struct Hash {
        virtual bytes::ByteArr<OutputSize> compute(const bytes::ByteVec& input) const = 0;
    };
}
