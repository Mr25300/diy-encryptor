#pragma once

#include <utils/bytes.hpp>

#include <stdexcept>
#include <iostream>
#include <iomanip>

namespace test {
    bytes::ByteVec toBytes(std::string_view str, std::size_t repeatCount = 1);

    template <std::size_t N>
    constexpr bytes::ByteArr<N> toBytes(std::string_view str, std::size_t repeatCount = 1) {
        if (str.size() * repeatCount != N)
            throw std::invalid_argument("String length does not match expected byte count.");

        bytes::ByteArr<N> bytes;

        for (std::size_t i{}; i < repeatCount; ++i) {
            std::copy(str.begin(), str.end(), bytes.begin() + str.size() * i);
        }

        return bytes;
    }

    constexpr std::uint8_t parseHexChr(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        else if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        else throw std::invalid_argument("Invalid hex character.");
    }

    bytes::ByteVec parseHexStr(std::string_view str);

    template <std::size_t N>
    constexpr bytes::ByteArr<N> parseHexStr(std::string_view str) {
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
}
