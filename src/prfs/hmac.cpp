#include "hmac.hpp"

// namespace {
//     std::vector<std::uint8_t> xorBytes(const std::vector<std::uint8_t>& bytes, std::uint8_t pad) {
//         std::vector<std::uint8_t> result(bytes.size());
//
//         for (std::size_t i{}; i < bytes.size(); ++i) {
//             result[i] = bytes[i] ^ pad;
//         }
//
//         return result;
//     }
//
//     void appendBytes(std::vector<std::uint8_t>& bytes1, const std::vector<std::uint8_t>& bytes2) {
//         bytes1.reserve(bytes1.size() + bytes2.size());
//         bytes1.insert(bytes1.end(), bytes2.begin(), bytes2.end());
//     }
// }

template <std::size_t BlockSize, std::size_t OutputSize>
bytes::ByteArr<OutputSize> prfs::HMAC<BlockSize, OutputSize>::compute(const bytes::ByteVec& key, const bytes::ByteVec& text) const {
    bytes::ByteArr<BlockSize> workingKey;

    if (key.size() > BlockSize) {
        bytes::ByteArr<OutputSize> hashedKey{hash.compute(key)};

        std::copy(hashedKey.begin(), hashedKey.end(), workingKey.begin());
        std::fill(workingKey.begin() + OutputSize, workingKey.end(), 0);

    } else {
        std::copy(key.begin(), key.end(), workingKey.begin());
        std::fill(workingKey.begin() + key.size(), workingKey.end(), 0);
    }

    bytes::ByteArr<OutputSize> innerXor{bytes::getXorBytes(workingKey, ipad)};
    bytes::ByteVec inner{bytes::getAppendBytes(innerXor, text)};
    bytes::ByteArr<OutputSize> innerHash{hash.compute(inner)};

    bytes::ByteArr<OutputSize> outerXor{bytes::getXorBytes(workingKey, opad)};
    bytes::ByteVec outer{bytes::getAppendBytes(outerXor, innerHash)};

    return hash.compute(outer);
}
