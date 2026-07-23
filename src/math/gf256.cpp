#include "gf256.hpp"

#include <bitset>

namespace math {
    void GF256::print(std::ostream& stream, GFFormat format) const {
        switch(format) {
            case GFFormat::Hex: {
                stream << "0x" << hexDigits[value >> 4] << hexDigits[value & 0b1111];

                break;
            }
            case GFFormat::Binary: {
                stream << "0b" << std::bitset<8>(value).to_string();

                break;
            }
            case GFFormat::Char: {
                stream << static_cast<char>(value);

                break;
            }
            case GFFormat::Int: {
                stream << std::to_string(value);

                break;
            }
            case GFFormat::Poly: {
                bool firstPlaced = false;

                if (value == 0) {
                    stream << '0';

                    break;
                }

                for (int i = 7; i >= 0; i--) {
                    if (value & (1 << i)) {
                        if (firstPlaced) {
                            stream << " + ";
                        }

                        firstPlaced = true;

                        if (i == 0) {
                            stream << '1';
                        } else if (i == 1) {
                            stream << 'x';
                        } else {
                            stream << "x^" << std::to_string(i);
                        }
                    }
                }

                break;
            }
            default:
                throw std::invalid_argument("Unknown format enum value");
        }
    }

    std::ostream& operator<<(std::ostream& stream, GF256 number) {
        number.print(stream);

        return stream;
    }
}
