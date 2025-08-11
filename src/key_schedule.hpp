#pragma once

#include <array>

#include "vector.hpp"
#include "block.hpp"

template <size_t cols, size_t rows>
class Block;

template <size_t cols, size_t rows, size_t rounds>
class KeySchedule {
    std::array<Block<cols, rows>, rounds + 1> roundKeys;

    Vector<rows>& getWord(size_t wordIndex) {
        return roundKeys[wordIndex / cols][wordIndex % cols];
    }

public:
    template <size_t keyWordCount>
    KeySchedule(const Block<keyWordCount, rows>& key, const std::array<GF256, 256>& subBox, const std::array<GF256, rounds>& roundConstants) {
        size_t totalWords = (rounds + 1) * cols;
        size_t currentWord = 0;

        for (; currentWord < keyWordCount && currentWord < totalWords; currentWord++) {
            this->getWord(currentWord) = key[currentWord];
        }

        for (; currentWord < totalWords; currentWord++) {
            Vector<rows>& word = this->getWord(currentWord);
            Vector<rows>& aboveWord = this->getWord(currentWord - keyWordCount);
            Vector<rows> intermediateWord = this->getWord(currentWord - 1);

            if (currentWord % keyWordCount == 0) {
                intermediateWord.rotWord();
                intermediateWord.subWord(subBox);
                intermediateWord.applyConstant(roundConstants[currentWord / keyWordCount - 1]);
            }

            word = aboveWord + intermediateWord;
        }
    }

    const Block<cols, rows>& getRoundKey(size_t round) const {
        return roundKeys[round];
    }
};