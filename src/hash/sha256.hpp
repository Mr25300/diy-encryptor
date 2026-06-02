#pragma once

#include <vector>
#include <array>
#include <cstdint>

#include <math/utils.hpp>
#include "hash.hpp"

class SHA256 : public Hash {
private:
    std::array<uint32_t, 8> hVals;
    std::array<uint32_t, 64> kVals;

    static constexpr std::array<unsigned int, 64> getPrimes() {
        std::array<uint32_t, 64> primes{};

        size_t count = 0;
        int current = 2;

        while (count < 64) {
            bool isPrime = true;

            for (int i = 2; i * i <= current; i++) {
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

    static constexpr uint32_t getFractionalBits(double x) {
        double frac{x - static_cast<std::uint64_t>(x)};

        return static_cast<uint32_t>(frac * (1ULL << 32));
    }

public:
    constexpr SHA256() {
        constexpr std::array<unsigned int, 64> primes{SHA256::getPrimes()};

        for (std::size_t i{}; i < 8; ++i) {
            this->hVals[i] = SHA256::getFractionalBits(utils::constexprSqrt(primes[i]));
        }

        for (std::size_t i{}; i < 64; ++i) {
            this->kVals[i] = SHA256::getFractionalBits(utils::constexprCbrt(primes[i]));
        }
    }

    std::vector<std::uint8_t> compute(const std::vector<std::uint8_t>& input) const;

    std::size_t blockSize() const { return 64; }
    std::size_t outputSize() const { return 32; }
};
