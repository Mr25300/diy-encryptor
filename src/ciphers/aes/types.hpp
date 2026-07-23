#pragma once

#include "constants.hpp"

#include <math/gf256.hpp>
#include <math/vector.hpp>
#include <math/matrix.hpp>

#include <cstddef>
#include <array>
#include <ostream>

namespace ciphers::aes {
    using Word = math::Vector<math::GF256, constants::rows>;
    using MixColsMatrix = math::Matrix<math::GF256, constants::rows, constants::rows>;

    template <std::size_t Cols>
    using Block = std::array<Word, Cols>;

    using StateBlock = Block<constants::cols>;

    template <typename Pol>
    using KeyBlock = Block<Pol::keyCols>;

    // TODO: maybe use inline and for other functions too
    template <std::size_t Cols>
    std::ostream& operator<<(std::ostream& stream, const Block<Cols>& block) {
        for (std::size_t i{}; i < constants::rows; ++i) {
            if (i > 0) stream << '\n';

            for (std::size_t j{}; j < Cols; ++j) {
                if (j == 0) stream << '|';

                stream << block[j][i] << '|';
            }
        }

        return stream;
    }
}
