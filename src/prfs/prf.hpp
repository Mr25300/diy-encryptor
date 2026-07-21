#pragma once

#include <utils/bytes.hpp>

#include <cstddef>

namespace prfs {
    template <std::size_t OutputSize>
    struct PRF {
        static constexpr std::size_t outputSize{OutputSize};

        virtual bytes::ByteArr<OutputSize> compute(const bytes::ByteVec& key, const bytes::ByteVec& text) const = 0;
    };
}
