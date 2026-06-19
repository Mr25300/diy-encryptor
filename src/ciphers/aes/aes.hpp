#pragma once

#include "types.hpp"
#include "substitution_box.hpp"
#include "key_schedule.hpp"

#include <ciphers/block.hpp>
#include <ciphers/block_cipher.hpp>
#include <math/matrix.hpp>

#include <cassert>

namespace ciphers::aes {
    class AES : public ciphers::BlockCipher<constants::blockSize> {
        const SubstitutionBox& subBox;
        const KeySchedule& keySchedule;

    public:
        AES(const SubstitutionBox& subBox, const KeySchedule& keySchedule) : subBox{subBox}, keySchedule{keySchedule} {}

        void encrypt(ciphers::Block<constants::blockSize>& block) const;
        void decrypt(ciphers::Block<constants::blockSize>& block) const;
    };
}
