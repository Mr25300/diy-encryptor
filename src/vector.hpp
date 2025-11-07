#pragma once

#include <ostream>

#include "gf256.hpp"
#include "substitution_box.hpp"
#include "util.hpp"

template <size_t size>
class Vector {
    std::array<GF256, size> values;

public:
    constexpr Vector() : values{} {}
    constexpr Vector(std::array<GF256, size> values) : values(values) {}

    constexpr const GF256& operator[](uint8_t index) const {
        return values[index];
    }

    constexpr GF256& operator[](uint8_t index) {
        return values[index];
    }

    constexpr Vector operator+(const Vector& other) const {
        std::array<GF256, size> newValues;

        for (int i = 0; i < size; i++) {
            newValues[i] = values[i] + other.values[i];
        }

        return Vector(newValues);
    }

    constexpr Vector& operator+=(const Vector& other) {
        for (int i = 0; i < size; i++) {
            values[i] += other.values[i];
        }

        return *this;
    }

    constexpr Vector operator-(const Vector& other) const {
        std::array<GF256, size> newValues;

        for (int i = 0; i < size; i++) {
            newValues[i] = values[i] - other.values[i];
        }

        return Vector(newValues);
    }

    constexpr Vector& operator-=(const Vector& other) {
        for (int i = 0; i < size; i++) {
            values[i] -= other.values[i];
        }

        return *this;
    }

    constexpr Vector operator*(GF256 scalar) const {
        std::array<GF256, size> newValues;

        for (int i = 0; i < size; i++) {
            newValues[i] = values[i] * scalar;
        }

        return Vector(newValues);
    }

    constexpr Vector& operator*=(GF256 scalar) {
        for (int i = 0; i < size; i++) {
            values[i] *= scalar;
        }

        return *this;
    }

    constexpr Vector operator/(GF256 scalar) const {
        std::array<GF256, size> newValues;

        for (int i = 0; i < size; i++) {
            newValues[i] = values[i] / scalar;
        }

        return Vector(newValues);
    }

    constexpr Vector& operator/=(GF256 scalar) {
        for (int i = 0; i < size; i++) {
            values[i] /= scalar;
        }

        return *this;
    }

    constexpr GF256 operator*(const Vector& other) const {
        GF256 sum = 0;

        for (int i = 0; i < size; i++) {
            sum += values[i] * other.values[i];
        }

        return sum;
    }

    constexpr void rotWord(bool invDir = false) {
        std::array<GF256, size> temp_bytes = values;

        int direction = invDir ? -1 : 1;

        for (int i = 0; i < size; i++) {
            int newInd = mod(i + direction, size);

            values[i] = temp_bytes[newInd];
        }
    }
    
    void subWord(const SubstitutionBox& subBox, bool inverse = false) {
        for (int i = 0; i < size; i++) {
            values[i] = inverse ? subBox.sub(values[i]) : subBox.subInv(values[i]);
        }
    }

    void applyConstant(GF256 constant) {
        values[0] += constant;
    }

    void print(std::ostream& stream, GFFormat format = GFFormat::Hex, bool list = false) const {
        for (int i = 0; i < size; i++) {
            if (list && i > 0) stream << ',' << ' ';

            values[i].print(stream, format);
        }
    }

    friend std::ostream& operator<<(std::ostream& stream, const Vector<size>& vector) {
        vector.print(stream);

        return stream;
    }
};