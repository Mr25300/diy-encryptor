#pragma once

#include "types.hpp"
#include "utils.hpp"
#include "substitution_box.hpp"

#include <math/gf256.hpp>
#include <math/vector.hpp>
#include <math/matrix.hpp>
#include <utils/bytes.hpp>

#include <ostream>

namespace ciphers::rij {
    template <RijPolConcept Pol>
    class KeySchedule {
        std::array<StateBlock<Pol>, Pol::rounds + 1> roundKeys;

        static constexpr std::array<math::GF256, Pol::rounds> roundConstants{[] {
            std::array<math::GF256, Pol::rounds> constants;

            math::GF256 constant{1};

            for (std::size_t i{}; i < Pol::rounds; ++i) {
                constants[i] = constant;
                constant *= 2;
            }

            return constants;
        }()};

        Word& getWord(std::size_t wordInd) {
            return roundKeys[wordInd / Pol::bCols][wordInd % Pol::bCols];
        }

    public:
        KeySchedule(const SubstitutionBox& subBox, const bytes::ByteArr<Pol::kSize>& key) {
            KeyBlock<Pol> keyBlock{utils::getBlockView<Pol::kCols>(key)}; // Avoids mutating key's memory
            std::size_t wordCount{(Pol::rounds + 1) * Pol::bCols};
            std::size_t wordInd{};

            for (; wordInd < Pol::kCols && wordInd < wordCount; ++wordInd) {
                this->getWord(wordInd) = keyBlock[wordInd];
            }

            for (; wordInd < wordCount; ++wordInd) {
                Word& aboveWord{this->getWord(wordInd - Pol::kCols)};
                Word prevTemp{this->getWord(wordInd - 1)};

                if (wordInd % Pol::kCols == 0) {
                    utils::rotWord(prevTemp);
                    utils::subWord(prevTemp, subBox);
                    utils::applyConstant(prevTemp, roundConstants[wordInd / Pol::kCols - 1]);

                } else if (Pol::kCols > 6 && wordInd % Pol::kCols == 4) { // TODO: Generalize this for Rinjdael (replace magic numbers with constants)
                    utils::subWord(prevTemp, subBox);
                }

                this->getWord(wordInd) = aboveWord + prevTemp;
            }
        }

        const StateBlock<Pol>& operator[](std::size_t round) const { return roundKeys[round]; }

        friend std::ostream& operator<<(std::ostream& stream, const KeySchedule<Pol>& keySchedule) {
            for (std::size_t i{}; i < Pol::rounds; ++i) {
                if (i > 0) stream << '\n';

                stream << "Round " << i << ":\n" << keySchedule[i];
            }

            return stream;
        }
    };
}
