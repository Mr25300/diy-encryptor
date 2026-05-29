#pragma once

#include <vector>
#include <cstdint>

#include "prf.hpp"
#include "pbkdf.hpp"

class PBKDF2 { // Should implement PBKDF
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
