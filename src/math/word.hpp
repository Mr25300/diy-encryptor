#pragma once

#include <ostream>

#include <math/gf256.hpp>
#include <math/utils.hpp>
#include <aes/substitution_box.hpp>

namespace math {
    template <std::size_t size>
    class Word {
        std::array<GF256, size> values;

    public:
        constexpr Word() : values{} {}
        constexpr Word(std::array<GF256, size> values) : values(values) {}

        constexpr const GF256& operator[](uint8_t index) const {
            return values[index];
        }

        constexpr GF256& operator[](uint8_t index) {
            return values[index];
        }

        constexpr Word operator+(const Word& other) const {
            std::array<GF256, size> newValues;

            for (int i = 0; i < size; i++) {
                newValues[i] = values[i] + other.values[i];
            }

            return Word(newValues);
        }

        constexpr Word& operator+=(const Word& other) {
            for (int i = 0; i < size; i++) {
                values[i] += other.values[i];
            }

            return *this;
        }

        constexpr Word operator-(const Word& other) const {
            std::array<GF256, size> newValues;

            for (int i = 0; i < size; i++) {
                newValues[i] = values[i] - other.values[i];
            }

            return Word(newValues);
        }

        constexpr Word& operator-=(const Word& other) {
            for (int i = 0; i < size; i++) {
                values[i] -= other.values[i];
            }

            return *this;
        }

        constexpr Word operator*(GF256 scalar) const {
            std::array<GF256, size> newValues;

            for (int i = 0; i < size; i++) {
                newValues[i] = values[i] * scalar;
            }

            return Word(newValues);
        }

        constexpr Word& operator*=(GF256 scalar) {
            for (int i = 0; i < size; i++) {
                values[i] *= scalar;
            }

            return *this;
        }

        constexpr Word operator/(GF256 scalar) const {
            std::array<GF256, size> newValues;

            for (int i = 0; i < size; i++) {
                newValues[i] = values[i] / scalar;
            }

            return Word(newValues);
        }

        constexpr Word& operator/=(GF256 scalar) {
            for (int i = 0; i < size; i++) {
                values[i] /= scalar;
            }

            return *this;
        }

        constexpr GF256 operator*(const Word& other) const {
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
                int newInd = utils::properMod(i + direction, size);

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

        friend std::ostream& operator<<(std::ostream& stream, const Word<size>& vector) {
            vector.print(stream);

            return stream;
        }
    };
}
