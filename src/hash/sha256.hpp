#include <vector>
#include <array>
#include <cstdint>
#include <span>
#include <algorithm>

#include <math/utils.hpp>
#include "hash.hpp"

namespace {
    static std::uint32_t rightrot(std::uint32_t word, std::uint32_t amount) {
        if (amount == 0) return word;

        amount &= 31;

        return word >> amount | word << (32 - amount);
    }

    static std::uint32_t ch(std::uint32_t x, std::uint32_t y, std::uint32_t z) {
        return (x & y) ^ (~x & z);
    }

    static std::uint32_t maj(std::uint32_t x, std::uint32_t y, std::uint32_t z) {
        return (x & y) ^ (x & z) ^ (y & z);
    }

    static std::uint32_t sum0(std::uint32_t x) {
        return rightrot(x, 2) ^ rightrot(x, 13) ^ rightrot(x, 22);
    }

    static std::uint32_t sum1(std::uint32_t x) {
        return rightrot(x, 6) ^ rightrot(x, 11) ^ rightrot(x, 25);
    }

    static std::uint32_t sigma0(std::uint32_t x) {
        return rightrot(x, 7) ^ rightrot(x, 18) ^ (x >> 3);
    }

    static std::uint32_t sigma1(std::uint32_t x) {
        return rightrot(x, 17) ^ rightrot(x, 19) ^ (x >> 10);
    }
}

class SHA256 : public Hash {
private:
    std::array<uint32_t, 8> h;
    std::array<uint32_t, 64> k;

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
    constexpr SHA256() : h{}, k{} {
        constexpr std::array<unsigned int, 64> primes{SHA256::getPrimes()};

        for (std::size_t i{}; i < 8; ++i) {
            this->h[i] = SHA256::getFractionalBits(utils::constexprSqrt(primes[i]));
        }

        for (std::size_t i{}; i < 64; ++i) {
            this->k[i] = SHA256::getFractionalBits(utils::constexprCbrt(primes[i]));
        }
    }

    std::vector<std::uint8_t> compute(const std::vector<std::uint8_t>& input) const {
        std::array<std::uint32_t, 8> hCopy{this->h};
        const std::array<std::uint32_t, 64>& kRef = this->k;

        std::vector<std::array<std::uint32_t, 16>> chunks;
        std::size_t inputWords{(input.size() >> 2) + 1}; // Divide by 4 and add an extra 1

        for (std::size_t i{}; i < inputWords; ++i) { // Loop through words (32 bit segments)
            std::size_t chunkIndex{i / 16};

            if (chunkIndex >= chunks.size()) chunks.emplace_back(); // Triggers zero-initialization

            std::array<std::uint32_t, 16>& chunk{chunks[chunkIndex]};
            std::uint32_t word{0};

            for (size_t j{}; j < 4; ++j) {
                std::size_t byteIndex{i * 4 + j};

                if (byteIndex < input.size()) {
                    word |= static_cast<std::uint32_t>(input[i * 4 + j]) << (3 - j) * 8;
                } else if (byteIndex == input.size()) {
                    word |= 1 << ((4 - j) * 8 - 1); // Append single '1' bit
                } else {
                    break;
                }
            }

            chunk[i % 16] = word;
        }

        if (inputWords % 16 > 16 - 2) chunks.emplace_back(); // 2 words needed for 64 bit size

        std::array<std::uint32_t, 16>& lastChunk{chunks[chunks.size() - 1]};
        std::int64_t inputBits{static_cast<std::int64_t>(input.size()) * 8};

        lastChunk[14] = static_cast<std::int32_t>(inputBits >> 32);
        lastChunk[15] = static_cast<std::int32_t>(inputBits);

        for (const std::array<std::uint32_t, 16>& chunk : chunks) {
            std::array<std::uint32_t, 64> w{}; // Message schedule
            std::span<std::uint32_t, 16> firstPart(w.data(), 16);
            std::ranges::copy(chunk, firstPart.begin());

            for (std::size_t i{16}; i < 64; ++i) {
                w[i] = w[i - 16] + sigma0(w[i - 15]) + w[i - 7] + sigma1(w[i - 2]);
            }

            std::array<std::uint32_t, 8> hVars{hCopy};

            for (std::size_t i{}; i < 64; ++i) {
                uint32_t temp1{hVars[7] + sum1(hVars[4]) + ch(hVars[4], hVars[5], hVars[6])
                    + kRef[i] + w[i]};
                uint32_t temp2{sum0(hVars[0]) + maj(hVars[0], hVars[1], hVars[2])};

                hVars[7] = hVars[6];
                hVars[6] = hVars[5];
                hVars[5] = hVars[4];
                hVars[4] = hVars[3] + temp1;
                hVars[3] = hVars[2];
                hVars[2] = hVars[1];
                hVars[1] = hVars[0];
                hVars[0] = temp1 + temp2;
            }

            for (int i{}; i < 8; ++i) {
                hCopy[i] += hVars[i];
            }
        }

        std::vector<std::uint8_t> output(32);

        for (int i{}; i < 8; ++i) {
            std::uint32_t word{hCopy[i]};

            for (int j{}; j < 4; ++j) {
                output[i * 4 + j] = static_cast<std::uint8_t>(word >> (3 - j) * 8);
            }
        }

        return output;
    }

    std::size_t blockSize() const {
        return 64;
    }

    std::size_t outputSize() const {
        return 32;
    }
};
