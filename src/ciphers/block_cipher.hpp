#include <cstdint>
#include <array>

namespace ciphers {
    template <std::size_t Size>
    using Block = std::array<std::uint8_t, Size>;

    template <std::size_t BlockSize>
    struct BlockCipher {
        virtual void encrypt(Block<BlockSize>& block) const = 0;
        virtual void decrypt(Block<BlockSize>& block) const = 0;
    };
}
