#pragma once

#include "vector.hpp"

namespace math {
    template <typename T, std::size_t Rows, std::size_t Cols>
    class Matrix;

    template <typename T, std::size_t Size>
    struct InverseResult {
        Matrix<T, Size, Size> mat;
        bool isSingular;
    };

    template <typename T, std::size_t Rows, std::size_t Cols = Rows>
    class Matrix {
        std::array<Vector<T, Cols>, Rows> rows;

    public:
        constexpr Matrix() : rows{} {}
        constexpr Matrix(std::array<Vector<T, Cols>, Rows> values) : rows{values} {}

        template<std::size_t Size>
        static constexpr Matrix<T, Size, Size> getIdentityMatrix() {
            std::array<Vector<T, Size>, Size> values{};

            for (std::size_t i{}; i < Size; ++i) {
                values[i][i] = static_cast<T>(1);
            }

            return Matrix<T, Size, Size>{values};
        }

        static constexpr Matrix<T, Rows, Cols> getCirculantMatrix(const Vector<T, Cols>& initRow) {
            std::array<Vector<T, Cols>, Rows> values{};

            for (std::size_t i{}; i < Rows; ++i) {
                for (std::size_t j{}; j < Cols; ++j) {
                    values[i][(j + i) % Cols] = initRow[j];
                }
            }

            return Matrix<T, Rows, Cols>{values};
        }

        constexpr const Vector<T, Cols>& operator[](uint8_t index) const { return rows[index]; }
        constexpr Vector<T, Cols>& operator[](uint8_t index) { return rows[index]; }

        constexpr Matrix<T, Rows, Cols> operator*(T scalar) const {
            std::array<Vector<T, Cols>, Rows> newValues;

            for (std::size_t i{}; i < Rows; ++i) {
                for (std::size_t j{}; j < Cols; ++j) {
                    newValues[i][j] = rows[i][j] * scalar;
                }
            }

            return Matrix<T, Rows, Cols>{newValues};
        }

        constexpr Matrix<T, Rows, Cols>& operator*=(T scalar) {
            for (std::size_t i{}; i < Rows; ++i) {
                for (std::size_t j{}; j < Cols; ++j) {
                    rows[i][j] *= scalar;
                }
            }

            return *this;
        }

        constexpr Vector<T, Rows> operator*(const Vector<T, Cols>& vec) const {
            std::array<T, Rows> values;

            for (std::size_t i{}; i < Rows; ++i) {
                values[i] = rows[i] * vec;
            }

            return Vector<T, Rows>{values};
        }

        constexpr friend T operator*(const Vector<T, Cols>& vec, const Matrix<T, 1, Cols>& mat) {
            return vec * mat[0];
        }

        template <std::size_t Size>
        constexpr friend Vector<T, Size>& operator*=(Vector<T, Size>& vec, const Matrix<T, Size, Size>& mat) {
            Vector<T, Size> vecCopy{vec};

            for (std::size_t i{}; i < Size; ++i) {
                vec[i] = mat[i] * vecCopy;
            }

            return vec;
        }

        template <std::size_t NewCols>
        constexpr Matrix<T, Rows, NewCols> operator*(const Matrix<T, Cols, NewCols>& mat) const {
            std::array<Vector<T, NewCols>, Rows> values;

            for (std::size_t i{}; i < Rows; ++i) {
                for (std::size_t j{}; j < NewCols; ++j) {
                    T sum{};

                    for (int k{}; k < Cols; ++k) {
                        sum += rows[i][k] * mat.rows[k][j];
                    }

                    values[i][j] = sum;
                }
            }

            return Matrix<T, Rows, NewCols>{values};
        }

        template <std::size_t Size>
        constexpr Matrix<T, Size, Size>& operator*=(const Matrix<T, Size, Size>& mat) {
            // TODO: Implement method

            return *this;
        }

        constexpr bool operator==(const Matrix<T, Rows, Cols>& mat) const { return rows == mat.rows; }
        constexpr bool operator!=(const Matrix<T, Rows, Cols>& mat) const { return rows != mat.rows; }

        template <std::size_t AugCols>
        constexpr bool rowReduce(Matrix<T, Rows, AugCols>& aug);

        template <std::size_t Size>
        constexpr InverseResult<T, Size> getInverse() const {
            Matrix<T, Size, Size> copy{*this};
            Matrix<T, Size, Size> inverse{Matrix::getIdentityMatrix<Size>()};
            bool isSingular{copy.rowReduce(inverse)};

            return {inverse, isSingular};
        }

        friend std::ostream& operator<<(std::ostream& stream, const Matrix<T, Rows, Cols>& mat) {
            for (std::size_t i{}; i < Rows; ++i) {
                if (i > 0) stream << '\n';

                stream << '[' << mat.rows[i] << ']';
            }

            return stream;
        }
    };

    template<typename T, std::size_t Rows, std::size_t Cols>
    template<std::size_t AugCols>
    constexpr bool Matrix<T, Rows, Cols>::rowReduce(Matrix<T, Rows, AugCols>& aug) {
        std::size_t currentPivotRow{0};
        T zero{static_cast<T>(0)};

        for (std::size_t pivotCol{}; pivotCol < Cols; ++pivotCol) {
            for (std::size_t rowInd{currentPivotRow}; rowInd < Rows; ++rowInd) {
                T pivot{rows[rowInd][pivotCol]};

                if (pivot != zero) {
                    if (rowInd != currentPivotRow) {
                        std::swap(rows[rowInd], rows[currentPivotRow]);
                        std::swap(aug[rowInd], aug[currentPivotRow]);
                        // Vector<T, Size> tempRow{A.rows[rowInd]};
                        // Vector<T, Size> invTempRow{rows[rowInd]};
                        //
                        // A.rows[rowInd] = A.rows[currentPivotRow];
                        // A.rows[currentPivotRow] = tempRow;
                        //
                        // I.rows[rowInd] = I.rows[currentPivotRow];
                        // I.rows[currentPivotRow] = invTempRow;
                    }

                    Vector<T, Cols>& pivotRow{rows[currentPivotRow]};
                    Vector<T, AugCols>& augPivotRow{aug[currentPivotRow]};

                    pivotRow /= pivot;
                    augPivotRow /= pivot;

                    for (std::size_t i{}; i < Rows; ++i) {
                        if (i == currentPivotRow) continue;

                        Vector<T, Cols>& otherRow{rows[i]};
                        Vector<T, AugCols>& augOtherRow{aug[i]};

                        T factor{otherRow[pivotCol]};

                        if (factor == zero) continue;

                        otherRow -= pivotRow * factor;
                        augOtherRow -= augPivotRow * factor;
                    }

                    currentPivotRow += 1;

                    break;
                }
            }
        }

        return currentPivotRow < Rows; // Whether or not this matrix is singular
    }
}
