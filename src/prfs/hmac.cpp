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

bytes::ByteVec prfs::HMAC::compute(const bytes::ByteVec& key, const bytes::ByteVec& text) const {
    std::size_t blockSize{hash.blockSize()};
    bytes::ByteVec workingKey;

    if (key.size() > blockSize) {
        workingKey = hash.compute(key);
    } else {
        workingKey = key;
    }

    if (workingKey.size() < blockSize) {
        workingKey.resize(blockSize, 0x00);
    }

    // TODO: Make sure static_cast to const type is correct
    bytes::ByteVec inner{bytes::xorBytes(static_cast<const bytes::ByteVec>(workingKey), ipad)};
    bytes::appendBytes(inner, text);

    bytes::ByteVec innerHash{hash.compute(inner)};
    bytes::ByteVec outer{bytes::xorBytes(static_cast<const bytes::ByteVec>(workingKey), opad)};
    bytes::appendBytes(outer, innerHash);

    return hash.compute(outer);
}
