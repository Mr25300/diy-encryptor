#include "bytes.hpp"

#include <stdexcept>

namespace bytes {
    void xorBytes(ByteVec& bytes1, const ByteVec& bytes2) {
        if (bytes1.size() != bytes2.size())
            throw std::invalid_argument("Cannot XOR byte blocks of different lengths.");

        for (std::size_t i{}; i < bytes1.size(); ++i) {
            bytes1[i] ^= bytes2[i];
        }
    }

    ByteVec xorBytes(const ByteVec& bytes1, const ByteVec& bytes2) {
        ByteVec copy{bytes1};
        xorBytes(copy, bytes2);

        return copy;
    }

    void xorBytes(ByteVec& bytes, std::uint8_t byte) {
        for (std::size_t i{}; i < bytes.size(); ++i) {
            bytes[i] ^= byte;
        }
    }

    template <std::size_t Size>
    ByteVec xorBytes(ByteVec& bytes, std::uint8_t byte) {
        ByteVec copy{bytes};
        xorBytes(bytes, byte);

        return copy;
    }

    template <std::size_t Size>
    void xorBytes(ByteArr<Size>& bytes1, const ByteArr<Size>& bytes2) {
        for (std::size_t i{}; i < Size; ++i) {
            bytes1[i] ^= bytes2[i];
        }
    }

    template <std::size_t Size>
    ByteArr<Size> xorBytes(const ByteArr<Size>& bytes1, const ByteArr<Size>& bytes2) {
        ByteArr<Size> copy{bytes1};
        xorBytes(copy, bytes2);

        return copy;
    }

    template <std::size_t Size>
    void xorBytes(ByteArr<Size>& bytes, std::uint8_t byte) {
        for (std::size_t i{}; i < bytes.size(); ++i) {
            bytes[i] ^= byte;
        }
    }

    template <std::size_t Size>
    ByteArr<Size> xorBytes(ByteArr<Size>& bytes, std::uint8_t byte) {
        ByteArr<Size> copy{bytes};
        xorBytes(bytes, byte);

        return copy;
    }

    void appendBytes(ByteVec& bytes1, const ByteVec& bytes2) {
        bytes1.reserve(bytes1.size() + bytes2.size());
        bytes1.insert(bytes1.end(), bytes2.begin(), bytes2.end());
    }

    ByteVec appendBytes(const ByteVec& bytes1, const ByteVec& bytes2) {
        ByteVec copy{bytes1};
        appendBytes(copy, bytes2);

        return copy;
    }
}
