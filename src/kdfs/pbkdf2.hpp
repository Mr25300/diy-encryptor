#pragma once

#include <prfs/prf.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace kdfs {
    // template <std::size_t keySize>
    class PBKDF2 {
    private:
        const prfs::PRF& prf;
        std::size_t iters;

    public:
        constexpr PBKDF2(const prfs::PRF& prf, std::size_t iterations) : prf(prf), iters{iterations} {}

        std::vector<std::uint8_t> compute(const std::vector<std::uint8_t>& key, const std::vector<std::uint8_t>& salt, std::size_t keySize) const;
    };
}
