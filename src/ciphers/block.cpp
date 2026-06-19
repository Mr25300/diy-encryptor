#include "block.hpp"

namespace ciphers {
    template <std::size_t Size>
    Block<Size> operator^(const Block<Size>& block1, const Block<Size>& block2) {
        Block<Size> newBlock;

        for (std::size_t i{}; i < Size; ++i) {
            newBlock[i] = block1[i] ^ block2[i];
        }

        return newBlock;
    }

    template <std::size_t Size>
    Block<Size>& operator^=(Block<Size>& block1, const Block<Size>& block2) {
        for (std::size_t i{}; i < Size; ++i) {
            block1[i] ^= block2[i];
        }

        return block1;
    }

    template <std::size_t Size>
    std::span<Block<Size>> getBlockView(std::vector<std::uint8_t>& input) {
        return std::span<Block<Size>>(
            reinterpret_cast<ciphers::Block<Size>*>(input.data()), input.size() / Size
        );
    }
}
