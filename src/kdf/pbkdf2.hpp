#pragma once

#include <vector>
#include <cstdint>

#include <prf/prf.hpp>

// template <std::size_t keySize>
class PBKDF2 {
private:
    const PRF& prf;
    int iterations;

public:
    constexpr PBKDF2(const PRF& prf, int iterations) : prf(prf), iterations{iterations} {}

    std::vector<uint8_t> compute(const std::vector<uint8_t>& key, const std::vector<uint8_t>& salt, std::size_t keySize) const;
};
