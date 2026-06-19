#pragma once

#include "prf.hpp"

#include <hashes/hash.hpp>

#include <cstddef>

namespace prfs {
    class HMAC : public PRF {
    private:
        const hashes::Hash& hash;

        static constexpr std::uint8_t ipad{0x36};
        static constexpr std::uint8_t opad{0x5C};

    public:
        HMAC(const hashes::Hash& hash) : hash{hash} {}

        std::vector<std::uint8_t> compute(const std::vector<std::uint8_t>& key, const std::vector<std::uint8_t>& text) const;

        size_t outputSize() const { return hash.outputSize(); }
    };
}
