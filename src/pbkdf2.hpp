#include <vector>
#include <array>
#include <cmath>

class HashFunction {
public:
    virtual std::vector<uint8_t> hash(const std::vector<uint8_t>& input) const = 0;
    virtual size_t blockSize() const = 0;
    virtual size_t outputLen() const = 0;
};

class SHA256 : public HashFunction {
private:
    std::tuple<std::array<uint32_t, 8>, std::array<uint32_t, 64>> initValues;

    constexpr std::array<unsigned int, 64> get_primes() {
        std::array<uint32_t, 64> primes{};

        size_t count = 0;
        int current = 2;

        while (count < 64) {
            bool is_prime = true;

            for (int i = 2; i * i <= current; i++) {
                if (current % i == 0) {
                    is_prime = false;

                    break;
                }
            }

            if (is_prime) {
                primes[count] = current;
                count += 1;
            }

            current += 1;
        }

        return primes;
    }

    constexpr uint32_t getFractionalBits(double x) {
        double frac = x - std::floor(x);

        return static_cast<uint32_t>(frac * (1ULL << 32));
    }

    constexpr std::tuple<std::array<uint32_t, 8>, std::array<uint32_t, 64>> getInitValues() {
        std::array<uint32_t, 64> primes = this->get_primes();
        std::array<uint32_t, 8> h{};
        std::array<uint32_t, 64> k{};

        for (size_t i = 0; i < 8; i++) {
            h[i] = this->getFractionalBits(std::sqrt(primes[i]));
        }

        for (size_t i = 0; i < 64; i++) {
            k[i] = this->getFractionalBits(std::cbrt(primes[i]));
        }

        return std::tuple<std::array<uint32_t, 8>, std::array<uint32_t, 64>>{h, k};
    }

public:
    constexpr SHA256() : initValues(getInitValues()) {}

    std::vector<uint8_t> hash(const std::vector<uint8_t>& input) const {
        // begin with the original message of length L bits
        // append a single '1' bit
        // append K '0' bits, where K is the minimum number >= 0 such that (L + 1 + K + 64) is a multiple of 512
        // append L as a 64-bit big-endian integer, making the total post-processed length a multiple of 512 bits
        // such that the bits in the message are: <original message of length L> 1 <K zeros> <L as 64 bit integer> , (the number of bits will be a multiple of 512)

        uint64_t length = input.size() * 8; // Number of bits in initial input

        std::vector<uint8_t> words;

        for (size_t i = 0; i < input.size() << 2; i++) { // Loop through words (64 bit segments)
            uint8_t byte1 = input[2 * i];
            uint8_t byte2 = input[2 * i + 1];

            words.emplace_back()
        }

        bytes.emplace_back(1 << 7); // Append a '1' bit followed by 7 '0' bits to complete the byte

        // Chunks must be a multiple of 64 bytes
        // Last 8 bytes must be the length

        size_t chunkSize = 64;
        size_t lengthSize = 8; // Size in bytes of the appended length
        size_t remainderBytes = (bytes.size() + lengthSize) % 64;
        size_t remainingBytes = remainderBytes == 0 ? 0 : chunkSize - remainderBytes; // Remaining 0x00 bytes to add

        for (size_t i = 0; i < remainingBytes; i++) {
            bytes.emplace_back(0);
        }

        // Now add the length to bytes here

        return;
    }

    size_t blockSize() const {
        return 0;
    }

    size_t outputLen() const {
        return 32;
    }
};

class PRF {
public:
    virtual void compute() const = 0;
    virtual size_t outputSize() const = 0;
};

class HMAC : public PRF {
public:
    void compute() {
        return;
    }

    size_t outputSize() {
        return 0;
    }
};

class PBKDF2 {
private:
    const std::vector<uint8_t>& salt;
    int iterations;

public:
    PBKDF2(const PRF& prf, const std::vector<uint8_t>& salt, int iterations) : salt(salt), iterations(iterations) {}

    std::vector<uint8_t> pseudorandomFunction(const std::vector<uint8_t>& input) {
        return {};
    }

    std::vector<uint8_t> deriveKey(const std::vector<uint8_t>& password, size_t keyLength) {
        // if (keyLength > (2 ^ 32 - 1) * hLen) throw error (hLen = length of pseudorandom output)

        return {};
    }
};