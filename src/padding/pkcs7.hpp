#pragma once

#include "padding_scheme.hpp"

#include <utils/bytes.hpp>

namespace padding {
    struct PKCS7 : PaddingScheme { // TODO: Make sure private methods stay private
        void pad(bytes::ByteVec& input) const;
        bool unpad(bytes::ByteVec& input) const;
    };
}
