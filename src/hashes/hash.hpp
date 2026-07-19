#pragma once

#include <utils/bytes.hpp>

#include <cstddef>

namespace hashes {
    template <std::size_t BlockSize, std::size_t OutputSize>
    class Hash {
    public:
        virtual bytes::ByteArr<OutputSize> compute(const bytes::ByteVec& input) const = 0;

        std::size_t blockSize() const { return BlockSize; }
        std::size_t outputSize() const { return OutputSize; }
    };
}
