#pragma once

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

    constexpr Matrix<size> inv() {

    }

    Word<size> operator*(const Word<size>& word) const {
        return matMultiply(word);
    }

    friend Word<size> operator*(const Word<size>& word, const Matrix<size>& mat) {
        return mat * word;
    }

    friend Word<size>& operator*=(Word<size>& word, const Matrix<size>& mat) {
        word = mat * word;

        return word;
    }
};