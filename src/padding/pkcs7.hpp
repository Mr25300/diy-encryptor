#pragma once

#include "padding_scheme.hpp"

#include <utils/bytes.hpp>

namespace padding {
    template <std::size_t BlockSize>
    struct PKCS7 : PaddingScheme<BlockSize> {
        void pad(bytes::ByteVec& input) const;
        bool unpad(bytes::ByteVec& input) const;
    };
}
