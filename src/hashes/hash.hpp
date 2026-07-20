#pragma once

#include <utils/bytes.hpp>

#include <cstddef>

namespace hashes {
    class Hash {
    public:
        virtual bytes::ByteVec compute(const bytes::ByteVec& input) const = 0;

        virtual std::size_t blockSize() const = 0;
        virtual std::size_t outputSize() const = 0;
    };
}
