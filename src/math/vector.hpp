#pragma once

#include <math/gf256.hpp>

#include <ostream>

namespace math {
    template <typename T, std::size_t Size>
    class Vector {
    private:
        std::array<T, Size> values;

    public:
        constexpr Vector() : values{} {}
        constexpr Vector(std::array<T, Size> values) : values{values} {}

        constexpr const T& operator[](std::size_t index) const { return values[index]; }
        constexpr T& operator[](std::size_t index) { return values[index]; }

        constexpr Vector<T, Size> operator+(const Vector<T, Size>& vec) const {
            std::array<T, Size> newValues;

            for (int i{}; i < Size; ++i) {
                newValues[i] = values[i] + vec.values[i];
            }

            return Vector<T, Size>{newValues};
        }

        constexpr Vector<T, Size>& operator+=(const Vector<T, Size>& vec) {
            for (int i{}; i < Size; ++i) {
                values[i] += vec.values[i];
            }

            return *this;
        }

        constexpr Vector<T, Size> operator-(const Vector<T, Size>& vec) const {
            std::array<T, Size> newValues;

            for (int i{}; i < Size; ++i) {
                newValues[i] = values[i] - vec.values[i];
            }

            return Vector<T, Size>{newValues};
        }

        constexpr Vector<T, Size>& operator-=(const Vector<T, Size>& vec) {
            for (int i{}; i < Size; ++i) {
                values[i] -= vec.values[i];
            }

            return *this;
        }

        constexpr Vector<T, Size> operator*(T scalar) const {
            std::array<T, Size> newValues;

            for (int i{}; i < Size; ++i) {
                newValues[i] = values[i] * scalar;
            }

            return Vector<T, Size>{newValues};
        }

        constexpr Vector<T, Size>& operator*=(T scalar) {
            for (int i{}; i < Size; ++i) {
                values[i] *= scalar;
            }

            return *this;
        }

        constexpr Vector<T, Size> operator/(T scalar) const {
            std::array<T, Size> newValues;

            for (int i{}; i < Size; ++i) {
                newValues[i] = values[i] / scalar;
            }

            return Vector<T, Size>{newValues};
        }

        constexpr Vector<T, Size>& operator/=(T scalar) {
            for (int i{}; i < Size; ++i) {
                values[i] /= scalar;
            }

            return *this;
        }

        constexpr T operator*(const Vector<T, Size>& vec) const {
            T sum{};

            for (int i{}; i < Size; ++i) {
                sum += values[i] * vec.values[i];
            }

            return sum;
        }

        // void subWord(const SubstitutionBox& subBox, bool inverse = false) {
        //     for (int i = 0; i < size; i++) {
        //         values[i] = inverse ? subBox.sub(values[i]) : subBox.subInv(values[i]);
        //     }
        // }
        //
        // void applyConstant(GF256 constant) {
        //     values[0] += constant;
        // }
        //
        // void print(std::ostream& stream, GFFormat format = GFFormat::Hex, bool list = false) const {
        //     for (int i = 0; i < size; i++) {
        //         if (list && i > 0) stream << ',' << ' ';
        //
        //         values[i].print(stream, format);
        //     }
        // }

        friend std::ostream& operator<<(std::ostream& stream, const Vector<T, Size>& vector) {
            for (std::size_t i{}; i < Size; ++i) {
                if (i > 0) stream << ", ";

                stream << vector.values[i];
            }

            return stream;
        }
    };
}
