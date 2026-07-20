#pragma once

#include <utils/bytes.hpp>
#include <prfs/prf.hpp>

#include <cstddef>

namespace kdfs {
    class PBKDF2 {
    private:
        const prfs::PRF& prf;
        std::size_t iters;

    public:
        constexpr PBKDF2(const prfs::PRF& prf, std::size_t iterations) : prf(prf), iters{iterations} {}

        bytes::ByteVec compute(const bytes::ByteVec& key, const bytes::ByteVec& salt, std::size_t dKeySize) const;
    };
}
