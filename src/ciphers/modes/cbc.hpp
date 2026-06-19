#pragma once

#include <padding/padding_scheme.hpp>
#include <ciphers/block.hpp>
#include <ciphers/block_cipher.hpp>

#include <cstddef>
#include <vector>

namespace ciphers::modes {
    template <std::size_t BlockSize>
    class CBC {
        const padding::PaddingScheme<BlockSize>& padder;
        const ciphers::BlockCipher<BlockSize>& cipher;
        ciphers::Block<BlockSize> iv;

    public:
        CBC(const padding::PaddingScheme<BlockSize>& padder, const ciphers::BlockCipher<BlockSize>& cipher, const ciphers::Block<BlockSize>& iv) : padder{padder}, cipher{cipher}, iv{iv} {}

        void encrypt(std::vector<std::uint8_t>& input) const;
        bool decrypt(std::vector<std::uint8_t>& input) const;

    //     CBC(const std::string& text, bool encrypted) {
    //         size_t blockSize = cols * rows;
    //         size_t textLength = text.length();
    //
    //         size_t padLength = blockSize - textLength % blockSize;
    //         if (encrypted) padLength -= blockSize;
    //         else if (padLength == 0) padLength = blockSize;
    //
    //         size_t blockCount = (textLength + padLength) / blockSize;
    //
    //         for (int b = 0; b < blockCount; b++) {
    //             Block<cols, rows> block;
    //
    //             for (int i = 0; i < blockSize; i++) {
    //                 int charIndex = b * blockSize + i;
    //
    //                 block[i / rows][i % rows] = charIndex < textLength ? static_cast<math::GF256>(text[charIndex]) : static_cast<math::GF256>(padLength);
    //             }
    //
    //             blocks.push_back(block);
    //         }
    //     }
    //
    //     template <size_t rounds>
    //     void encrypt(const KeySchedule<cols, rows, rounds>& keySchedule, const SubstitutionBox& subBox, const math::Matrix<rows>& mixColMatrix, const Block<cols, rows>& ivBlock) {
    //         Block<cols, rows> prevBlock = ivBlock;
    //
    //         for (Block<cols, rows>& block : blocks) {
    //             block.addKey(prevBlock);
    //             block.encrypt(keySchedule, subBox, mixColMatrix);
    //
    //             prevBlock = block;
    //         }
    //     }
    //
    //     template <size_t rounds>
    //     void decrypt(const KeySchedule<cols, rows, rounds>& keySchedule, const SubstitutionBox& subBox, const math::Matrix<rows>& mixColMatrixInv, const Block<cols, rows>& ivBlock) {
    //         Block<cols, rows> prevBlock = ivBlock;
    //
    //         for (Block<cols, rows>& block : blocks) {
    //             Block<cols, rows> cipherBlock = block;
    //
    //             block.decrypt(keySchedule, subBox, mixColMatrixInv);
    //             block.addKey(prevBlock);
    //
    //             prevBlock = cipherBlock;
    //         }
    //     }
    //
    //     std::string getText(bool removePKCS7PPadding = false) {
    //         std::string text;
    //         text.reserve(blocks.size() * cols * rows);
    //
    //         for (Block<cols, rows> block : blocks) {
    //             for (int c = 0; c < cols; c++) {
    //                 for (int r = 0; r < rows; r++) {
    //                     text += static_cast<char>(block[c][r].get());
    //                 }
    //             }
    //         }
    //
    //         if (removePKCS7PPadding) {
    //             uint8_t padLength = static_cast<uint8_t>(text[text.length() - 1]);
    //             bool validPad = true;
    //
    //             if (padLength > text.size()) {
    //                 validPad = false;
    //             } else {
    //                 for (size_t i = text.size() - padLength; i < text.size(); i++) {
    //                     if (static_cast<uint8_t>(text[i]) != padLength) {
    //                         validPad = false;
    //
    //                         break;
    //                     }
    //                 }
    //             }
    //
    //             if (validPad) {
    //                 text.erase(text.end() - padLength, text.end());
    //             }
    //         }
    //
    //         return text;
    //     }
    };
}
