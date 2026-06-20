#pragma once

#include <cstdint>
#include <vector>
#include <stdexcept>

namespace bytes {
    using Bytes = std::vector<std::uint8_t>;

    inline void xorBytes(Bytes& bytes1, const Bytes& bytes2) {
        if (bytes1.size() != bytes2.size())
            throw std::invalid_argument("Cannot XOR byte blocks of different lengths.");

        for (std::size_t i{}; i < bytes1.size(); ++i) {
            bytes1[i] ^= bytes2[i];
        }
    }

    inline void xorBytes(Bytes& bytes, std::uint8_t byte) {
        for (std::size_t i{}; i < bytes.size(); ++i) {
            bytes[i] ^= byte;
        }
    }

    inline void appendBytes(Bytes& bytes1, const Bytes& bytes2) {
        bytes1.reserve(bytes1.size() + bytes2.size());
        bytes1.insert(bytes1.end(), bytes2.begin(), bytes2.end());
    }
}
