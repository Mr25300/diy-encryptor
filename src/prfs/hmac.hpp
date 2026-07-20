#pragma once

#include "prf.hpp"

#include <hashes/hash.hpp>
#include <utils/bytes.hpp>

#include <cstddef>

namespace prfs {
    class HMAC : public PRF {
    private:
        const hashes::Hash& hash;

        static constexpr std::uint8_t ipad{0x36};
        static constexpr std::uint8_t opad{0x5C};

    public:
        HMAC(const hashes::Hash& hash) : hash{hash} {}

        bytes::ByteVec compute(const bytes::ByteVec& key, const bytes::ByteVec& text) const;

        std::size_t outputSize() { return hash.outputSize(); }
    };
}
