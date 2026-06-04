#include <limits>
#include <stdexcept>

#include "pbkdf2.hpp"

std::vector<uint8_t> PBKDF2::compute(const std::vector<uint8_t>& key, const std::vector<uint8_t>& salt, std::size_t dKeySize) const {
    if (dKeySize == 0) {
        throw std::invalid_argument("Derived key size must be greater than 0.");
    }

    std::size_t hLen{prf.outputSize()};

    // Since indexing variable is only 4 bytes
    if (dKeySize / hLen > std::numeric_limits<std::uint32_t>::max()) {
        throw std::length_error("Requested dervied key size is too large.");
    }

    std::size_t len{(dKeySize + hLen - 1) / hLen};
    std::size_t remainder{dKeySize - (len - 1) * hLen};

    std::vector<std::vector<std::uint8_t>> tVals(len);

    std::vector<std::uint8_t> saltAndI{salt};
    saltAndI.resize(salt.size() + 4);

    std::vector<std::uint8_t> uVal;
    uVal.reserve(hLen);

    for (std::uint32_t i{1}; i <= len; ++i) {
        std::vector<std::uint8_t>& tVal{tVals[i - 1]};
        tVal.assign(hLen, 0);

        for (int j{}; j < iterations; ++j) {
            if (j == 0) {
                for (int k{}; k < 4; ++k) {
                    saltAndI[salt.size() + k] = static_cast<std::uint8_t>(i >> (3 - k) * 8);
                }

                uVal = prf.compute(key, saltAndI);

            } else {
                uVal = prf.compute(key, uVal);
            }

            for (int k{}; k < hLen; ++k) {
                tVal[k] ^= uVal[k];
            }
        }
    }

    std::vector<std::uint8_t> dKey;
    dKey.reserve(dKeySize);

    for (std::size_t i{}; i < len - 1; ++i) {
        dKey.insert(dKey.end(), tVals[i].begin(), tVals[i].end());
    }

    dKey.insert(dKey.end(), tVals[len - 1].begin(), tVals[len - 1].begin() + remainder);

    return dKey;
}
