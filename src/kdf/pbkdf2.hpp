#pragma once

#include <vector>
#include <cstdint>

#include "prf.hpp"

class PBKDF2 {
private:
    const PRF& prf;
    int iters;

public:
    constexpr PBKDF2(const PRF& prf, int iterations) : prf(prf), iters{iterations} {}

    std::vector<uint8_t> compute(const std::vector<uint8_t>& key, const std::vector<uint8_t>& salt);
};
