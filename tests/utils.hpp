#include <utils/bytes.hpp>

#include <cassert>
#include <stdexcept>
#include <iostream>
#include <iomanip>

bytes::ByteVec toBytes(std::string_view str, std::size_t repeatCount = 1) {
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

template <std::size_t N>
bytes::ByteArr<N> parseHexStr(std::string_view str) {
    if (str.size() != N * 2) throw std::invalid_argument("Invalid hex string.");

    bytes::ByteArr<N> bytes;

    for (std::size_t i{}; i < N; ++i) {
        bytes[i] = parseHexChr(str[2 * i]) << 4 | parseHexChr(str[2 * i + 1]);
    }

    return bytes;
}

template <typename C>
void printBytes(const C& bytes, std::ostream& output) {
    output << std::hex << std::setw(2) << std::setfill('0');

    for (std::uint8_t b : bytes) {
        output << static_cast<int>(b);
    }

    output << std::dec;
}
