#pragma once

#include <utils/bytes.hpp>

#include <cstddef>

namespace padding {
    class PaddingScheme {
    protected:
        const std::size_t blockSize;

    public:
        constexpr PaddingScheme(std::size_t blockSize) : blockSize{blockSize} {}

        virtual void pad(bytes::ByteVec& input) const = 0;
        virtual bool unpad(bytes::ByteVec& input) const = 0;
    };
}
