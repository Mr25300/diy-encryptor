#pragma once

#include "constants.hpp"
#include "types.hpp"
#include "utils.hpp"
#include "substitution_box.hpp"

#include <math/gf256.hpp>
#include <math/vector.hpp>
#include <math/matrix.hpp>
#include <utils/bytes.hpp>

#include <ostream>

namespace ciphers::aes {
    template <typename Pol>
    class KeySchedule {
        std::array<StateBlock, Pol::rounds + 1> roundKeys;

        static constexpr std::array<math::GF256, Pol::rounds> roundConstants{[] { // TODO: See if inline should be used here
            std::array<math::GF256, Pol::rounds> constants;

            math::GF256 constant{1};

            for (std::size_t i{}; i < Pol::rounds; ++i) {
                constants[i] = constant;
                constant *= 2;
            }

            return constants;
        }()};

        Word& getWord(std::size_t wordInd) {
            return roundKeys[wordInd / constants::rows][wordInd % constants::rows];
        }

    public:
        KeySchedule(const SubstitutionBox& subBox, const bytes::ByteArr<Pol::keySize>& key) {
            KeyBlock<Pol> keyBlock{utils::bytesToBlock<Pol::keyCols>(key)};
            std::size_t wordCount{(Pol::rounds + 1) * constants::cols};
            std::size_t wordInd{};

            for (; wordInd < Pol::keyCols && wordInd < wordCount; ++wordInd) {
                this->getWord(wordInd) = keyBlock[wordInd];
            }

            for (; wordInd < wordCount; ++wordInd) {
                Word& aboveWord{this->getWord(wordInd - Pol::keyCols)};
                Word intermediateWord{this->getWord(wordInd - 1)};

                if (wordInd % Pol::keyCols == 0) {
                    utils::rotWord(intermediateWord);
                    utils::subWord(intermediateWord, subBox);
                    utils::applyConstant(intermediateWord, roundConstants[wordInd / Pol::keyCols - 1]);
                }

                this->getWord(wordInd) = aboveWord + intermediateWord;
            }
        }

        const StateBlock& operator[](std::size_t round) const { return roundKeys[round]; }

        friend std::ostream& operator<<(std::ostream& stream, const KeySchedule<Pol>& keySchedule) {
            for (std::size_t i{}; i < Pol::rounds; ++i) {
                if (i > 0) stream << '\n';

                stream << "Round " << i << ":\n" << keySchedule[i];
            }

            return stream;
        }
    };
}
