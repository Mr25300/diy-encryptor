#pragma once

#include "padding_scheme.hpp"

#include <utils/bytes.hpp>

namespace padding {
    template <std::size_t BlockSize>
    struct PKCS7 : PaddingScheme<BlockSize> {
        void pad(bytes::ByteVec& input) const {
            std::size_t padLength{BlockSize - input.size() % BlockSize};

            input.reserve(input.size() + padLength);

            for (std::size_t i{}; i < padLength; ++i) {
                input.push_back(static_cast<std::uint8_t>(padLength));
            }
        }

        bool unpad(bytes::ByteVec& input) const {
            std::uint8_t padLength{input.back()};

            if (padLength == 0 || padLength > BlockSize || padLength > input.size())
                return false;

            std::size_t paddingStart{input.size() - padLength};

            for (std::size_t i{paddingStart}; i < input.size(); ++i) {
                if (input[i] != padLength) return false;
            }

            input.resize(paddingStart);

            return true;
        }
    };
}
