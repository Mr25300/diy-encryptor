#pragma once

#include <cstdint>
#include <vector>

namespace bytes {
    template <std::size_t N>
    using ByteArr = std::array<std::uint8_t, N>;

    using ByteVec = std::vector<std::uint8_t>;

    template <typename C>
    void xorBytes(C& bytes1, const C& bytes2);
    template <typename C>
    C getXorBytes(const C& bytes1, const C& bytes2);

    template <typename C>
    void xorBytes(C& bytes, std::uint8_t byte);
    template <typename C>
    C getXorBytes(C& bytes, std::uint8_t byte);

    template <typename C>
    void appendBytes(ByteVec& bytes1, const C& bytes2);
    template <typename C1, typename C2>
    ByteVec getAppendBytes(const C1& bytes1, const C2& bytes2);
    template <std::size_t N1, std::size_t N2>
    ByteArr<N1 + N2> getAppendBytesArr(const ByteArr<N1> bytes1, const ByteArr<N2> bytes2);

    template <std::size_t N>
    std::vector<ByteArr<N>> getChunks(const ByteVec& bytes);
}
