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
    Block<Cols> bytesToBlock(const std::array<std::uint8_t, Cols * constants::rows>& bytes);

    template <std::size_t Cols>
    void blockToBytes(const Block<Cols>& block, std::array<std::uint8_t, Cols * constants::rows>& bytes);

    void addKey(StateBlock& block, const StateBlock& key);
    void subBytes(StateBlock& block, const SubstitutionBox& subBox, bool inverse = false);
    void shiftRows(StateBlock& block, bool invDir = false);
    void mixColumns(StateBlock& block, const MixColsMatrix& mat);
}
