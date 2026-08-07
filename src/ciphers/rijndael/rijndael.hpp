#pragma once

#include "substitution_box.hpp"
#include "key_schedule.hpp"

#include <ciphers/block_cipher.hpp>
#include <math/matrix.hpp>
#include <utils/bytes.hpp>

#include <cassert>

namespace ciphers::rij {
    namespace {
        inline constexpr MixColsMatrix mixColsMat{MixColsMatrix::getCirculantMatrix(
            math::Vector<math::GF256, rows>({2, 3, 1, 1})
        )};

        inline constexpr MixColsMatrix mixColsMatInv{[] {
            auto invRes{mixColsMat.getInverse<rows>()};
            assert(!invRes.isSingular && "AES MixCols Matrix must be invertible!");

            return invRes.mat;
        }()};
    }

    template <RijPolConcept Pol>
    class Rijndael : public ciphers::BlockCipher<Pol::bSize> {
        const SubstitutionBox& subBox;
        const KeySchedule<Pol>& keySchedule;

    public:
        Rijndael(const SubstitutionBox& subBox, const KeySchedule<Pol>& keySchedule) : subBox{subBox}, keySchedule{keySchedule} {}

        void encrypt(bytes::ByteArr<Pol::bSize>& block) const {
            StateBlock<Pol>& aesBlock{utils::getBlockView<Pol::bCols>(block)};

            utils::addKey<Pol>(aesBlock, keySchedule[0]);

            for (std::size_t i{1}; i <= Pol::rounds; ++i) {
                utils::subBytes<Pol>(aesBlock, subBox);
                utils::shiftRows<Pol>(aesBlock);

                if (i != Pol::rounds) utils::mixColumns<Pol>(aesBlock, mixColsMat);

                utils::addKey<Pol>(aesBlock, keySchedule[i]);
            }
        }

        void decrypt(bytes::ByteArr<Pol::bSize>& block) const {
            StateBlock<Pol>& aesBlock{utils::getBlockView<Pol::bCols>(block)};

            for (std::size_t i{Pol::rounds}; i >= 1; --i) {
                utils::addKey<Pol>(aesBlock, keySchedule[i]);

                if (i != Pol::rounds) utils::mixColumns<Pol>(aesBlock, mixColsMatInv);

                utils::shiftRows<Pol>(aesBlock, true);
                utils::subBytes<Pol>(aesBlock, subBox, true);
            }

            utils::addKey<Pol>(aesBlock, keySchedule[0]);
        }
    };
}
