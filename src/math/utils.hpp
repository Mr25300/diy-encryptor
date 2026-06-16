#pragma once

#include <cstddef>

namespace math::utils {
    constexpr double sqrt(double num) {
        double curr{num};
        double prev{0};

        for (std::size_t i{}; i < 15; ++i) {
            prev = curr;
            curr = 0.5 * (curr + num / curr);

            if (curr == prev) {
                break;
            }
        }

        return curr;
    }

    constexpr double cbrt(double num) {
        double absNum{num < 0.0 ? -num : num};
        double curr{num}; // TODO: Test with negative num, if it breaks replace num with absNum here
        double prev{0};

        for (std::size_t i{}; i < 20; ++i) {
            prev = curr;
            curr = (2.0 * curr + absNum / (curr * curr)) / 3.0;

            if (curr == prev) {
                break;
            }
        }

        return curr;
    }
}
