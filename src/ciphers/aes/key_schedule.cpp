#include "key_schedule.hpp"

#include "utils.hpp"

namespace ciphers::aes {
    template <typename Pol>
    constexpr bytes::ByteArr<Pol::rounds> roundConstants{[] {
        bytes::ByteArr<Pol::rounds> constants;

        math::GF256 constant{1};
        math::GF256 two{2};

        for (std::size_t i{}; i < Pol::rounds; ++i) {
            constants[i] = constant;
            constant *= two;
        }

        return constants;
    }()};

    template <typename Pol>
    void KeySchedule<Pol>::generate(const bytes::ByteArr<Pol::keySize>& key) {
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
                utils::applyConstant(intermediateWord, roundConstants<Pol>[wordInd / Pol::keyCols - 1]);
            }

            this->getWord(wordInd) = aboveWord + intermediateWord;
        }
    }

    template <typename Pol>
    std::ostream& operator<<(std::ostream& stream, const KeySchedule<Pol>& keySchedule) {
        for (std::size_t i{}; i < Pol::rounds; ++i) {
            if (i > 0) stream << '\n';

            stream << "Round " << i << ":\n" << keySchedule[i];
        }

        return stream;
    }
}
