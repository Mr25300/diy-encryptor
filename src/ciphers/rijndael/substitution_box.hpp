#pragma once

#include <math/gf256.hpp>

namespace ciphers::rij {
    struct SubstitutionBox {
        math::GF256 sub(math::GF256 byte) const;
        math::GF256 subInv(math::GF256 byte) const;
    };
}
