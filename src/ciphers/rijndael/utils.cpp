#include "utils.hpp"
#include "types.hpp"

#include <math/gf256.hpp>

namespace ciphers::rij::utils {
    void rotWord(Word& word, bool invDir) {
        int start{invDir ? static_cast<int>(rows - 1) : 0};
        int end{invDir ? 0 : static_cast<int>(rows - 1)};
        int direction{invDir ? -1 : 1};

        math::GF256 temp{word[start]};

        for (int i{start}; i != end; i += direction) {
            word[i] = word[i + direction];
        }

        word[end] = temp;
    }

    void subWord(Word& word, const SubstitutionBox& subBox, bool inverse) {
        for (std::size_t i{}; i < rows; ++i) {
            word[i] = inverse ? subBox.subInv(word[i]) : subBox.sub(word[i]);
        }
    }

    void applyConstant(Word &word, math::GF256 constant) {
        word[0] += constant;
    }
}
