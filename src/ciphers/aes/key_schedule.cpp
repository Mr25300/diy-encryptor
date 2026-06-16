#include "key_schedule.hpp"
#include "utils.hpp"

namespace ciphers::aes {
    void KeySchedule::generate(const std::array<std::uint8_t, constants::keySize>& key) {
        KeyBlock keyBlock{utils::bytesToBlock<constants::keyCols>(key)};
        std::size_t wordCount{(constants::rounds + 1) * constants::cols};
        std::size_t wordInd{};

        for (; wordInd < constants::keyCols && wordInd < wordCount; ++wordInd) {
            this->getWord(wordInd) = keyBlock[wordInd];
        }

        for (; wordInd < wordCount; ++wordInd) {
            Word& word{this->getWord(wordInd)};
            Word& aboveWord{this->getWord(wordInd - constants::keyCols)};
            Word intermediateWord{this->getWord(wordInd - 1)};

            if (wordInd % constants::keyCols == 0) {
                utils::rotWord(intermediateWord);
                utils::subWord(intermediateWord, subBox);
                utils::applyConstant(intermediateWord, roundConstants[wordInd / constants::keyCols - 1]);
            }

            word = aboveWord + intermediateWord;
        }
    }

    std::ostream& operator<<(std::ostream& stream, const KeySchedule& keySchedule) {
        for (std::size_t i{}; i < constants::rounds; ++i) {
            if (i > 0) stream << '\n';

            stream << "Round " << i << ":\n" << keySchedule[i];
        }

        return stream;
    }
}

