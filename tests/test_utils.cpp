#include "test_utils.hpp"

#include <stdexcept>

bytes::ByteVec toBytes(std::string_view str, std::size_t repeatCount) {
    bytes::ByteVec bytes;
    bytes.reserve(str.size() * repeatCount);

    for (std::size_t i{}; i < repeatCount; ++i) {
        bytes.insert(bytes.end(), str.begin(), str.end());
    }

    return bytes;
}

std::uint8_t parseHexChr(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    else if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    else if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    else throw std::invalid_argument("Invalid hex character.");
}

bytes::ByteVec parseHexStr(std::string_view str) {
    if (str.size() % 2 != 0) throw std::invalid_argument("Invalid hex string.");

    bytes::ByteVec bytes;
    bytes.reserve(str.size() / 2);

    for (std::size_t i{}; i < str.size(); i += 2) {
        bytes.push_back(parseHexChr(str[i]) << 4 | parseHexChr(str[i + 1]));
    }

    return bytes;
}
