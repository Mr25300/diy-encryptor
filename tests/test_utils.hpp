#pragma once

#include <utils/bytes.hpp>

#include <stdexcept>
#include <iostream>
#include <iomanip>

bytes::ByteVec toBytes(std::string_view str, std::size_t repeatCount = 1);

std::uint8_t parseHexChr(char c);
bytes::ByteVec parseHexStr(std::string_view str);

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
