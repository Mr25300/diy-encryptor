#pragma once

#include "gf256.hpp"
#include "util.hpp"

template <size_t len>
class Word {
    std::array<GF256, len> bytes;

public:
    constexpr Word() : bytes{} {}
    constexpr Word(std::array<GF256, len> values) : bytes(values) {}

    const GF256& operator[](uint8_t index) const {
        return bytes[index];
    }

    GF256& operator[](uint8_t index) {
        return bytes[index];
    }

    Word operator+(const Word& other) const {
        std::array<GF256, len> newValues;

        for (int i = 0; i < len; i++) {
            newValues[i] = bytes[i] + other.bytes[i];
        }

        return Word(newValues);
    }

    Word& operator+=(const Word& other) {
        for (int i = 0; i < len; i++) {
            bytes[i] += other.bytes[i];
        }

        return *this;
    }

    GF256 operator*(const Word& other) const {
        GF256 sum = 0;

        for (int i = 0; i < len; i++) {
            sum += bytes[i] * other.bytes[i];
        }

        return sum;
    }

    // friend Word& operator*=(Word<len>& word, const Matrix<len>& mat);

    constexpr void rotWord(bool invDir = false) {
        std::array<GF256, len> temp_bytes = bytes;

        int direction = invDir ? -1 : 1;

        for (int i = 0; i < len; i++) {
            int newInd = mod(i + direction, len);

            bytes[i] = temp_bytes[newInd];
        }
    }
    
    void subWord(const std::array<GF256, 256>& subBox) {
        for (int i = 0; i < len; i++) {
            bytes[i] = subBox[bytes[i].get()];
        }
    }

    void applyConstant(GF256 constant) {
        bytes[0] += constant;
    }

    std::string asString() {
        std::string result = "";

        for (int i = 0; i < len; i++) {
            result += bytes[i].asChar();
        }

        return result;
    }
};