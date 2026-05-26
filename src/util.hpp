#pragma once

namespace Util {
    constexpr int properMod(int num, int base) {
        return (num % base + base) % base;
    }

    constexpr double constexprSqrt(double num) {
        double curr{num};
        double prev{0};

        for (int i{}; i < 15; ++i) {
            prev = curr;
            curr = 0.5 * (curr + num / curr);

            if (curr == prev) {
                break;
            }
        }

        return curr;
    }

    constexpr double constexprCbrt(double num) {
        double absNum{num < 0.0 ? -num : num};
        double curr{num};
        double prev{0};

        for (int i{}; i < 20; ++i) {
            prev = curr;
            curr = (2.0 * curr + absNum / (curr * curr)) / 3.0;

            if (curr == prev) {
                break;
            }
        }

        return curr;
    }
}
