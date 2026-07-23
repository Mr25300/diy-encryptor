#include "bytes.hpp"

#include <random>

namespace bytes {
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
