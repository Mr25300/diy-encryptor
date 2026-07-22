#pragma once

#include "constants.hpp"
#include "types.hpp"
#include "substitution_box.hpp"

#include <math/gf256.hpp>
#include <math/vector.hpp>
#include <math/matrix.hpp>
#include <utils/bytes.hpp>

#include <ostream>

namespace ciphers::aes {
    template <typename Pol>
    class KeySchedule {
        const SubstitutionBox& subBox;

        std::array<StateBlock, Pol::rounds + 1> roundKeys;

        Word& getWord(std::size_t wordInd) {
            return roundKeys[wordInd / constants::rows][wordInd % constants::rows];
        }

    public:
        KeySchedule(const SubstitutionBox& subBox, const bytes::ByteArr<Pol::keySize>& key);

        const StateBlock& operator[](std::size_t round) const { return roundKeys[round]; }

        friend std::ostream& operator<<(std::ostream& stream, const KeySchedule& keySchedule);
    };
}
