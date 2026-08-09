#pragma once

#include <cstdint>
#include <cstddef>
#include <bit>
#include <ostream>

namespace math {
    struct GF256LongDivisionResult {
        std::uint8_t quotient;
        std::uint8_t remainder;
    };

    class GF256 {
        static constexpr std::uint16_t irreduciblePolynomial{0b100011011};

        static constexpr std::size_t getDegree(std::uint16_t n) {
            if (n == 0) return 0;

            return std::bit_width(n) - 1;
        }

        static constexpr GF256LongDivisionResult longDivide(std::uint16_t dividend, std::uint16_t divisor) {
            if (dividend == 0 || divisor == 0) return {0, 0};

            std::size_t dividendDeg{getDegree(dividend)};
            const std::size_t divisorDeg{getDegree(divisor)};

            std::uint8_t quotient{0};

            // Dividend must not be 0, otherwise the degree may match the degree of a divisor of 1, resulting in an infinite loop
            while (dividend > 0 && dividendDeg >= divisorDeg) {
                std::size_t degDiff{dividendDeg - divisorDeg};

                dividend ^= static_cast<std::uint16_t>(divisor << degDiff);
                quotient ^= static_cast<std::uint8_t>(1U << degDiff);

                dividendDeg = getDegree(dividend);
            }

            return {quotient, static_cast<std::uint8_t>(dividend)};
        }

        static constexpr std::uint8_t multiply(std::uint16_t multiplier, std::uint8_t multiplicand) {
            std::uint16_t product{0};

            for (std::size_t i{}; i < 8; ++i) {
                if (multiplicand == 0) break;
                if (multiplicand & 1) product ^= multiplier;

                multiplier <<= 1;
                multiplicand >>= 1;
            }

            return longDivide(product, irreduciblePolynomial).remainder;
        }

        std::uint8_t value{};

    public:
        constexpr GF256() = default;
        constexpr GF256(std::uint8_t v) : value{v} {}

        constexpr std::uint8_t getValue() const { return value; }

        constexpr GF256 operator-() const { return *this; }

        constexpr GF256 inv() const { // Utilizes euclidean algorithm while keeping track of coefficients and ensuring that the Bezout identity is satisfied for the remainder at each step
            if (value == 0 || value == 1) return *this;

            std::uint16_t prevRemainder{irreduciblePolynomial}; // r0
            std::uint8_t remainder{value}; // r1
            std::uint8_t prevCoeff{0}; // n_0, where m_0 * a + n_0 * b = r_0 (a = value, b = irreducible polynomial)
            std::uint8_t coeff{1}; // n_1, where m_1 * a + n_1 * b = r_1 (a = value, b = irreducible polynomial)

            while (remainder != 0) {
                GF256LongDivisionResult result{longDivide(prevRemainder, remainder)};
                std::uint8_t newCoeff{prevCoeff};
                newCoeff ^= multiply(coeff, result.quotient);

                // (r0, r1) = (r1, r0 % r1)
                prevRemainder = remainder; // r0 = r1
                remainder = result.remainder; // r1 = r0 % r1 = r0 - r1 * floor(r0 / r1)

                prevCoeff = coeff; // c0 = c1
                coeff = newCoeff; // c1 = c0 - c1 * floor(r0 / r1)
            }

            return prevCoeff; // n_0, where m_0 * a + n_0 * b = 1 --> gcd(a, b)
        }

        constexpr GF256 operator+(GF256 num) const { return value ^ num.value; }

        constexpr GF256& operator+=(GF256 num) {
            value ^= num.value;

            return *this;
        }

        constexpr GF256 operator-(GF256 num) const { return value ^ num.value; }

        constexpr GF256& operator-=(GF256 num) {
            value ^= num.value;

            return *this;
        }

        constexpr GF256 operator*(GF256 num) const { return multiply(value, num.value); }

        constexpr GF256& operator*=(GF256 num) {
            value = multiply(value, num.value);

            return *this;
        }

        constexpr GF256 operator/(GF256 num) const { return multiply(value, num.inv().value); }

        constexpr GF256& operator/=(GF256 num) {
            value = multiply(value, num.inv().value);

            return *this;
        }

        constexpr bool operator==(GF256 num) const { return value == num.value; }
        constexpr bool operator!=(GF256 num) const { return value != num.value; }

        friend std::ostream& operator<<(std::ostream& stream, GF256 num);
    };

    std::ostream& gfHex(std::ostream& os);
    std::ostream& gfBin(std::ostream& os);
    std::ostream& gfChr(std::ostream& os);
    std::ostream& gfInt(std::ostream& os);
    std::ostream& gfPoly(std::ostream& os);
}
