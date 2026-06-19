#pragma once

#include "types.hpp"
#include "substitution_box.hpp"

#include <math/gf256.hpp>
#include <math/vector.hpp>
#include <math/matrix.hpp>

#include <array>
#include <ostream>

namespace ciphers::aes {
    class KeySchedule {
        const SubstitutionBox& subBox;

        std::array<StateBlock, constants::rounds + 1> roundKeys;

        Word& getWord(std::size_t wordInd) {
            return roundKeys[wordInd / constants::rows][wordInd % constants::rows];
        }

    public:
        KeySchedule(const SubstitutionBox& subBox) : subBox{subBox} {}

        const StateBlock& operator[](std::size_t round) const { return roundKeys[round]; }

        void generate(const std::array<std::uint8_t, constants::keySize>& key);

        friend std::ostream& operator<<(std::ostream& stream, const KeySchedule& keySchedule);
    };
}
