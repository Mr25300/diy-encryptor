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

        static constexpr std::size_t rConCount{
            (Pol::bCols * (Pol::rounds + 1) - 1) / Pol::kCols
        };

        static constexpr std::array<math::GF256, rConCount> roundConstants{[] {
            std::array<math::GF256, rConCount> constants;

            math::GF256 constant{1};

            for (std::size_t i{}; i < rConCount; ++i) {
                constants[i] = constant;
                constant *= 2;
            }

            return constants;
        }()};

        Word& getWord(std::size_t i) {
            return roundKeys[i / Pol::bCols][i % Pol::bCols];
        }

    public:
        KeySchedule(const SubstitutionBox& subBox, const bytes::ByteArr<Pol::kSize>& key) {
            const KeyBlock<Pol>& keyBlock{utils::getBlockView<Pol::kCols>(key)};

            std::size_t words{(Pol::rounds + 1) * Pol::bCols};
            std::size_t i{};

            for (; i < Pol::kCols && i < words; ++i) {
                getWord(i) = keyBlock[i];
            }

            for (; i < words; ++i) {
                const Word& aboveWord{getWord(i - Pol::kCols)};
                Word prevTemp{getWord(i - 1)};

                if (i % Pol::kCols == 0) {
                    utils::rotWord(prevTemp);
                    utils::subWord(prevTemp, subBox);
                    utils::applyConstant(prevTemp, roundConstants[i / Pol::kCols - 1]);

                } else if (Pol::kCols > 6 && i % Pol::kCols == 4) {
                    utils::subWord(prevTemp, subBox);
                }

                this->getWord(i) = aboveWord + prevTemp;
            }
        }

        const StateBlock<Pol>& operator[](std::size_t round) const { return roundKeys[round]; }

        friend std::ostream& operator<<(std::ostream& stream, const KeySchedule<Pol>& keySchedule) {
            for (std::size_t i{}; i < Pol::rounds + 1; ++i) {
                if (i > 0) stream << '\n';

                stream << "Round " << i << ":\n" << keySchedule[i];
            }

            return stream;
        }
    };
}
