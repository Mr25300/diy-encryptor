#include "substitution_box.hpp"

#include <array>

namespace ciphers::aes {
    constexpr uint8_t constantVector{0b01100011};

    constexpr math::GF256 getTransformedByte(math::GF256 value) {
        uint8_t byte{value.inv().value};
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
            map[i] = getTransformedByte(static_cast<std::uint8_t>(i));
        }

        return map;
    }()};

    constexpr std::array<math::GF256, 256> sboxInv{[] {
        std::array<math::GF256, 256> map{};

        for (std::size_t i{}; i < 256; ++i) {
            map[sbox[i].value] = static_cast<math::GF256>(i);
        }

        return map;
    }()};

    math::GF256 SubstitutionBox::sub(math::GF256 byte) const { return sbox[byte.value]; }
    math::GF256 SubstitutionBox::subInv(math::GF256 byte) const { return sboxInv[byte.value]; }

    std::ostream& operator<<(std::ostream& stream, const SubstitutionBox& subBox) {
        for (std::size_t i{}; i < 16; ++i) {
            if (i > 0) stream << '\n';

            for (std::size_t j{}; j < 16; ++j) {
                stream << sbox[(i << 4) ^ j];

                if (i != 15 || j != 15) {
                    stream << ',' << ' ';
                }
            }
        }

        return stream;
    }
}
