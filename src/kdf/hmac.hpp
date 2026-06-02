#pragma once

#include <cstddef>

#include <hash/hash.hpp>
#include "prf.hpp"

class HMAC : public PRF {
private:
    const Hash& hash;

    static constexpr std::uint8_t ipad{0x36};
    static constexpr std::uint8_t opad{0x5C};

    static std::vector<std::uint8_t> xorBytes(const std::vector<std::uint8_t>& bytes, std::uint8_t pad);
    static void appendBytes(std::vector<std::uint8_t>& bytes1, const std::vector<std::uint8_t>& bytes2);

public:
    constexpr HMAC(const Hash& hash) : hash{hash} {}

    std::vector<std::uint8_t> compute(const std::vector<std::uint8_t>& key, const std::vector<std::uint8_t>& text) const;

    size_t outputSize() const {
        return hash.outputSize();
    }
};
