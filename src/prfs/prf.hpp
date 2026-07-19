#pragma once

#include <utils/bytes.hpp>

#include <cstddef>

namespace prfs {
    template <std::size_t OutputSize>
    struct PRF {
        virtual bytes::ByteArr<OutputSize> compute(const bytes::ByteVec& key, const bytes::ByteVec& text) const = 0;

        virtual std::size_t outputSize() const { return OutputSize; }
    };
}
