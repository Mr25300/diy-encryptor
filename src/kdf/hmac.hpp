#pragma once

#include <cstddef>

#include <hash/hash.hpp>
#include "prf.hpp"

class HMAC : public PRF {
private:
    const Hash& hash;

    static constexpr std::uint8_t ipad{0x36};
    static constexpr std::uint8_t opad{0x5C};

    static std::vector<std::uint8_t> xorBytes(const std::vector<std::uint8_t>& bytes, std::uint8_t pad) {
        std::vector<std::uint8_t> result(bytes.size());

        for (std::size_t i{}; i < bytes.size(); ++i) {
            result[i] = bytes[i] ^ pad;
        }

        return result;
    }

    static void appendBytes(std::vector<std::uint8_t>& bytes1, const std::vector<std::uint8_t>& bytes2) {
        bytes1.reserve(bytes1.size() + bytes2.size());
        bytes1.insert(bytes1.end(), bytes2.begin(), bytes2.end());
    }

public:
    HMAC(const Hash& hash) : hash(hash) {}

    std::vector<std::uint8_t> compute(const std::vector<std::uint8_t>& key, const std::vector<std::uint8_t>& text) const {
        std::size_t blockSize{this->hash.blockSize()};
        std::vector<std::uint8_t> workingKey;

        if (key.size() > blockSize) {
            workingKey = this->hash.compute(key);
        } else {
            workingKey = key;
        }

        if (workingKey.size() < blockSize) {
            workingKey.resize(blockSize, 0x00);
        }

        std::vector<std::uint8_t> inner{xorBytes(workingKey, ipad)};
        appendBytes(inner, text);

        std::vector<std::uint8_t> innerHash{this->hash.compute(inner)};
        std::vector<std::uint8_t> outer{xorBytes(workingKey, opad)};
        appendBytes(outer, innerHash);

        return this->hash.compute(outer);
    }

    size_t outputSize() const {
        return hash.outputSize();
    }
};
