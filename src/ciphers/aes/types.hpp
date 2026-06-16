#pragma once

#include <cstddef>
#include <array>
#include <ostream>

#include "constants.hpp"

#include <math/gf256.hpp>
#include <math/vector.hpp>
#include <math/matrix.hpp>

namespace ciphers::aes {
    using Word = math::Vector<math::GF256, constants::rows>;
    using MixColsMatrix = math::Matrix<math::GF256, constants::rows, constants::rows>;

    template <std::size_t Cols>
    using Block = std::array<Word, Cols>;
    using StateBlock = Block<constants::cols>;
    using KeyBlock = Block<constants::keyCols>;

    template <std::size_t Cols>
    std::ostream& operator<<(std::ostream& stream, const Block<Cols>& block); // Maybe use inline and for other functions too
}
