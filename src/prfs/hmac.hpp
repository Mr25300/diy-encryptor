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

        static const std::uint8_t ipad{0x36};
        static const std::uint8_t opad{0x5C};

    public:
        constexpr HMAC(const hashes::Hash<BlockSize, OutputSize>& hash) : hash{hash} {}

        bytes::ByteArr<OutputSize> compute(const bytes::ByteVec& key, const bytes::ByteVec& text) const {
            bytes::ByteArr<BlockSize> workingKey;

            if (key.size() > BlockSize) {
                bytes::ByteArr<OutputSize> hashedKey{hash.compute(key)};

                std::copy(hashedKey.begin(), hashedKey.end(), workingKey.begin());
                std::fill(workingKey.begin() + OutputSize, workingKey.end(), 0);

            } else {
                std::copy(key.begin(), key.end(), workingKey.begin());
                std::fill(workingKey.begin() + key.size(), workingKey.end(), 0);
            }

            bytes::ByteArr<BlockSize> innerXor{bytes::getXorBytes(workingKey, ipad)};
            bytes::ByteVec inner{bytes::getAppendBytes(innerXor, text)};
            bytes::ByteArr<OutputSize> innerHash{hash.compute(inner)};

            bytes::ByteArr<BlockSize> outerXor{bytes::getXorBytes(workingKey, opad)};
            bytes::ByteVec outer{bytes::getAppendBytes(outerXor, innerHash)};

            return hash.compute(outer);
        }
    };
}
