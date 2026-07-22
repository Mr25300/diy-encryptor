#pragma once

#include <utils/bytes.hpp>
#include <prfs/prf.hpp>

#include <cstddef>
#include <limits>

namespace kdfs {
    template <std::size_t HLen, std::size_t DKeySize>
    class PBKDF2 {
        // TODO: Make sure this is correct
        static_assert(DKeySize != 0, "Derived key size must be greater than 0.");
        static_assert(
            DKeySize / HLen <= std::numeric_limits<std::uint32_t>::max(),
            "Derived key size is too large."
        );

        const prfs::PRF<HLen>& prf;
        std::size_t iters;

    public:
        constexpr PBKDF2(const prfs::PRF<HLen>& prf, std::size_t iterations) : prf(prf), iters{iterations} {}

        bytes::ByteArr<DKeySize> compute(const bytes::ByteVec& key, const bytes::ByteVec& salt) const;
    };
}
