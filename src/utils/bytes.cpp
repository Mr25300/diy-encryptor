#include "bytes.hpp"

#include <stdexcept>
#include <random>

namespace bytes {
    void xorBytes(ByteView bytes1, ConstByteView bytes2) {
        if (bytes1.size() != bytes2.size())
            throw std::invalid_argument("Cannot XOR byte blocks of different lengths.");

        for (std::size_t i{}; i < bytes1.size(); ++i) {
            bytes1[i] ^= bytes2[i];
        }
    }

    void xorBytes(ByteView bytes, std::uint8_t byte) {
        for (std::uint8_t& b : bytes) {
            b ^= byte;
        }
    }

    ByteVec getXorBytes(ConstByteView bytes1, ConstByteView bytes2) {
        ByteVec res{bytes1.begin(), bytes1.end()};
        xorBytes(res, bytes2);

        return res;
    }

    ByteVec getXorBytes(ConstByteView bytes, std::uint8_t byte) {
        ByteVec res(bytes.begin(), bytes.end());
        xorBytes(res, byte);

        return res;
    }

    void appendBytes(ByteVec& bytes1, ConstByteView bytes2) {
        bytes1.reserve(bytes1.size() + bytes2.size());
        bytes1.insert(bytes1.end(), bytes2.begin(), bytes2.end());
    }

    ByteVec getAppendBytes(ConstByteView bytes1, ConstByteView bytes2) {
        ByteVec res;
        res.reserve(bytes1.size() + bytes2.size());

        res.insert(res.end(), bytes1.begin(), bytes1.end());
        res.insert(res.end(), bytes2.begin(), bytes2.end());

        return res;
    }

    std::uint8_t getRandByte() {
        // static but thread safe
        thread_local std::random_device rd;
        thread_local std::mt19937 gen(rd());
        thread_local std::uniform_int_distribution<> dist(0, 255);

        return dist(gen);
    }

    ByteVec getRandBytes(std::size_t n) {
        ByteVec bytes;
        bytes.resize(n);

        for (std::size_t i{}; i < n; ++i) {
            bytes[i] = getRandByte();
        }

        return bytes;
    }
}
