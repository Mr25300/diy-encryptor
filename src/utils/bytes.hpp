#pragma once

#include <cstdint>
#include <vector>

namespace bytes {
    template <std::size_t Size>
    using ByteArr = std::array<std::uint8_t, Size>;

    using ByteVec = std::vector<std::uint8_t>;

    void xorBytes(ByteVec& bytes1, const ByteVec& bytes2);
    ByteVec xorBytes(const ByteVec& bytes1, const ByteVec& bytes2);

    void xorBytes(ByteVec& bytes, std::uint8_t byte);
    ByteVec xorBytes(const ByteVec& bytes, std::uint8_t byte);

    template <std::size_t Size>
    void xorBytes(ByteArr<Size>& bytes1, const ByteArr<Size>& bytes2);
    template <std::size_t Size>
    ByteArr<Size> xorBytes(const ByteArr<Size>& bytes1, const ByteArr<Size>& bytes2);

    template <std::size_t Size>
    void xorBytes(ByteArr<Size>& bytes, std::uint8_t byte);
    template <std::size_t Size>
    ByteArr<Size> xorBytes(ByteArr<Size>& bytes, std::uint8_t byte);

    void appendBytes(ByteVec& bytes1, const ByteVec& bytes2);
    ByteVec appendBytes(const ByteVec& bytes1, const ByteVec& bytes2);

    template <std::size_t Size>
    std::vector<ByteArr<Size>> getChunks(const ByteVec& bytes);
}
