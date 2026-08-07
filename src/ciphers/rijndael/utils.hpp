#pragma once

#include "types.hpp"
#include "substitution_box.hpp"

#include <utils/bytes.hpp>

namespace ciphers::rij::utils {
    void rotWord(Word& word, bool invDir = false);
    void subWord(Word& word, const SubstitutionBox& subBox, bool inverse = false);
    void applyConstant(Word& word, math::GF256 constant);

    // Word& asWord(bytes::ByteArr<constants::rows>& bytes) {
    //     return reinterpret_cast<Word&>(bytes);
    // }

    template <std::size_t Cols>
    Block<Cols>& getBlockView(bytes::ByteArr<Cols * rows>& bytes) {
        return *reinterpret_cast<Block<Cols>*>(bytes.data());
    }

    template <std::size_t Cols>
    const Block<Cols>& getBlockView(const bytes::ByteArr<Cols * rows>& bytes) {
        return *reinterpret_cast<const Block<Cols>*>(bytes.data());
    }

    template <std::size_t Cols>
    bytes::ByteArr<Cols * rows>& getByteView(Block<Cols>& block) {
        return *reinterpret_cast<bytes::ByteArr<Cols * rows>*>(block.data());
    }

    template <RijPolConcept Pol>
    void addKey(StateBlock<Pol>& block, const StateBlock<Pol>& key) {
        for (std::size_t i{}; i < Pol::bCols; ++i) {
            block[i] += key[i];
        }
    }

    template <RijPolConcept Pol>
    void subBytes(StateBlock<Pol>& block, const SubstitutionBox& subBox, bool inverse = false) {
        for (Word& word : block) {
            subWord(word, subBox, inverse);
        }
    }

    template <RijPolConcept Pol>
    void shiftRows(StateBlock<Pol>& block, bool invDir = false) {
        StateBlock<Pol> temp{block};

        for (std::size_t i{}; i < rows; ++i) {
            std::size_t offset{i};

            if (
                // Pol::bCols == 7 && i > rows / 2 || // Unnecessary case
                Pol::bCols == 8 && i >= rows / 2
            ) {
                offset += 1;
            }

            std::size_t modShift{offset % Pol::bCols}; // Shifting by offset is equivalent to offset + cols
            std::size_t shift{invDir ? Pol::bCols - modShift : modShift};

            for (std::size_t j{}; j < Pol::bCols; ++j) {
                std::size_t newCol{(j + shift) % Pol::bCols};

                block[j][i] = temp[newCol][i];
            }
        }
    }

    template <RijPolConcept Pol>
    void mixColumns(StateBlock<Pol>& block, const MixColsMatrix& mat) {
        for (Word& word : block) {
            word *= mat;
        }
    }
}
