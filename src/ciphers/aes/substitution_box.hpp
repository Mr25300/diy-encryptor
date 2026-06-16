#pragma once

#include <array>
#include <ostream>

#include <math/gf256.hpp>

class SubstitutionBox {
    std::array<math::GF256, 256> map;
    std::array<math::GF256, 256> mapInv;

    static constexpr uint8_t constantVector{0b01100011};

    static constexpr math::GF256 getTransformedByte(math::GF256 value) {
        uint8_t byte{value.inv().value};
        uint8_t result = 0;

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

public:
    constexpr SubstitutionBox() : map{}, mapInv{} {
        for (std::size_t i{}; i < 256; ++i) {
            math::GF256 transformedByte{getTransformedByte(i)};

            map[i] = transformedByte;
            mapInv[transformedByte.value] = i;
        }
    }

    math::GF256 sub(math::GF256 byte) const { return map[byte.value]; }
    math::GF256 subInv(math::GF256 byte) const { return mapInv[byte.value]; }

    friend std::ostream& operator<<(std::ostream& stream, const SubstitutionBox& subBox) {
        for (std::size_t i{}; i < 16; ++i) {
            if (i > 0) stream << '\n';

            for (std::size_t j{}; j < 16; ++j) {
                stream << subBox.map[(i << 4) ^ j];

                if (i != 15 || j != 15) {
                    stream << ',' << ' ';
                }
            }
        }

        return stream;
    }
};
