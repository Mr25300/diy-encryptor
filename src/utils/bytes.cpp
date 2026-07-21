#include "bytes.hpp"

#include <stdexcept>

namespace bytes {
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

        xorBytesGeneric(res, bytes2);

        return res;
    }

    template <typename C>
    void xorBytes(C& bytes, std::uint8_t byte) {
        for (std::uint8_t& b : bytes) {
            b ^= byte;
        }
    }

    template <typename C>
    C getXorBytes(C& bytes, std::uint8_t byte) {
        C res{bytes};

        for (std::uint8_t& b : res) {
            b ^= byte;
        }

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
    std::vector<ByteArr<N>> getChunks(const ByteVec& bytes) {
        std::vector<ByteArr<N>> chunks;
        chunks.reserve(bytes.size() / N);

        if (bytes.size() % N != 0)
            throw std::invalid_argument("Number of bytes must be a multiple of the chunk size.");

        for (std::size_t i{}; i < bytes.size() / N; ++i) {
            std::copy(
                bytes.begin() + i * N, bytes.begin() + (i + 1) * N, chunks[i].begin()
            );
        }

        return chunks;
    }
}
