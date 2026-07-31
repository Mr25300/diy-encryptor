#pragma once

#include "types.hpp"
#include "substitution_box.hpp"

#include <utils/bytes.hpp>

namespace ciphers::aes::utils {
    void rotWord(Word& word, bool invDir = false);
    void subWord(Word& word, const SubstitutionBox& subBox, bool inverse = false);
    void applyConstant(Word& word, math::GF256 constant);

    // Word& asWord(bytes::ByteArr<constants::rows>& bytes) {
    //     return reinterpret_cast<Word&>(bytes);
    // }

    template <std::size_t Cols>
    Block<Cols>& getBlockView(bytes::ByteArr<Cols * constants::rows>& bytes) {
        return *reinterpret_cast<Block<Cols>*>(bytes.data());
    }

    template <std::size_t Cols>
    const Block<Cols>& getBlockView(const bytes::ByteArr<Cols * constants::rows>& bytes) {
        return *reinterpret_cast<const Block<Cols>*>(bytes.data());
    }

    template <std::size_t Cols>
    bytes::ByteArr<Cols * constants::rows>& getByteView(Block<Cols>& block) {
        return *reinterpret_cast<bytes::ByteArr<Cols * constants::rows>*>(block.data());
    }

    void addKey(StateBlock& block, const StateBlock& key);
    void subBytes(StateBlock& block, const SubstitutionBox& subBox, bool inverse = false);
    void shiftRows(StateBlock& block, bool invDir = false);
    void mixColumns(StateBlock& block, const MixColsMatrix& mat);
}
