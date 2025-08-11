#pragma once

#include <string>

#include "word.hpp"

template <size_t size>
class Matrix {
    std::array<Word<size>, size> rows;

    Word<size> matMultiply(const Word<size>& word) const {
        Word<size> newWord = word;

        for (int i = 0; i < size; i++) {
            newWord[i] = rows[i] * word; // Dot each row with word vector to get resulting vector at the same row
        }

        return newWord;
    }

public:
    constexpr Matrix() : rows{} {}
    constexpr Matrix(std::array<Word<size>, size> values) : rows(values) {}

    static constexpr Matrix<size> createMaxDiffusion(const Word<size>& initRow) {
        Word<size> tempRow = initRow;
        std::array<Word<size>, size> vals;

        for (int i = 0; i < size; i++) {
            vals[i] = tempRow;
            tempRow.rotWord(true);
        }

        return Matrix<size>(vals);
    }

    constexpr const Word<size>& operator[](uint8_t index) const {
        return rows[index];
    }

    constexpr Word<size>& operator[](uint8_t index) {
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

    constexpr Word<size> operator*(const Word<size>& word) const {
        return matMultiply(word);
    }

    constexpr friend Word<size> operator*(const Word<size>& word, const Matrix<size>& mat) {
        return mat * word;
    }

    constexpr friend Word<size>& operator*=(Word<size>& word, const Matrix<size>& mat) {
        word = mat * word;

        return word;
    }

    std::string asString() const {
        std::string result;

        for (int i = 0; i < size; i++) {
            if (i > 0) result += "\n";

            result += rows[i].asVec();
        }

        return result;
    }
};