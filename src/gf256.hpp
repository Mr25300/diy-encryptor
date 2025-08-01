#pragma once

#include <cstdint>
#include <string>
#include <bitset>

class GF256 {
    uint8_t value;

    static constexpr uint8_t REDUCTION_POLYNOMIAL = 0b00011011;

    static constexpr uint8_t gfMultiply(uint8_t a, uint8_t b) { // Understand how this works
        uint8_t result = 0;

        for (int i = 0; i < 8; i++) {
            if (b & 1) result ^= a;

            bool hasHighBit = a & 0b10000000;

            a <<= 1;

            if (hasHighBit) a ^= REDUCTION_POLYNOMIAL;

            b >>= 1;
        }

        return result;
    }

    static constexpr char hexDigits[] = "0123456789ABCDEF";

public:
    constexpr GF256(uint8_t v = 0) : value(v) {}

    constexpr uint8_t get() const {
        return value;
    }

    constexpr GF256 operator-() const {
        return *this;
    }

    // constexpr GF256 inv() const;

    constexpr GF256 operator+(GF256 other) const {
        return GF256(value ^ other.value);
    }

    constexpr GF256& operator+=(GF256 other) {
        value ^= other.value;

        return *this;
    }

    constexpr GF256 operator-(GF256 other) const {
        return *this + other;
    }

    constexpr GF256& operator-=(GF256 other) {
        *this += other;

        return *this;
    }

    constexpr GF256 operator*(GF256 other) const {
        return GF256(gfMultiply(value, other.value));
    }

    constexpr GF256& operator*=(GF256 other) {
        value = gfMultiply(value, other.value);

        return *this;
    }

    // constexpr GF256 operator/(GF256 other) const;

    // constexpr void operator/=(GF256 other);

    std::string asHex() const {
        return {'0', 'x', hexDigits[value >> 4], hexDigits[value & 0b1111]};
    }

    std::string asBinary() const {
        return "0b" + std::bitset<8>(value).to_string();
    }

    std::string asPoly() const {
        std::string result = "";

        for (int i = 7; i >= 0; i--) {
            if (value & (1 << i)) {
                if (result.size() > 0) {
                    result += " + ";
                }

                if (i == 0) {
                    result += "1";
                } else if (i == 1) {
                    result += "x";
                } else {
                    result += "x^" + std::to_string(i);
                }
            }
        }

        return result;
    }

    std::string asChar() const {
        return {static_cast<char>(value)};
    }
};