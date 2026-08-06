#pragma once

#include <utils/bytes.hpp>

#include <cstddef>

namespace prfs {
    template <std::size_t OutputSize>
    struct PRF {
        virtual bytes::ByteArr<OutputSize> compute(bytes::ConstByteView key, bytes::ConstByteView text) const = 0;
    };
}
