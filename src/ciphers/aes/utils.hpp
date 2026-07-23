#pragma once

#include "types.hpp"
#include "substitution_box.hpp"

#include <cstdint>
#include <array>

namespace ciphers::aes::utils {
    void rotWord(Word& word, bool invDir = false);
    void subWord(Word& word, const SubstitutionBox& subBox, bool inverse = false);
    void applyConstant(Word& word, math::GF256 constant);

    template <std::size_t Cols>
    Block<Cols> bytesToBlock(const std::array<std::uint8_t, Cols * constants::rows>& bytes) {
        Block<Cols> block;

        for (std::size_t j{}; j < Cols; ++j) {
            for (std::size_t i{}; i < constants::rows; ++i) {
                block[j][i] = bytes[j * constants::rows + i];
            }
        }

        return block;
    }

    template <std::size_t Cols>
    void blockToBytes(const Block<Cols>& block, std::array<std::uint8_t, Cols * constants::rows>& bytes) {
        for (std::size_t j{}; j < Cols; ++j) {
            for (std::size_t i{}; i < constants::rows; ++i) {
                bytes[j * constants::rows + i] = block[j][i].value;
            }
        }
    }

    void addKey(StateBlock& block, const StateBlock& key);
    void subBytes(StateBlock& block, const SubstitutionBox& subBox, bool inverse = false);
    void shiftRows(StateBlock& block, bool invDir = false);
    void mixColumns(StateBlock& block, const MixColsMatrix& mat);
}
