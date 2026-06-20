#pragma once

#include <cstddef>
#include <cstdint>
#include <array>
#include <vector>
#include <span>

namespace ciphers {
    template <std::size_t Size>
    using Block = std::array<std::uint8_t, Size>;

    // TODO: Remove (bad idea, replace with explicit functions)
    template <std::size_t Size>
    Block<Size> operator^(const Block<Size>& block1, const Block<Size>& block2);
    template <std::size_t Size>
    Block<Size>& operator^=(Block<Size>& block1, const Block<Size>& block2);

    template <std::size_t Size>
    std::span<Block<Size>> getBlockView(std::vector<std::uint8_t>& input);
}
