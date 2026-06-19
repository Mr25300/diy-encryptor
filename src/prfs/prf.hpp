#pragma once

#include <cstdint>
#include <vector>

namespace prfs {
    struct PRF {
        virtual std::vector<std::uint8_t> compute(const std::vector<std::uint8_t>& key, const std::vector<std::uint8_t>& text) const = 0;
        virtual std::size_t outputSize() const = 0;
    };
}
