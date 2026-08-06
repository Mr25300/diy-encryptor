#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <span>
#include <cassert>

namespace bytes {
    template <std::size_t N>
    using ByteArr = std::array<std::uint8_t, N>;

    using ByteVec = std::vector<std::uint8_t>;

    using ByteView = std::span<std::uint8_t>;
    using ConstByteView = std::span<const std::uint8_t>;

    void xorBytes(ByteView bytes1, ConstByteView bytes2);
    void xorBytes(ByteView bytes, std::uint8_t byte);

    ByteVec getXorBytes(ConstByteView bytes1, ConstByteView bytes2);
    ByteVec getXorBytes(ConstByteView bytes, std::uint8_t byte);

    template <std::size_t N>
    ByteArr<N> getXorBytesArr(const ByteArr<N>& bytes1, const ByteArr<N>& bytes2) {
        ByteArr<N> res{bytes1};
        xorBytes(res, bytes2);

        return res;
    }

    template <std::size_t N>
    ByteArr<N> getXorBytesArr(const ByteArr<N>& bytes, std::uint8_t byte) {
        ByteArr<N> res{bytes};
        xorBytes(res, byte);

        return res;
    }

    void appendBytes(ByteVec& bytes1, ConstByteView bytes2);
    ByteVec getAppendBytes(ConstByteView bytes1, ConstByteView bytes2);

    template <std::size_t N1, std::size_t N2>
    ByteArr<N1 + N2> getAppendBytesArr(const ByteArr<N1>& bytes1, const ByteArr<N2>& bytes2) {
        ByteArr<N1 + N2> res;

        std::copy(bytes1.begin(), bytes1.end(), res.begin());
        std::copy(bytes2.begin(), bytes2.end(), res.begin() + bytes1.size());

        return res;
    }

    template <std::size_t N>
    std::span<ByteArr<N>> getChunkView(ByteView bytes) {
        static_assert(N > 0, "Chunk size cannot be zero.");
        assert(bytes.size() % N == 0 && "Byte count must be a multiple of the chunk size.");

        // Avoids memory unsafety of casting to vector as the first 24 bytes of memory for a vector is overhead
        return std::span<ByteArr<N>>(
            // Returns first pointer in contiguous ByteVec data
            reinterpret_cast<ByteArr<N>*>(bytes.data()),
            bytes.size() / N
        );
    }

    template <std::size_t N>
    ByteView getFlatView(std::span<ByteArr<N>> chunks) {
        return std::span<std::uint8_t>(
            reinterpret_cast<std::uint8_t*>(chunks.data()),
            chunks.size() * N
        );
    }

    std::uint8_t getRandByte();
    ByteVec getRandBytes(std::size_t n);

    template <std::size_t N>
    ByteArr<N> getRandBytes() {
        ByteArr<N> bytes;

        for (std::size_t i{}; i < N; ++i) {
            bytes[i] = getRandByte();
        }

        return bytes;
    }
}
