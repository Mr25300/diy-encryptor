#pragma once

#include <utils/bytes.hpp>

#include <cstddef>

namespace prfs {
    template <std::size_t OutputSize>
    struct PRF {
        virtual bytes::ByteArr<OutputSize> compute(const bytes::ByteVec& key, const bytes::ByteVec& text) const = 0;
    };
}
