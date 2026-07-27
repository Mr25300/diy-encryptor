#pragma once

#include <cstdint>
#include <cstddef>
#include <ostream>

namespace math {
    struct GF256LongDivisionResult {
        std::uint8_t quotient;
        std::uint8_t remainder;
    };

    enum class GFFormat {
        Hex,
        Binary,
        Char,
        Int,
        Poly
    };

    class GF256 {
        static constexpr std::uint16_t irreduciblePolynomial{0b100011011};

        static constexpr std::size_t gfGetDegree(std::uint16_t n) {
            if (n == 0) {
                return -1;
            } else if (n == irreduciblePolynomial) {
                return 8;
            }

            std::size_t d{0};

            for (d = 15; d >= 0; d--) {
                if (n & (1 << d)) {
                    return d;
                }
            }

            return d;
        }

        static constexpr GF256LongDivisionResult gfLongDivide(std::uint16_t dividend, std::uint16_t divisor) {
            if (divisor == 0) {
                return {0, 0};
            }

            std::size_t dividendDeg{gfGetDegree(dividend)};
            const std::size_t divisorDeg{gfGetDegree(divisor)};

            std::uint8_t quotient{0};

            while (dividendDeg >= divisorDeg) {
                std::size_t degDiff{dividendDeg - divisorDeg};

                if (degDiff >= 16) break;

                dividend ^= static_cast<std::uint16_t>(divisor << degDiff);
                quotient ^= static_cast<std::uint8_t>(1U << degDiff);

                dividendDeg = gfGetDegree(dividend);
            }

            return {quotient, static_cast<std::uint8_t>(dividend)};
        }

        static constexpr std::uint8_t gfMultiply(std::uint16_t multiplier, std::uint8_t multiplicand) {
            std::uint16_t product{0};

            for (std::size_t i{}; i < 8; ++i) {
                if (multiplicand == 0) break;
                if (multiplicand & 1) product ^= multiplier;

                multiplier <<= 1;
                multiplicand >>= 1;
            }

            return gfLongDivide(product, irreduciblePolynomial).remainder;
        }

        static constexpr char hexDigits[]{"0123456789ABCDEF"};

        std::uint8_t value{};

    public:
        constexpr GF256() = default;
        constexpr GF256(std::uint8_t v) : value{v} {}

        constexpr std::uint8_t getValue() const { return value; }

        constexpr GF256 operator-() const { return *this; }

        constexpr GF256 inv() const { // Utilizes euclidean algorithm while keeping track of coefficients and ensuring that the Bezout identity is satisfied for the remainder at each step
            if (value == 0) return 0;
            if (value == 1) return 1;

            std::uint16_t prevRemainder{irreduciblePolynomial}; // r0
            std::uint8_t remainder{value}; // r1
            std::uint8_t prevCoeff{0}; // n_0, where m_0 * a + n_0 * b = r_0 (a = value, b = irreducible polynomial)
            std::uint8_t coeff{1}; // n_1, where m_1 * a + n_1 * b = r_1 (a = value, b = irreducible polynomial)

            while (remainder != 0) {
                GF256LongDivisionResult result{gfLongDivide(prevRemainder, remainder)};
                std::uint8_t newCoeff{prevCoeff};
                newCoeff ^= gfMultiply(coeff, result.quotient);

                // (r0, r1) = (r1, r0 % r1)
                prevRemainder = remainder; // r0 = r1
                remainder = result.remainder; // r1 = r0 % r1 = r0 - r1 * floor(r0 / r1)

                prevCoeff = coeff; // c0 = c1
                coeff = newCoeff; // c1 = c0 - c1 * floor(r0 / r1)
            }

            return prevCoeff; // n_0, where m_0 * a + n_0 * b = 1 --> gcd(a, b)
        }

        constexpr GF256 operator+(GF256 other) const {
            return GF256{static_cast<std::uint8_t>(value ^ other.value)};
        }

        constexpr GF256& operator+=(GF256 other) {
            value ^= other.value;

            return *this;
        }

        constexpr GF256 operator-(GF256 other) const {
            return GF256{static_cast<std::uint8_t>(value ^ other.value)};
        }

        constexpr GF256& operator-=(GF256 other) {
            value ^= other.value;

            return *this;
        }

        constexpr GF256 operator*(GF256 other) const {
            return GF256{gfMultiply(value, other.value)};
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

        constexpr bool operator==(GF256 other) const { return value == other.value; }
        constexpr bool operator!=(GF256 other) const { return value != other.value; }

        // TODO: Remove below and make operator<< use state to manage format
        void print(std::ostream& stream, GFFormat format = GFFormat::Poly) const;
        friend std::ostream& operator<<(std::ostream& stream, GF256 number);
    };
}
