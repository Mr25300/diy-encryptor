#pragma once

#include <utils/bytes.hpp>

namespace ciphers {
    template <std::size_t BlockSize>
    struct BlockCipher {
        virtual void encrypt(bytes::ByteArr<BlockSize>& block) const = 0;
        virtual void decrypt(bytes::ByteArr<BlockSize>& block) const = 0;
    };
}
