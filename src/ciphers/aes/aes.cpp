#include "aes.hpp"
#include "types.hpp"
#include "utils.hpp"

namespace ciphers::aes {
    void AES::encrypt(std::array<std::uint8_t, constants::blockSize>& block) {
        StateBlock aesBlock{utils::bytesToBlock<constants::cols>(block)};

        utils::addKey(aesBlock, this->keySchedule[0]);

        for (std::size_t i{1}; i <= constants::rounds; ++i) {
            utils::subBytes(aesBlock, this->subBox);
            utils::shiftRows(aesBlock);

            if (i != constants::rounds) utils::mixColumns(aesBlock, this->mixColsMat);

            utils::addKey(aesBlock, keySchedule[i]);
        }

        utils::blockToBytes(aesBlock, block);
    }

    void AES::decrypt(std::array<std::uint8_t, constants::blockSize>& block) {
        StateBlock aesBlock{utils::bytesToBlock<constants::cols>(block)};

        for (std::size_t i{constants::rounds}; i >= 1; --i) {
            utils::addKey(aesBlock, keySchedule[i]);

            if (i != constants::rounds) utils::mixColumns(aesBlock, this->mixColsMat);

            utils::shiftRows(aesBlock, true);
            utils::subBytes(aesBlock, this->subBox, true);
        }

        utils::addKey(aesBlock, keySchedule[0]);
    }
}
