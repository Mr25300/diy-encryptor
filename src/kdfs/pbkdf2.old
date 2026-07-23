#include "pbkdf2.hpp"

// bytes::ByteVec kdfs::PBKDF2::compute(const bytes::ByteVec& key, const bytes::ByteVec& salt, std::size_t dKeySize) const {
//     if (dKeySize == 0) {
//         throw std::invalid_argument("Derived key size must be greater than 0.");
//     }
//
//     std::size_t hLen{prf.outputSize()};
//
//     // Since indexing variable is only 4 bytes
//     if (dKeySize / hLen > std::numeric_limits<std::uint32_t>::max()) {
//         throw std::length_error("Requested dervied key size is too large.");
//     }
//
//     std::size_t len{(dKeySize + hLen - 1) / hLen};
//     std::size_t remainder{dKeySize - (len - 1) * hLen};
//
//     std::vector<bytes::ByteVec> tVals(len);
//
//     bytes::ByteVec saltAndI{salt};
//     saltAndI.resize(salt.size() + 4); // TODO: See if reserve is better here
//
//     std::vector<std::uint8_t> uVal;
//     uVal.reserve(hLen);
//
//     for (std::uint32_t i{1}; i <= len; ++i) {
//         bytes::ByteVec& tVal{tVals[i - 1]};
//         tVal.assign(hLen, 0);
//
//         for (int j{}; j < iters; ++j) {
//             if (j == 0) {
//                 for (int k{}; k < 4; ++k) {
//                     saltAndI[salt.size() + k] = static_cast<std::uint8_t>(i >> (3 - k) * 8);
//                 }
//
//                 uVal = prf.compute(key, saltAndI);
//
//             } else {
//                 uVal = prf.compute(key, uVal);
//             }
//
//             for (int k{}; k < hLen; ++k) {
//                 tVal[k] ^= uVal[k];
//             }
//         }
//     }
//
//     bytes::ByteVec dKey(dKeySize);
//
//     for (std::size_t i{}; i < len - 1; ++i) {
//         dKey.insert(dKey.end(), tVals[i].begin(), tVals[i].end());
//     }
//
//     dKey.insert(dKey.end(), tVals[len - 1].begin(), tVals[len - 1].begin() + remainder);
//
//     return dKey;
// }

template <std::size_t HLen, std::size_t DKeySize>
bytes::ByteArr<DKeySize> kdfs::PBKDF2<HLen, DKeySize>::compute(const bytes::ByteVec& key, const bytes::ByteVec& salt) const {
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
