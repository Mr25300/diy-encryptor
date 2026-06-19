#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

namespace math::utils {
    template <std::size_t N>
    static constexpr std::array<unsigned int, N> getPrimes() {
        std::array<unsigned int, N> primes{};

        size_t count{};
        unsigned int current{2};

        while (count < N) {
            bool isPrime = true;

            for (std::size_t i{2}; i * i <= current; i++) {
                if (current % i == 0) {
                    isPrime = false;

                    break;
                }
            }

            if (isPrime) {
                primes[count] = current;
                count += 1;
            }

            current += 1;
        }

        return primes;
    }

    constexpr double sqrt(double num) {
        double curr{num};
        double prev{0};

        for (std::size_t i{}; i < 15; ++i) {
            prev = curr;
            curr = 0.5 * (curr + num / curr);

            if (curr == prev) {
                break;
            }
        }

        return curr;
    }

    constexpr double cbrt(double num) {
        double absNum{num < 0.0 ? -num : num};
        double curr{num}; // TODO: Test with negative num, if it breaks replace num with absNum here
        double prev{0};

        for (std::size_t i{}; i < 20; ++i) {
            prev = curr;
            curr = (2.0 * curr + absNum / (curr * curr)) / 3.0;

            if (curr == prev) {
                break;
            }
        }

        return curr;
    }

    constexpr uint32_t getFractionalBits(double x) {
        double frac{x - static_cast<std::uint64_t>(x)};

        return static_cast<uint32_t>(frac * (1ULL << 32));
    }
}
