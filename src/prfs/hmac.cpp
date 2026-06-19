#include "hmac.hpp"

#include <cstdint>

namespace {
    std::vector<std::uint8_t> xorBytes(const std::vector<std::uint8_t>& bytes, std::uint8_t pad) {
        std::vector<std::uint8_t> result(bytes.size());

        for (std::size_t i{}; i < bytes.size(); ++i) {
            result[i] = bytes[i] ^ pad;
        }

        return result;
    }

    void appendBytes(std::vector<std::uint8_t>& bytes1, const std::vector<std::uint8_t>& bytes2) {
        bytes1.reserve(bytes1.size() + bytes2.size());
        bytes1.insert(bytes1.end(), bytes2.begin(), bytes2.end());
    }
}

std::vector<std::uint8_t> prfs::HMAC::compute(const std::vector<std::uint8_t>& key, const std::vector<std::uint8_t>& text) const {
    std::size_t blockSize{hash.blockSize()};
    std::vector<std::uint8_t> workingKey;

    if (key.size() > blockSize) {
        workingKey = hash.compute(key);
    } else {
        workingKey = key;
    }

    if (workingKey.size() < blockSize) {
        workingKey.resize(blockSize, 0x00);
    }

    std::vector<std::uint8_t> inner{xorBytes(workingKey, ipad)};
    appendBytes(inner, text);

    std::vector<std::uint8_t> innerHash{hash.compute(inner)};
    std::vector<std::uint8_t> outer{xorBytes(workingKey, opad)};
    appendBytes(outer, innerHash);

    return hash.compute(outer);
}
