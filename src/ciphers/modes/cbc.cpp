#include "cbc.hpp"

#include <cstring>
#include <span>

namespace ciphers::modes {
    template <std::size_t BlockSize>
    void CBC<BlockSize>::encrypt(std::vector<std::uint8_t>& input) const {
        this->padder.pad(input);

        std::span<ciphers::Block<BlockSize>> blockView{ciphers::getBlockView<BlockSize>(input)};
        const ciphers::Block<BlockSize>* prevBlock{&(this->iv)};

        for (ciphers::Block<BlockSize>& block : blockView) {
            block ^= *prevBlock;
            this->cipher.encrypt(block);

            prevBlock = &block;
        }
    }

    template <std::size_t BlockSize>
    bool CBC<BlockSize>::decrypt(std::vector<std::uint8_t>& input) const {
        if (input.size() % BlockSize != 0) return false;

        std::span<ciphers::Block<BlockSize>> blockView{ciphers::getBlockView<BlockSize>(input)};
        const ciphers::Block<BlockSize> prevBlock{this->iv};

        for (ciphers::Block<BlockSize>& block : blockView) {
            ciphers::Block<BlockSize> currBlock{block};

            this->cipher.decrypt(block);
            block ^= currBlock;

            prevBlock = block;
        }

        return this->padder.unpad(input);
    }
}
