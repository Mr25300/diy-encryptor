#pragma once

#include <array>
#include <ostream>

#include "types.hpp"
#include "substitution_box.hpp"

#include <math/gf256.hpp>
#include <math/vector.hpp>
#include <math/matrix.hpp>

namespace ciphers::aes {
    class KeySchedule {
        const SubstitutionBox& subBox;

        std::array<math::GF256, constants::rounds> roundConstants;
        std::array<StateBlock, constants::rounds + 1> roundKeys;

        Word& getWord(std::size_t wordInd) {
            return roundKeys[wordInd / constants::rows][wordInd % constants::rows];
        }

    public:
        constexpr KeySchedule(const SubstitutionBox& subBox) : subBox{subBox} {
            math::GF256 constant{1};
            math::GF256 two{2};

            for (std::size_t i{}; i < constants::rounds; ++i) {
                this->roundConstants[i] = constant;
                constant *= two;
            }
        }

        const StateBlock& operator[](std::size_t round) const { return roundKeys[round]; }

        void generate(const std::array<std::uint8_t, constants::keySize>& key);

        friend std::ostream& operator<<(std::ostream& stream, const KeySchedule& keySchedule);
    };
}
