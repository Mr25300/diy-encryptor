#pragma once

#include <math/gf256.hpp>

#include <cstddef>
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

            for (std::size_t i{}; i < Size; ++i) {
                newValues[i] = values[i] + vec.values[i];
            }

            return Vector<T, Size>{newValues};
        }

        constexpr Vector<T, Size>& operator+=(const Vector<T, Size>& vec) {
            for (std::size_t i{}; i < Size; ++i) {
                values[i] += vec.values[i];
            }

            return *this;
        }

        constexpr Vector<T, Size> operator-(const Vector<T, Size>& vec) const {
            std::array<T, Size> newValues;

            for (std::size_t i{}; i < Size; ++i) {
                newValues[i] = values[i] - vec.values[i];
            }

            return Vector<T, Size>{newValues};
        }

        constexpr Vector<T, Size>& operator-=(const Vector<T, Size>& vec) {
            for (std::size_t i{}; i < Size; ++i) {
                values[i] -= vec.values[i];
            }

            return *this;
        }

        constexpr Vector<T, Size> operator*(T scalar) const {
            std::array<T, Size> newValues;

            for (std::size_t i{}; i < Size; ++i) {
                newValues[i] = values[i] * scalar;
            }

            return Vector<T, Size>{newValues};
        }

        constexpr Vector<T, Size>& operator*=(T scalar) {
            for (std::size_t i{}; i < Size; ++i) {
                values[i] *= scalar;
            }

            return *this;
        }

        constexpr Vector<T, Size> operator/(T scalar) const {
            std::array<T, Size> newValues;

            for (std::size_t i{}; i < Size; ++i) {
                newValues[i] = values[i] / scalar;
            }

            return Vector<T, Size>{newValues};
        }

        constexpr Vector<T, Size>& operator/=(T scalar) {
            for (std::size_t i{}; i < Size; ++i) {
                values[i] /= scalar;
            }

            return *this;
        }

        constexpr T operator*(const Vector<T, Size>& vec) const {
            T sum{};

            for (std::size_t i{}; i < Size; ++i) {
                sum += values[i] * vec.values[i];
            }

            return sum;
        }

        constexpr bool operator==(const Vector<T, Size>& vec) const { return values == vec.values; }
        constexpr bool operator!=(const Vector<T, Size>& vec) const { return values == vec.values; }

        friend std::ostream& operator<<(std::ostream& stream, const Vector<T, Size>& vector) {
            for (std::size_t i{}; i < Size; ++i) {
                if (i > 0) stream << ", ";

                stream << vector.values[i];
            }

            return stream;
        }
    };
}
