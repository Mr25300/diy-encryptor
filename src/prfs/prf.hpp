#pragma once

#include <utils/bytes.hpp>

#include <cstddef>

namespace prfs {
    struct PRF {
        virtual bytes::ByteVec compute(const bytes::ByteVec& key, const bytes::ByteVec& text) const = 0;

        virtual std::size_t outputSize() const = 0;
    };
}
