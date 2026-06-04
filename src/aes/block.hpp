#pragma once

#include <ostream>
#include <vector>
#include <cstdint>
#include <stdexcept>

#include <math/utils.hpp>
#include <math/word.hpp>
#include <math/matrix.hpp>
#include "substitution_box.hpp"
#include "key_schedule.hpp"

template <size_t cols, size_t rows, size_t rounds>
class KeySchedule;

template <size_t cols, size_t rows>
class Block {
    std::array<math::Word<rows>, cols> words;

public:
    Block() : words{} {}
    Block(std::array<math::Word<rows>, cols> values) : words(values) {}

    static Block<cols, rows> fromBytes(std::vector<std::uint8_t>& bytes) {
        if (bytes.size() != cols * rows) {
            throw std::invalid_argument("Length of bytes does not match block size.");
        }

        Block<cols, rows> block;

        for (int c{}; c < cols; ++c) {
            for (int r{}; r < rows; ++r) {
                block.words[c][r] = bytes[c * rows + r];
            }
        }

        return block;
    }

    const math::Word<rows>& operator[](uint8_t index) const {
        return words[index];
    }

    math::Word<rows>& operator[](uint8_t index) {
        return words[index];
    }

    void addKey(const Block<cols, rows>& key) {
        for (int i = 0; i < cols; i++) {
            words[i] += key.words[i];
        }
    }

    void subBytes(const SubstitutionBox& subBox, bool inverse = false) {
        for (int i = 0; i < cols; i++) {
            words[i].subWord(subBox, inverse);
        }
    }

    void shiftRows(bool invDir = false) {
        std::array<math::Word<rows>, cols> tempValues{words};

        int direction = invDir ? -1 : 1;

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                int shiftedCol{math::utils::properMod(c + r * direction, cols)}; // Shift row an amount equal to the corresponding row number in the correct direction

                words[c][r] = tempValues[shiftedCol][r];
            }
        }
    }

    void mixColumns(const math::Matrix<rows>& mat) {
        for (int i = 0; i < cols; i++) {
            words[i] *= mat;
        }
    }

    template <size_t rounds>
    void encrypt(const KeySchedule<cols, rows, rounds>& keySchedule, const SubstitutionBox& subBox, const math::Matrix<rows>& mixColMatrix) {
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
    void decrypt(const KeySchedule<cols, rows, rounds>& keySchedule, const SubstitutionBox& subBox, const math::Matrix<rows>& mixColMatrixInv) {
        for (int n = rounds; n >= 1; n--) {
            addKey(keySchedule.getRoundKey(n));

            if (n != rounds) {
                mixColumns(mixColMatrixInv);
            }

            shiftRows(true);
            subBytes(subBox, true);
        }

        addKey(keySchedule.getRoundKey(0));
    }

    void print(std::ostream& stream, math::GFFormat format = math::GFFormat::Hex) const {
        for (int r = 0; r < rows; r++) {
            if (r > 0) stream << '\n';

            for (int c = 0; c < cols; c++) {
                if (c == 0) stream << '|';

                words[c][r].print(stream, format);
                stream << '|';
            } 
        }
    }

    friend std::ostream& operator<<(std::ostream& stream, const Block<cols, rows>& block) {
        block.print(stream);

        return stream;
    }
};
