#pragma once

#include <vector>
#include <cstdint>

namespace hashes {
    class Hash {
    public:
        virtual std::vector<std::uint8_t> compute(const std::vector<std::uint8_t>& input) const = 0;
        virtual std::size_t blockSize() const = 0;
        virtual std::size_t outputSize() const = 0;
    };
}
