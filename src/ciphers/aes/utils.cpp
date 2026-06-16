#include "utils.hpp"
#include "types.hpp"

#include <math/gf256.hpp>
#include <math/utils.hpp>

namespace ciphers::aes::utils {
    void rotWord(Word& word, bool invDir) {
        int start{invDir ? static_cast<int>(constants::rows - 1) : 0};
        int end{invDir ? 0 : static_cast<int>(constants::rows - 1)};
        int direction{invDir ? -1 : 1};

        math::GF256 temp{word[start]};

        for (int i{start}; i != end; i += direction) {
            word[i] = word[i + direction];
        }

        word[end] = temp;
    }

    void subWord(Word& word, const SubstitutionBox& subBox, bool inverse) {
        for (std::size_t i{}; i < constants::rows; ++i) {
            word[i] = inverse ? subBox.subInv(word[i]) : subBox.sub(word[i]);
        }
    }

    void applyConstant(Word &word, math::GF256 constant) {
        word[0] += constant;
    }

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

    void addKey(StateBlock &block, const StateBlock &key) {
        for (std::size_t i{}; i < constants::cols; ++i) {
            block[i] += key[i];
        }
    }

    void subBytes(StateBlock &block, const SubstitutionBox &subBox, bool inverse) {
        for (Word& word : block) {
            subWord(word, subBox, inverse);
        }
    }

    void shiftRows(StateBlock& block, bool invDir) {
        Block temp{block};

        for (std::size_t i{}; i < constants::rows; ++i) {
            std::size_t modShift{i % constants::cols}; // Shifting by i is equivalent to i + cols
            std::size_t shift{invDir ? constants::cols - modShift : modShift};

            for (std::size_t j{}; j < constants::cols; ++j) {
                std::size_t newCol{(j + shift) % constants::cols};

                block[j][i] = temp[newCol][i];
            }
        }
    }

    void mixColumns(StateBlock &block, const MixColsMatrix& mat) {
        for (Word& word : block) {
            word *= mat;
        }
    }
}
