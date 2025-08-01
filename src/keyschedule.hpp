#pragma once

#include <array>

#include "word.hpp"
#include "block.hpp"

template <size_t cols, size_t rows>
class Block;

template <size_t cols, size_t rows, size_t rounds>
class KeySchedule {
    std::array<Block<cols, rows>, rounds + 1> roundKeys;

    Word<rows>& getWord(size_t wordIndex) {
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
            Word<rows>& word = this->getWord(currentWord);
            Word<rows>& aboveWord = this->getWord(currentWord - keyWordCount);
            Word<rows> intermediateWord = this->getWord(currentWord - 1);

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

// template <size_t cols, size_t rows, size_t rounds>
// std::array<Block<cols, rows>, rounds + 1> keyExpansion(const std::string key) {
//     size_t keyLength = passwordKey.length();
//     size_t keyWordCount = keyLength / rows;

//     std::array<Block<cols, rows>, rounds + 1> roundKeys;

//     size_t totalWords = (rounds + 1) * cols;
//     size_t currentWord = 0;

//     for (; currentWord < keyWordCount && currentWord < totalWords; currentWord++) {
//         for (int i = 0; i < rows; i++) {
//             int round = currentWord / cols;
//             int col = currentWord % cols;

//             roundKeys[round][col][i] = static_cast<uint8_t>(key[currentWord * rows + i]);
//         }
//     }

//     for (; currentWord < totalWords; currentWord++) {
//         if (currentWord % keyWordCount == 0) {
//             int prevRound = 
//             int round = currentWord / cols;
//         }
//     }

//     for (int c = 0; c < blockColumnCount; c++) {
//         for (int r = 0; r < blockRowCount; r++) {
//             key[c][r] = static_cast<uint8_t>(passwordKey[(r + c * blockRowCount) % passwordLength]);
//         }
//     }

//     for (int c = 0; c < Constants::blockColumns; c++) {
//         for (int r = 0; r < Constants::blockRows; r++) {
//             keys[0][c][r] = static_cast<uint8_t>(passwordKey[(r + c * Constants::blockRows) % passwordLength]);
//         }
//     }

//     for (int n = 0; n < rounds; n++) {
//         for (int c = 0; c < rows; c++) {
//             Word wordToAdd;

//             if (c == 0) {
//                 Word previousWord = keys[n][Constants::blockColumns - 1];
//                 Word mutableWord = previousWord;

//                 mutableWord.rotWord();
//                 mutableWord.subWord(Constants::subBox);
//                 mutableWord.applyConstant(Constants::roundConstants[n]);

//                 wordToAdd = mutableWord;
                
//             } else {
//                 wordToAdd = keys[n + 1][c - 1];
//             }

//             keys[n + 1][c] = keys[n][c] + wordToAdd;
//         }
//     }

//     return keys;
// }