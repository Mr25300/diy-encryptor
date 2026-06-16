#pragma once

#include "types.hpp"
#include "substitution_box.hpp"
#include "key_schedule.hpp"

#include <ciphers/block_cipher.hpp>

namespace ciphers::aes {
    class AES : ciphers::BlockCipher<constants::blockSize> {
        const SubstitutionBox& subBox;
        const KeySchedule& keySchedule;
        const MixColsMatrix& mixColsMat;

    public:
        constexpr AES(const SubstitutionBox& subBox, const KeySchedule& keySchedule, const MixColsMatrix& mixColsMat) : subBox{subBox}, keySchedule{keySchedule}, mixColsMat{mixColsMat} {}

        void encrypt(std::array<std::uint8_t, constants::blockSize>& block);
        void decrypt(std::array<std::uint8_t, constants::blockSize>& block);
    };
}
