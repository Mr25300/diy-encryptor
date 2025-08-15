#pragma once

#include <array>
#include <string>
#include <sstream>

#include "gf256.hpp"

class SubstitutionBox {
    std::array<GF256, 256> map;
    std::array<GF256, 256> mapInv;

    static constexpr uint8_t constantVector = 0b01100011;

    static constexpr GF256 getTransformedByte(GF256 value) {
        GF256 invValue = value.inv();

        uint8_t byte = invValue.get();
        uint8_t result = 0;

        // Affine matrix transformation on bits to produce result
        for (int i = 0; i < 8; i++) {
            uint8_t bit = (byte >> i) & 1;

            for (int j = 4; j < 8; j++) {
                bit ^= (byte >> ((i + j) % 8)) & 1;
            }

            result ^= bit << i; // Bit i equivalent to itself XORed with the 4 bits 3 after itself
        }

        return result ^ constantVector;
    }

public:
    constexpr SubstitutionBox() : map{}, mapInv{} {
        for (int i = 0; i < 256; i++) {
            GF256 transformedByte = getTransformedByte(i);

            map[i] = transformedByte;
            mapInv[transformedByte.get()] = i;
        }
    }

    GF256 sub(GF256 val) const {
        return map[val.get()];
    }

    GF256 subInv(GF256 val) const {
        return mapInv[val.get()];
    }

    std::string toString(GFFormat format = GFFormat::Hex) const {
        std::ostringstream oss;

        for (int i = 0; i < 16; i++) {
            if (i > 0) oss << '\n';

            for (int j = 0; j < 16; j++) {
                if (j > 0 || (j == 14 && i == 15)) oss << ", ";

                GF256 outputByte = map[(i << 4) ^ j];

                oss << outputByte.toString(format);
            }
        }

        return oss.str();
    }
};