#include "substitution_box.hpp"

#include <array>

constexpr uint8_t constantVector{0b01100011};

constexpr math::GF256 getTransformedByte(math::GF256 value) {
    uint8_t byte{value.inv().getValue()};
    uint8_t result{};

    // Affine matrix transformation on bits to produce result
    for (std::size_t i{}; i < 8; ++i) {
        uint8_t bit{static_cast<uint8_t>((byte >> i) & 1)};

        for (int j{4}; j < 8; ++j) {
            bit ^= (byte >> ((i + j) % 8)) & 1;
        }

        result ^= bit << i; // Bit i equivalent to itself XORed with the 4 bits 3 after itself
    }

    return result ^ constantVector;
}

constexpr std::array<math::GF256, 256> sbox{[] {
    std::array<math::GF256, 256> map{};

    for (std::size_t i{}; i < 256; ++i) {
        map[i] = getTransformedByte(i);
    }

    return map;
}()};

constexpr std::array<math::GF256, 256> sboxInv{[] {
    std::array<math::GF256, 256> map{};

    for (std::size_t i{}; i < 256; ++i) {
        map[sbox[i].getValue()] = i;
    }

    return map;
}()};

namespace ciphers::rij {
    math::GF256 SubstitutionBox::sub(math::GF256 byte) const { return sbox[byte.getValue()]; }
    math::GF256 SubstitutionBox::subInv(math::GF256 byte) const { return sboxInv[byte.getValue()]; }
}
