#include <cstddef>

namespace ciphers::aes::constants {
    inline constexpr std::size_t cols{4};
    inline constexpr std::size_t rows{4};
    inline constexpr std::size_t blockSize{cols * rows};

    inline constexpr std::size_t keyCols{4}; // 4, 6, 8
    inline constexpr std::size_t keySize{keyCols * rows};

    inline constexpr std::size_t rounds{10}; // 10, 12, 14
}
