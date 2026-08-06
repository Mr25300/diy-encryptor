#pragma once

#include "hash.hpp"

#include <utils/bytes.hpp>

namespace hashes {
    struct SHA256 : Hash<64, 32> {
        bytes::ByteArr<32> compute(bytes::ConstByteView input) const;
    };
}
