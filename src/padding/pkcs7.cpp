#include "pkcs7.hpp"

namespace padding {
    template <std::size_t BlockSize>
    void PKCS7<BlockSize>::pad(std::vector<std::uint8_t>& input) const {
        std::size_t padLength{BlockSize - input.size() % BlockSize};

        input.reserve(input.size() + padLength);

        for (std::size_t i{}; i < padLength; ++i) {
            input.push_back(static_cast<std::uint8_t>(padLength));
        }
    }

    template <std::size_t BlockSize>
    bool PKCS7<BlockSize>::unpad(std::vector<std::uint8_t>& input) const {
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
}
