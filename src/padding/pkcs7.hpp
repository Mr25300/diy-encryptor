#pragma once

#include "padding_scheme.hpp"

#include <cstdint>
#include <vector>

namespace padding {
    template <std::size_t BlockSize>
    struct PKCS7 : PaddingScheme<BlockSize> {
        void pad(std::vector<std::uint8_t>& input) const;
        bool unpad(std::vector<std::uint8_t>& input) const;
    };
}
