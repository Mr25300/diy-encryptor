#pragma once

#include <string>

#include "vector.hpp"
#include "key_schedule.hpp"
#include "util.hpp"

template <size_t cols, size_t rows, size_t rounds>
class KeySchedule;

template <size_t cols, size_t rows>
class Block {
    std::array<Vector<rows>, cols> words;

public:
    Block() : words{} {}
    Block(std::array<Vector<rows>, cols> values) : words(values) {}

    const Vector<rows>& operator[](uint8_t index) const {
        return words[index];
    }

    Vector<rows>& operator[](uint8_t index) {
        return words[index];
    }

    void addKey(const Block<cols, rows>& key) {
        for (int i = 0; i < cols; i++) {
            words[i] += key.words[i];
        }
    }

    void subBytes(std::array<GF256, 256> subBox) {
        for (int i = 0; i < cols; i++) {
            words[i].subWord(subBox);
        }
    }

    void shiftRows(bool invDir = false) {
        std::array<Vector<rows>, cols> tempValues = words;

        int direction = invDir ? -1 : 1;

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                int shiftedCol = mod(c + r * direction, cols); // Shift row an amount equal to the corresponding row number in the correct direction

                words[c][r] = tempValues[shiftedCol][r];
            }
        }
    }

    void mixColumns(const Matrix<rows>& mat) {
        for (int i = 0; i < cols; i++) {
            words[i] *= mat;
        }
    }

    template <size_t rounds>
    void encrypt(const KeySchedule<cols, rows, rounds>& keySchedule, const std::array<GF256, 256>& subBox, const Matrix<rows>& mixColMatrix) {
        addKey(keySchedule.getRoundKey(0));

        for (int n = 1; n <= rounds; n++) {
            subBytes(subBox);
            shiftRows();

            if (n != rounds) {
                mixColumns(mixColMatrix);
            }

            addKey(keySchedule.getRoundKey(n));
        }
    }

    template <size_t rounds>
    void decrypt(const KeySchedule<cols, rows, rounds>& keySchedule, const std::array<GF256, 256>& subBoxInv, const Matrix<rows>& mixColMatrixInv) {
        for (int n = rounds; n >= 1; n--) {
            addKey(keySchedule.getRoundKey(n));

            if (n != rounds) {
                mixColumns(mixColMatrixInv);
            }

            shiftRows(true);
            subBytes(subBoxInv);
        }

        addKey(keySchedule.getRoundKey(0));
    }

    std::string asString() const {
        std::string results = "";

        for (int r = 0; r < rows; r++) {
            if (r > 0) {
                results += "\n";
            }

            for (int c = 0; c < cols; c++) {
                if (c > 0) {
                    results += "|";
                }

                results += words[c][r].asChar();
            }
        }

        return results;
    }
};