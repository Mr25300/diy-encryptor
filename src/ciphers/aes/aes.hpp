#pragma once

#include "substitution_box.hpp"
#include "key_schedule.hpp"

#include <ciphers/block_cipher.hpp>
#include <math/matrix.hpp>
#include <utils/bytes.hpp>

#include <cassert>
#include <iostream>

namespace ciphers::aes {
    namespace {
        inline constexpr MixColsMatrix mixColsMat{MixColsMatrix::getCirculantMatrix(
            math::Vector<math::GF256, constants::rows>({2, 3, 1, 1})
        )};

        inline constexpr MixColsMatrix mixColsMatInv{[] {
            auto invRes{mixColsMat.getInverse<constants::rows>()};
            assert(!invRes.isSingular && "AES MixCols Matrix must be invertible!");

            return invRes.mat;
        }()};
    }

    template <typename Pol>
    class AES : public ciphers::BlockCipher<constants::blockSize> {
        const SubstitutionBox& subBox;
        const KeySchedule<Pol>& keySchedule;

    public:
        AES(const SubstitutionBox& subBox, const KeySchedule<Pol>& keySchedule) : subBox{subBox}, keySchedule{keySchedule} {}

        void encrypt(bytes::ByteArr<constants::blockSize>& block) const {
            StateBlock aesBlock{utils::bytesToBlock<constants::cols>(block)};

            utils::addKey(aesBlock, keySchedule[0]);

            for (std::size_t i{1}; i <= Pol::rounds; ++i) {
                utils::subBytes(aesBlock, subBox);
                utils::shiftRows(aesBlock);

                if (i != Pol::rounds) utils::mixColumns(aesBlock, mixColsMat);

                utils::addKey(aesBlock, keySchedule[i]);
            }

            utils::blockToBytes(aesBlock, block);
        }

        void decrypt(bytes::ByteArr<constants::blockSize>& block) const {
            StateBlock aesBlock{utils::bytesToBlock<constants::cols>(block)};

            for (std::size_t i{Pol::rounds}; i >= 1; --i) {
                utils::addKey(aesBlock, keySchedule[i]);

                if (i != Pol::rounds) utils::mixColumns(aesBlock, mixColsMatInv);

                utils::shiftRows(aesBlock, true);
                utils::subBytes(aesBlock, subBox, true);
            }

            utils::addKey(aesBlock, keySchedule[0]);

            utils::blockToBytes(aesBlock, block);
        }
    };
}
