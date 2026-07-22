#pragma once

#include "prf.hpp"

#include <hashes/hash.hpp>
#include <utils/bytes.hpp>

#include <cstdint>

namespace prfs {
    template <std::size_t BlockSize, std::size_t OutputSize>
    class HMAC : public PRF<OutputSize> {
    private:
        const hashes::Hash<BlockSize, OutputSize>& hash;

        static constexpr std::uint8_t ipad{0x36};
        static constexpr std::uint8_t opad{0x5C};

    public:
        constexpr HMAC(const hashes::Hash<BlockSize, OutputSize>& hash) : hash{hash} {}

        bytes::ByteArr<OutputSize> compute(const bytes::ByteVec& key, const bytes::ByteVec& text) const;
    };
}
