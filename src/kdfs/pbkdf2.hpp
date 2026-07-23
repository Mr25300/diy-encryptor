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

        bytes::ByteArr<DKeySize> compute(const bytes::ByteVec& key, const bytes::ByteVec& salt) const {
            // if (DKeySize == 0) {
            //     throw std::invalid_argument("Derived key size must be greater than 0.");
            // }

            // Since indexing variable is only 4 bytes
            // if (DKeySize / HLen > std::numeric_limits<std::uint32_t>::max()) {
            //     throw std::length_error("Requested dervied key size is too large.");
            // }

            std::size_t len{(DKeySize + HLen - 1) / HLen};
            std::size_t remainder{DKeySize - (len - 1) * HLen};

            std::vector<bytes::ByteArr<HLen>> tVals(len);

            bytes::ByteVec saltAndI{salt};
            saltAndI.resize(salt.size() + 4); // TODO: See if reserve is better here

            bytes::ByteArr<HLen> uVal;

            for (std::uint32_t i{1}; i <= len; ++i) {
                bytes::ByteArr<HLen>& tVal{tVals[i - 1]};

                for (std::size_t j{}; j < iters; ++j) {
                    if (j == 0) {
                        for (std::size_t k{}; k < 4; ++k) {
                            saltAndI[salt.size() + k] = static_cast<std::uint8_t>(i >> (3 - k) * 8);
                        }

                        uVal = prf.compute(key, saltAndI);

                    } else {
                        uVal = prf.compute(key, bytes::ByteVec(uVal.begin(), uVal.end()));
                    }

                    bytes::xorBytes(tVal, uVal);
                }
            }

            bytes::ByteArr<DKeySize> dKey;

            for (std::size_t i{}; i < len - 1; ++i) {
                std::copy(tVals[i].begin(), tVals[i].end(), dKey.begin() + HLen * i);
            }

            std::copy(tVals[len - 1].begin(), tVals[len - 1].begin() + remainder, dKey.end() - remainder); // TODO: Check this

            return dKey;
        }
    };
}
