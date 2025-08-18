#pragma once

#include <cstdint>
#include <string>
#include <bitset>
#include <ostream>

struct LongDivisionResult {
    uint8_t quotient;
    uint8_t remainder;
};

enum class GFFormat {
    Hex,
    Binary,
    Char,
    Int,
    Poly
};

class GF256 {
    uint8_t value;

    static constexpr uint16_t irreduciblePolynomial = 0b100011011;

    static constexpr int gfGetDegree(uint16_t n) {
        if (n == 0) {
            return -1;
        } else if (n == irreduciblePolynomial) {
            return 8;
        }

        int d = 0;

        for (d = 15; d >= 0; d--) {
            if (n & (1 << d)) {
                return d;
            }
        }

        return d;
    }

    static constexpr LongDivisionResult gfLongDivide(uint16_t dividend, uint16_t divisor) {
        if (divisor == 0) {
            return {0, 0};
        }

        int dividendDeg = gfGetDegree(dividend);
        const int divisorDeg = gfGetDegree(divisor);

        uint8_t quotient = 0;

        while (dividendDeg >= divisorDeg) {
            int degDiff = dividendDeg - divisorDeg;

            dividend ^= divisor << degDiff;
            quotient ^= 1 << degDiff;

            dividendDeg = gfGetDegree(dividend);
        }

        return {quotient, static_cast<uint8_t>(dividend)};
    }

    static constexpr uint8_t gfMultiply(uint16_t multiplier, uint8_t multiplicand) {
        uint16_t product = 0;

        for (int i = 0; i < 8; i++) {
            if (multiplicand == 0) break;
            if (multiplicand & 1) product ^= multiplier;

            multiplier <<= 1;
            multiplicand >>= 1;
        }

        return gfLongDivide(product, irreduciblePolynomial).remainder;
    }

    // static constexpr uint8_t gfMultiply(uint16_t a, uint8_t b) {
    //     uint8_t result = 0;

    //     for (int i = 0; i < 8; i++) {
    //         if (b & 1) result ^= a;

    //         bool hasHighBit = a & 0b10000000;

    //         a <<= 1;

    //         if (hasHighBit) a ^= irreduciblePolynomial;

    //         b >>= 1;
    //     }

    //     return result;
    // }

    static constexpr char hexDigits[] = "0123456789ABCDEF";

public:
    constexpr GF256(uint8_t v = 0) : value(v) {}

    constexpr uint8_t get() const {
        return value;
    }

    constexpr GF256 operator-() const {
        return *this;
    }

    constexpr GF256 inv() const { // Utilizes euclidean algorithm while keeping track of coefficients and ensuring that the Bezout identity is satisfied for the remainder at each step
        if (value == 0) return 0;
        if (value == 1) return 1;

        uint16_t prevRemainder = irreduciblePolynomial; // r0
        uint8_t remainder = value; // r1
        uint8_t prevCoeff = 0; // n_0, where m_0 * a + n_0 * b = r_0 (a = value, b = irreducible polynomial)
        uint8_t coeff = 1; // n_1, where m_1 * a + n_1 * b = r_1 (a = value, b = irreducible polynomial)

        while (remainder != 0) {
            LongDivisionResult result = gfLongDivide(prevRemainder, remainder);
            uint8_t newCoeff = prevCoeff ^ gfMultiply(coeff, result.quotient);

            // (r0, r1) = (r1, r0 % r1)
            prevRemainder = remainder; // r0 = r1
            remainder = result.remainder; // r1 = r0 % r1 = r0 - r1 * floor(r0 / r1)

            prevCoeff = coeff; // c0 = c1
            coeff = newCoeff; // c1 = c0 - c1 * floor(r0 / r1)
        }

        return prevCoeff; // n_0, where m_0 * a + n_0 * b = 1 --> gcd(a, b)
    }

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

    constexpr GF256 operator/(GF256 other) const {
        return *this * other.inv();
    }

    constexpr GF256& operator/=(GF256 other) {
        *this *= other.inv();

        return *this;
    }

    void print(std::ostream& stream, GFFormat format = GFFormat::Poly) const {
        switch(format) {
            case GFFormat::Hex: {
                stream << "0x" << hexDigits[value >> 4] << hexDigits[value & 0b1111];

                break;
            }
            case GFFormat::Binary: {
                stream << "0b" << std::bitset<8>(value).to_string();

                break;
            }
            case GFFormat::Char: {
                stream << static_cast<char>(value);

                break;
            }
            case GFFormat::Int: {
                stream << std::to_string(value);

                break;
            }
            case GFFormat::Poly: {
                bool firstPlaced = false;

                for (int i = 7; i >= 0; i--) {
                    if (value & (1 << i)) {
                        if (firstPlaced) {
                            stream << " + ";
                        }

                        firstPlaced = true;

                        if (i == 0) {
                            stream << "1";
                        } else if (i == 1) {
                            stream << "x";
                        } else {
                            stream << "x^" << std::to_string(i);
                        }
                    }
                }

                break;
            }
            default:
                throw std::invalid_argument("Unknown format enum value");
        }
    }

    friend std::ostream& operator<<(std::ostream& stream, GF256 number) {
        number.print(stream);

        return stream;
    }
};