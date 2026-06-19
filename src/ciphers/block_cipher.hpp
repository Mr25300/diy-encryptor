#pragma once

#include "block.hpp"

namespace ciphers {
    template <std::size_t BlockSize>
    struct BlockCipher {
        virtual void encrypt(Block<BlockSize>& block) const = 0;
        virtual void decrypt(Block<BlockSize>& block) const = 0;
    };
}
