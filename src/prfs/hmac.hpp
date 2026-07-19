#pragma once

#include "prf.hpp"

#include <hashes/hash.hpp>
#include <utils/bytes.hpp>

#include <cstddef>

namespace prfs {
    template <std::size_t OutputSize>
    class HMAC : public PRF<OutputSize> {
    private:
        const hashes::Hash& hash<[ANY], OutputSize>;

        static constexpr std::uint8_t ipad{0x36};
        static constexpr std::uint8_t opad{0x5C};

    public:
        HMAC(const hashes::Hash& hash) : hash{hash} {}

        bytes::ByteArr<OutputSize> compute(const bytes::ByteVec& key, const bytes::ByteVec& text) const;
    };
}
