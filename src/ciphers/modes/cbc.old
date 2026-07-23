#include "cbc.hpp"

#include <vector>

namespace ciphers::modes {
    template <std::size_t BlockSize>
    void CBC<BlockSize>::encrypt(bytes::ByteVec& input) const {
        this->padder.pad(input);

        std::vector<bytes::ByteArr<BlockSize>> blocks{bytes::getChunks<BlockSize>(input)};
        const bytes::ByteArr<BlockSize>* prevBlock{&(this->iv)};

        for (bytes::ByteArr<BlockSize>& block : blocks) {
            bytes::getXorBytes(block, *prevBlock);
            this->cipher.encrypt(block);

            prevBlock = &block;
        }
    }

    template <std::size_t BlockSize>
    bool CBC<BlockSize>::decrypt(bytes::ByteVec& input) const {
        if (input.size() % BlockSize != 0) return false;

        std::vector<bytes::ByteArr<BlockSize>> blocks{bytes::getChunks<BlockSize>(input)};
        const bytes::ByteArr<BlockSize> prevBlock{this->iv};

        for (bytes::ByteArr<BlockSize>& block : blocks) {
            bytes::ByteArr<BlockSize> currBlock{block};

            this->cipher.decrypt(block);
            bytes::getXorBytes(block, currBlock);

            prevBlock = block;
        }

        return this->padder.unpad(input);
    }
}
