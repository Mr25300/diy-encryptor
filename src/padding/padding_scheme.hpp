#include <cstdint>
#include <vector>

namespace padding {
    template <std::size_t BlockSize>
    struct PaddingScheme {
        virtual void pad(std::vector<std::uint8_t>& input) = 0;
        virtual bool unpad(std::vector<std::uint8_t>& input) = 0;
    };
}
