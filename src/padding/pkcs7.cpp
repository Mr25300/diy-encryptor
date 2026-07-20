#include "pkcs7.hpp"

namespace padding {
    void PKCS7::pad(std::vector<std::uint8_t>& input) const {
        std::size_t padLength{blockSize - input.size() % blockSize};

        input.reserve(input.size() + padLength);

        for (std::size_t i{}; i < padLength; ++i) {
            input.push_back(static_cast<std::uint8_t>(padLength));
        }
    }

    bool PKCS7::unpad(std::vector<std::uint8_t>& input) const {
        std::uint8_t padLength{input.back()};

        if (padLength == 0 || padLength > blockSize || padLength > input.size())
            return false;

        std::size_t paddingStart{input.size() - padLength};

        for (std::size_t i{paddingStart}; i < input.size(); ++i) {
            if (input[i] != padLength) return false;
        }

        input.resize(paddingStart);

        return true;
    }
}
