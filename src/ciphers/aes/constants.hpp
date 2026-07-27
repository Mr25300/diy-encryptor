#pragma once

#include <cstddef>

namespace ciphers::aes {
    namespace constants {
        static constexpr std::size_t cols{4};
        static constexpr std::size_t rows{4};
        static constexpr std::size_t blockSize{cols * rows};
    }

    template <std::size_t KeyCols, std::size_t Rounds>
    struct AESPolicy {
        // Generalize this to Rijndael and add static assertions for specifications (i.e. block size)

        static constexpr std::size_t keyCols{KeyCols};
        static constexpr std::size_t keySize{keyCols * constants::rows};

        static constexpr std::size_t rounds{Rounds};
    };

    using AES128 = AESPolicy<4, 10>;
    using AES192 = AESPolicy<6, 12>;
    using AES256 = AESPolicy<8, 14>;
}
