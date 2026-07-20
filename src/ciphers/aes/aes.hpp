#pragma once

#include "substitution_box.hpp"
#include "key_schedule.hpp"

#include <ciphers/block_cipher.hpp>
#include <math/matrix.hpp>
#include <utils/bytes.hpp>

#include <cassert>

namespace ciphers::aes {
    template <typename Pol>
    class AES : public ciphers::BlockCipher<constants::blockSize> {
        const SubstitutionBox& subBox;
        const KeySchedule<Pol>& keySchedule;

    public:
        AES(const SubstitutionBox& subBox, const KeySchedule<Pol>& keySchedule) : subBox{subBox}, keySchedule{keySchedule} {}

        void encrypt(bytes::ByteArr<constants::blockSize>& block) const;
        void decrypt(bytes::ByteArr<constants::blockSize>& block) const;
    };
}
