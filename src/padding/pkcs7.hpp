#include <cstdint>
#include <vector>

#include "padding_scheme.hpp"

namespace padding {
    template <std::size_t BlockSize>
    struct PKCS7 : PaddingScheme<BlockSize> {
        void pad(std::vector<std::uint8_t>& input);
        bool unpad(std::vector<std::uint8_t>& input);
    };
}
