#pragma once

#include "policy.hpp"

#include <math/gf256.hpp>
#include <math/vector.hpp>
#include <math/matrix.hpp>

#include <cstddef>
#include <array>
#include <ostream>

namespace ciphers::rij {
    using Word = math::Vector<math::GF256, rows>;
    using MixColsMatrix = math::Matrix<math::GF256, rows, rows>;

    template <std::size_t Cols>
    using Block = std::array<Word, Cols>;

    template <RijPolConcept Pol>
    using StateBlock = Block<Pol::bCols>;

    template <RijPolConcept Pol>
    using KeyBlock = Block<Pol::kCols>;

    template <std::size_t Cols>
    std::ostream& operator<<(std::ostream& stream, const Block<Cols>& block) {
        for (std::size_t i{}; i < rows; ++i) {
            if (i > 0) stream << '\n';

            for (std::size_t j{}; j < Cols; ++j) {
                if (j == 0) stream << '|';

                stream << block[j][i] << '|';
            }
        }

        return stream;
    }
}
