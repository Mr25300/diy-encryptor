#pragma once

#include <string>
#include <sstream>

#include "vector.hpp"

template <size_t size>
class Matrix {
    std::array<Vector<size>, size> rows;
    bool singular;

    Vector<size> matMultiply(const Vector<size>& word) const {
        Vector<size> newWord = word;

        for (int i = 0; i < size; i++) {
            newWord[i] = rows[i] * word;
        }

        return newWord;
    }

public:
    constexpr Matrix() : rows{} {}
    constexpr Matrix(std::array<Vector<size>, size> values) : rows(values), singular(false) {}

    static constexpr Matrix<size> createIdentityMatrix() {
        std::array<Vector<size>, size> values;

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                values[i][j] = i == j ? 1 : 0;
            }
        }

        return Matrix<size>(values);
    }

    static constexpr Matrix<size> createCirculantMatrix(const Vector<size>& initRow) {
        Vector<size> tempRow = initRow;
        std::array<Vector<size>, size> values;

        for (int i = 0; i < size; i++) {
            values[i] = tempRow;
            tempRow.rotWord(true);
        }

        return Matrix<size>(values);
    }

    constexpr const Vector<size>& operator[](uint8_t index) const {
        return rows[index];
    }

    constexpr Vector<size>& operator[](uint8_t index) {
        return rows[index];
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

    constexpr Matrix<size> operator*(const Matrix<size>& matrix) const {
        std::array<Vector<size>, size> values;

        for (int r = 0; r < size; r++) {
            for (int c = 0; c < size; c++) {
                GF256 sum = 0;

                for (int i = 0; i < size; i++) {
                    sum += rows[r][i] * matrix.rows[i][c];
                }

                values[r][c] = sum;
            }
        }

        return Matrix<size>(values);
    }

    constexpr Matrix<size> inverse() const {
        Matrix<size> A = *this;
        Matrix<size> I = Matrix<size>::createIdentityMatrix();

        int currentPivotRow = 0;

        for (int pivotCol = 0; pivotCol < size; pivotCol++) {
            for (int rowInd = currentPivotRow; rowInd < size; rowInd++) {
                GF256 pivot = A.rows[rowInd][pivotCol];

                if (pivot != 0) {
                    if (rowInd != currentPivotRow) {
                        Vector<size> tempRow = A.rows[rowInd];
                        Vector<size> invTempRow = I.rows[rowInd];

                        A.rows[rowInd] = A.rows[currentPivotRow];
                        A.rows[currentPivotRow] = tempRow;

                        I.rows[rowInd] = I.rows[currentPivotRow];
                        I.rows[currentPivotRow] = invTempRow;
                    }

                    Vector<size>& invPivotRow = I.rows[currentPivotRow];
                    Vector<size>& pivotRow = A.rows[currentPivotRow];

                    pivotRow /= pivot;
                    invPivotRow /= pivot;

                    for (int i = 0; i < size; i++) {
                        if (i == currentPivotRow) continue;

                        Vector<size>& otherRow = A.rows[i];
                        Vector<size>& invOtherRow = I.rows[i];

                        GF256 factor = otherRow[pivotCol];

                        if (factor == 0) continue;

                        otherRow -= pivotRow * factor;
                        invOtherRow -= invPivotRow * factor;
                    }

                    currentPivotRow += 1;

                    break;
                }
            }
        }

        if (currentPivotRow < size) {
            A.singular = true;
            I.singular = true;
        }

        return I;
    }

    constexpr bool isSingular() const {
        return this->singular;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Matrix& matrix) {
        for (int i = 0; i < size; i++) {
            if (i > 0) stream << '\n';

            stream << '[';
            matrix.rows[i].print(stream, GFFormat::Int, true);
            stream << ']';
        }

        return stream;
    }
};