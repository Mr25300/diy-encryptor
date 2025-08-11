#pragma once

#include <string>
#include <sstream>

#include "vector.hpp"

template <size_t size>
class Matrix {
    std::array<Vector<size>, size> rows;

    Vector<size> matMultiply(const Vector<size>& word) const {
        Vector<size> newWord = word;

        for (int i = 0; i < size; i++) {
            newWord[i] = rows[i] * word; // Dot each row with word vector to get resulting vector at the same row
        }

        return newWord;
    }

public:
    constexpr Matrix() : rows{} {}
    constexpr Matrix(std::array<Vector<size>, size> values) : rows(values) {}

    static constexpr Matrix<size> createMaxDiffusion(const Vector<size>& initRow) {
        Vector<size> tempRow = initRow;
        std::array<Vector<size>, size> vals;

        for (int i = 0; i < size; i++) {
            vals[i] = tempRow;
            tempRow.rotWord(true);
        }

        return Matrix<size>(vals);
    }

    constexpr const Vector<size>& operator[](uint8_t index) const {
        return rows[index];
    }

    constexpr Vector<size>& operator[](uint8_t index) {
        return rows[index];
    }

    constexpr Matrix<size - 1> minor(int delRow, int delCol) const {
        Matrix<size - 1> minor{};

        for (int r = 0; r < size - 1; r++) {
            int row = r < delRow ? r : r + 1;

            for (int c = 0; c < size - 1; c++) {
                minor[r][c] = rows[row][c < delCol ? c : c + 1];
            }
        }

        return minor;
    }

    constexpr GF256 determinant() const {
        if constexpr (size == 2) {
            return rows[0][0] * rows[1][1] - rows[0][1] * rows[1][0];

        } else {
            GF256 sum = 0;

            for (int c = 0; c < size; c++) {
                sum += rows[0][c] * minor(0, c).determinant(); // Ignore cofactor sign since negation is equivalent to identity operator in gf(2^8)
            }

            return sum;
        }
    }

    constexpr Matrix<size> transpose() const {
        Matrix<size> result{};

        for (int r = 0; r < size; r++) {
            for (int c = 0; c < size; c++) {
                result[r][c] = rows[c][r];
            }
        }

        return result;
    }

    constexpr Matrix<size> adjugate() const {
        Matrix<size> cofactors{};

        for (int r = 0; r < size; r++) {
            for (int c = 0; c < size; c++) {
                Matrix<size - 1> min = minor(r, c); 

                cofactors[r][c] = min.determinant(); // Ignore cofactor sign since negation is equivalent to identity operator in gf(2^8)
            }
        }

        return cofactors.transpose();
    }

    constexpr Matrix<size> inverse() const {
        return this->adjugate() * this->determinant().inv();
    }

    constexpr Matrix<size> operator*(GF256 scalar) const {
        Matrix<size> scaled{};

        for (int r = 0; r < size; r++) {
            for (int c = 0; c < size; c++) {
                scaled[r][c] = rows[r][c] * scalar;
            }
        }

        return scaled;
    }

    constexpr Matrix<size>& operator*=(GF256 scalar) {
        for (int r = 0; r < size; r++) {
            for (int c = 0; c < size; c++) {
                rows[r][c] *= scalar;
            }
        }

        return *this;
    }

    constexpr Vector<size> operator*(const Vector<size>& word) const {
        return matMultiply(word);
    }

    constexpr friend Vector<size> operator*(const Vector<size>& word, const Matrix<size>& mat) {
        return mat * word;
    }

    constexpr friend Vector<size>& operator*=(Vector<size>& word, const Matrix<size>& mat) {
        word = mat * word;

        return word;
    }

    std::string toString(GFFormat format = GFFormat::Int) const {
        std::ostringstream oss;

        for (int i = 0; i < size; i++) {
            if (i > 0) oss << '\n';

            char leftBorder, rightBorder;

            switch (i) {
                case 0: {
                    leftBorder = '/';
                    rightBorder = '\\';

                    break;
                }
                case size - 1: {
                    leftBorder = '\\';
                    rightBorder = '/';

                    break;
                }
                default:
                    leftBorder = rightBorder = '|';

                    break;
            }

            oss << leftBorder << ' ' << rows[i].toString(format, true) << ' ' << rightBorder;
        }

        return oss.str();
    }
};