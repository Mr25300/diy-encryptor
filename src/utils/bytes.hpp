#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <span>
#include <stdexcept>
#include <cassert>

namespace bytes {
    template <std::size_t N>
    using ByteArr = std::array<std::uint8_t, N>;

    using ByteVec = std::vector<std::uint8_t>;

    template <typename C>
    void xorBytes(C& bytes1, const C& bytes2) {
        if (bytes1.size() != bytes2.size())
            throw std::invalid_argument("Cannot XOR byte blocks of different lengths.");

        for (std::size_t i{}; i < bytes1.size(); ++i) {
            bytes1[i] ^= bytes2[i];
        }
    }

    template <typename C>
    C getXorBytes(const C& bytes1, const C& bytes2) {
        C res{bytes1};

        xorBytes(res, bytes2);

        return res;
    }

    template <typename C>
    void xorBytes(C& bytes, std::uint8_t byte) {
        for (std::uint8_t& b : bytes) {
            b ^= byte;
        }
    }

    template <typename C>
    C getXorBytes(const C& bytes, std::uint8_t byte) {
        C res{bytes};

        xorBytes(res, byte);

        return res;
    }

    template <typename C>
    void appendBytes(ByteVec& bytes1, const C& bytes2) {
        bytes1.reserve(bytes1.size() + bytes2.size());
        bytes1.insert(bytes1.end(), bytes2.begin(), bytes2.end());
    }

    template <typename C1, typename C2>
    ByteVec getAppendBytes(const C1& bytes1, const C2& bytes2) {
        ByteVec res;
        res.reserve(bytes1.size() + bytes2.size());

        res.insert(res.end(), bytes1.begin(), bytes1.end());
        res.insert(res.end(), bytes2.begin(), bytes2.end());

        return res;
    }

    template <std::size_t N1, std::size_t N2>
    ByteArr<N1 + N2> getAppendBytesArr(const ByteArr<N1> bytes1, const ByteArr<N2> bytes2) {
        ByteArr<N1 + N2> res;

        std::copy(bytes1.begin(), bytes1.end(), res.begin());
        std::copy(bytes2.begin(), bytes2.end(), res.begin() + bytes1.size());

        return res;
    }

    template <std::size_t N>
    std::span<ByteArr<N>> getChunkView(const ByteVec& bytes) {
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
    std::span<std::uint8_t> getFlatView(std::span<ByteArr<N>>& chunks) {
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
