#pragma once

#include <cstddef>
#include <algorithm>

namespace ciphers::rij {
    inline constexpr std::size_t rows{4};

    template <typename T>
    concept RijPolConcept = requires {
        { T::bCols } -> std::same_as<const std::size_t&>;
        { T::bSize } -> std::same_as<const std::size_t&>;
        { T::kCols } -> std::same_as<const std::size_t&>;
        { T::kSize } -> std::same_as<const std::size_t&>;
        { T::rounds } -> std::same_as<const std::size_t&>;
    };

    template <std::size_t BCols, std::size_t KCols>
    struct RijndaelPolicy {
        // Generalize this to Rijndael and add static assertions for specifications (i.e. block size)
        static_assert(BCols >= 4 && BCols <= 8, "Invalid column count for Rijndael specifications.");
        static_assert(KCols >= 4 && KCols <= 8, "Invalid key column count for Rijndael specifications.");

        static constexpr std::size_t bCols{BCols};
        static constexpr std::size_t bSize{bCols * rows};

        static constexpr std::size_t kCols{KCols};
        static constexpr std::size_t kSize{kCols * rows};

        static constexpr std::size_t rounds{std::max(bCols, kCols) + 6};
    };

    using AES128 = RijndaelPolicy<4, 4>;
    using AES192 = RijndaelPolicy<4, 6>;
    using AES256 = RijndaelPolicy<4, 8>;

    using InvalidRij = RijndaelPolicy<3, 2>;
}
