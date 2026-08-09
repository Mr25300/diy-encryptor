#include "gf256.hpp"

#include <iomanip>
#include <bitset>

enum class GFFormat {
    Hex,
    Binary,
    Char,
    Int,
    Polynomial
};

static int getGFFormatIndex() {
    static int index{std::ios_base::xalloc()};

    return index;
}

static std::ostream& setIWord(std::ostream& os, GFFormat type) {
    os.iword(getGFFormatIndex()) = static_cast<int>(type);

    return os;
}

static GFFormat getIWord(std::ostream& os) {
    return static_cast<GFFormat>(os.iword(getGFFormatIndex()));
}

namespace math {
    std::ostream& gfHex(std::ostream& os) { return setIWord(os, GFFormat::Hex); }
    std::ostream& gfBin(std::ostream& os) { return setIWord(os, GFFormat::Binary); }
    std::ostream& gfChr(std::ostream& os) { return setIWord(os, GFFormat::Char); }
    std::ostream& gfInt(std::ostream& os) { return setIWord(os, GFFormat::Int); }
    std::ostream& gfPoly(std::ostream& os) { return setIWord(os, GFFormat::Polynomial); }

    std::ostream& operator<<(std::ostream& os, GF256 num) {
        switch(getIWord(os)) {
            case GFFormat::Hex: {
                os << "0x" << std::hex << std::setw(2) << std::setfill('0')
                    << static_cast<int>(num.value) << std::dec;

                break;
            }
            case GFFormat::Binary: {
                os << "0b" << std::bitset<8>(num.value).to_string();

                break;
            }
            case GFFormat::Char: {
                os << static_cast<char>(num.value);

                break;
            }
            case GFFormat::Int: {
                os << static_cast<int>(num.value);

                break;
            }
            case GFFormat::Polynomial: {
                bool firstPlaced{false};

                if (num.value == 0) {
                    os << '0';

                    break;
                }

                std::uint8_t bitPos{0x80};

                for (int i{7}; i >= 0; --i) {
                    if (num.value & bitPos) {
                        if (firstPlaced) os << " + ";

                        firstPlaced = true;

                        if (i == 0) os << '1';
                        else if (i == 1) os << 'x';
                        else os << "x^" << static_cast<int>(i);
                    }

                    bitPos >>= 1;
                }

                break;
            }
        }

        return os;
    }
}
