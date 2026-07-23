#include "types.hpp"

namespace ciphers::aes {
    template <std::size_t Cols>
    std::ostream& operator<<(std::ostream& stream, const Block<Cols>& block) {
        for (std::size_t i{}; i < constants::rows; ++i) {
            if (i > 0) stream << '\n';

            for (std::size_t j{}; j < Cols; ++j) {
                if (j == 0) stream << '|';

                stream << block[j][i] << '|';
            }
        }
    }
}
