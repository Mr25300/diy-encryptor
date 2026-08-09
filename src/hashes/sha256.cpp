#include "sha256.hpp"

#include <array>

namespace {
    struct SHA256Values {
        std::array<std::uint32_t, 8> h;
        std::array<std::uint32_t, 64> k;
    };

    template <std::size_t N>
    constexpr std::array<unsigned int, N> getPrimes() {
        std::array<unsigned int, N> primes{};

        size_t count{};
        unsigned int current{2};

        while (count < N) {
            bool isPrime{true};

            for (std::size_t i{2}; i * i <= current; ++i) {
                if (current % i == 0) {
                    isPrime = false;

                    break;
                }
            }

            if (isPrime) {
                primes[count] = current;
                ++count;
            }

            ++current;
        }

        return primes;
    }

    constexpr double sqrt(double num) {
        double curr{num};
        double prev{};

        // Mathematical minimum for accuracy to 32nd fractional binary digit up to 64th prime: 8
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
        double curr{num};
        double prev{};

        // Mathematical minimum for accuracy to 32nd fractional binary digit up to 64th prime: 14
        for (std::size_t i{}; i < 20; ++i) {
            prev = curr;
            curr = (2.0 * curr + num / (curr * curr)) / 3.0;

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

constexpr SHA256Values vals{[] {
    SHA256Values vals{};

    std::array<unsigned int, 64> primes{getPrimes<64>()};

    for (std::size_t i{}; i < 8; ++i) {
        vals.h[i] = getFractionalBits(sqrt(primes[i]));
    }

    for (std::size_t i{}; i < 64; ++i) {
        vals.k[i] = getFractionalBits(cbrt(primes[i]));
    }

    return vals;
}()};

namespace {
    std::uint32_t rightRot(std::uint32_t word, std::uint32_t amount) {
        amount &= 31;

        return word >> amount | word << (32 - amount);
    }

    std::uint32_t ch(std::uint32_t x, std::uint32_t y, std::uint32_t z) {
        return (x & y) ^ (~x & z);
    }

    std::uint32_t maj(std::uint32_t x, std::uint32_t y, std::uint32_t z) {
        return (x & y) ^ (x & z) ^ (y & z);
    }

    std::uint32_t sum0(std::uint32_t x) {
        return rightRot(x, 2) ^ rightRot(x, 13) ^ rightRot(x, 22);
    }

    std::uint32_t sum1(std::uint32_t x) {
        return rightRot(x, 6) ^ rightRot(x, 11) ^ rightRot(x, 25);
    }

    std::uint32_t sigma0(std::uint32_t x) {
        return rightRot(x, 7) ^ rightRot(x, 18) ^ (x >> 3);
    }

    std::uint32_t sigma1(std::uint32_t x) {
        return rightRot(x, 17) ^ rightRot(x, 19) ^ (x >> 10);
    }
}

namespace hashes {
    bytes::ByteArr<32> hashes::SHA256::compute(bytes::ConstByteView input) const {
        std::array<std::uint32_t, 8> hCopy{vals.h};

        std::uint64_t inputBits{static_cast<std::uint64_t>(input.size()) * 8};

        // If input.size() is a multiple of 4, we add 1 word since we need another word to store the appended 1 bit
        // Otherwise, we add 1 word since there are remaining bytes erased by flooring, and there will be at least 1 byte left in the word to append a 1 bit
        std::size_t wordCount{input.size() / 4 + 1};

        // If wordCount is a multiple of 16, we add 1 chunk since we need another chunk to store the last 64 bit bit length
        // Otherwise, we add 1 chunk since there are remaining words erased by flooring
        std::size_t chunkCount{wordCount / 16 + 1};
        std::size_t wordMod{wordCount % 16};
        if (wordMod > 14) ++chunkCount;

        for (std::size_t i{}; i < chunkCount; ++i) {
            std::array<std::uint32_t, 64> w{}; // Message schedule
            std::span<std::uint32_t, 16> chunk{w.data(), 16};

            for (std::size_t j{}; j < 16; ++j) {
                for (std::size_t k{}; k < 4; ++k) {
                    std::size_t byteIndex{i * 64 + j * 4 + k};

                    if (byteIndex < input.size()) {
                        chunk[j] |= static_cast<std::uint32_t>(input[byteIndex]) << (24 - k * 8);
                    } else if (byteIndex == input.size()) {
                        chunk[j] |= 1 << (31 - k * 8); // Append single 1 bit
                    } else {
                        break;
                    }
                }
            }

            if (i == chunkCount - 1) {
                chunk[14] = static_cast<std::uint32_t>(inputBits >> 32);
                chunk[15] = static_cast<std::uint32_t>(inputBits);
            }

            for (std::size_t j{16}; j < 64; ++j) {
                w[j] = w[j - 16] + sigma0(w[j - 15]) + w[j - 7] + sigma1(w[j - 2]);
            }

            std::array<std::uint32_t, 8> hVars{hCopy};

            for (std::size_t j{}; j < 64; ++j) {
                std::uint32_t temp1{hVars[7] + sum1(hVars[4]) + ch(hVars[4], hVars[5], hVars[6])
                    + vals.k[j] + w[j]};
                std::uint32_t temp2{sum0(hVars[0]) + maj(hVars[0], hVars[1], hVars[2])};

                hVars[7] = hVars[6];
                hVars[6] = hVars[5];
                hVars[5] = hVars[4];
                hVars[4] = hVars[3] + temp1;
                hVars[3] = hVars[2];
                hVars[2] = hVars[1];
                hVars[1] = hVars[0];
                hVars[0] = temp1 + temp2;
            }

            for (std::size_t j{}; j < 8; ++j) {
                hCopy[j] += hVars[j];
            }
        }

        bytes::ByteArr<32> output;

        for (int i{}; i < 8; ++i) {
            std::uint32_t word{hCopy[i]};

            for (int j{}; j < 4; ++j) {
                output[i * 4 + j] = static_cast<std::uint8_t>(word >> (3 - j) * 8);
            }
        }

        return output;
    }
}
