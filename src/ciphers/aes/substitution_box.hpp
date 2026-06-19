#pragma once

#include <math/gf256.hpp>

#include <ostream>

namespace ciphers::aes {
    struct SubstitutionBox {
        math::GF256 sub(math::GF256 byte) const;
        math::GF256 subInv(math::GF256 byte) const;
    };

    std::ostream& operator<<(std::ostream& stream, const SubstitutionBox& subBox);
}
