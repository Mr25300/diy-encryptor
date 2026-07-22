#pragma once

#include <utils/bytes.hpp>

#include <cstddef>

namespace padding {
    template <std::size_t BlockSize>
    struct PaddingScheme {
        virtual void pad(bytes::ByteVec& input) const = 0;
        virtual bool unpad(bytes::ByteVec& input) const = 0;
    };
}
