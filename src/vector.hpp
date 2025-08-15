#pragma once

#include "gf256.hpp"
#include "substitution_box.hpp"
#include "util.hpp"

template <size_t len>
class Vector {
    std::array<GF256, len> values;

public:
    constexpr Vector() : values{} {}
    constexpr Vector(std::array<GF256, len> values) : values(values) {}

    constexpr const GF256& operator[](uint8_t index) const {
        return values[index];
    }

    constexpr GF256& operator[](uint8_t index) {
        return values[index];
    }

    Vector operator+(const Vector& other) const {
        std::array<GF256, len> newValues;

        for (int i = 0; i < len; i++) {
            newValues[i] = values[i] + other.values[i];
        }

        return Vector(newValues);
    }

    Vector& operator+=(const Vector& other) {
        for (int i = 0; i < len; i++) {
            values[i] += other.values[i];
        }

        return *this;
    }

    GF256 operator*(const Vector& other) const {
        GF256 sum = 0;

        for (int i = 0; i < len; i++) {
            sum += values[i] * other.values[i];
        }

        return sum;
    }

    constexpr void rotWord(bool invDir = false) {
        std::array<GF256, len> temp_bytes = values;

        int direction = invDir ? -1 : 1;

        for (int i = 0; i < len; i++) {
            int newInd = mod(i + direction, len);

            values[i] = temp_bytes[newInd];
        }
    }
    
    void subWord(const SubstitutionBox& subBox, bool inverse = false) {
        for (int i = 0; i < len; i++) {
            values[i] = inverse ? subBox.sub(values[i]) : subBox.subInv(values[i]);
        }
    }

    void applyConstant(GF256 constant) {
        values[0] += constant;
    }

    std::string toString(GFFormat format = GFFormat::Hex, bool list = false) const {
        std::string result;

        for (int i = 0; i < len; i++) {
            if (list && i > 0) result += ", ";

            result += values[i].toString(format);
        }

        return result;
    }
};