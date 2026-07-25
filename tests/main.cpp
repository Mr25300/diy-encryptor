#include <hashes/sha256.hpp>
#include <prfs/hmac.hpp>
#include <utils/bytes.hpp>

#include <string>
#include <cassert>
#include <stdexcept>
#include <iostream>

template <typename C1, typename C2>
bool areEqual(C1 bytes1, C2 bytes2) {
    if (bytes1.size() != bytes2.size()) return false;

    for (std::size_t i{}; i < bytes1.size(); ++i) {
        if (bytes1[i] != bytes2[i]) return false;
    }

    return true;
}

bytes::ByteVec toBytes(std::string_view str, std::size_t repeatCount = 1) {
    bytes::ByteVec bytes;
    bytes.reserve(str.size() * repeatCount);

    for (std::size_t i{}; i < repeatCount; ++i) {
        bytes.insert(bytes.end(), str.begin(), str.end());
    }

    return bytes;
}

std::uint8_t parseHexChar(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    else if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    else if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    else throw std::invalid_argument("Invalid hex character.");
}

bytes::ByteVec parseHexString(std::string_view str) {
    if (str.size() % 2 != 0) throw std::invalid_argument("Invalid hex string.");

    bytes::ByteVec bytes;
    bytes.reserve(str.size() / 2);

    for (std::size_t i{}; i < str.size(); i += 2) {
        bytes.push_back(parseHexChar(str[i]) << 4 | parseHexChar(str[i + 1]));
    }

    return bytes;
}

// template <typename C>
// void printBytes(const C& bytes, std::ostream& output) {
//     output << std::hex << std::setw(2) << std::setfill('0');
//
//     for (std::uint8_t b : bytes) {
//         output << static_cast<int>(b);
//     }
//
//     output << std::dec;
// }

void testSHA256(const bytes::ByteVec& input, const bytes::ByteVec& expected) {
    static std::size_t testNum{1};
    static hashes::SHA256 sha256{};
    bytes::ByteArr<32> output{sha256.compute(input)};

    if (areEqual(output, expected)) {
        std::cout << "SHA256 passed test " << testNum << '\n';
    } else {
        std::cerr << "SHA256 failed test " << testNum << '\n';
    }

    ++testNum;
}

void testHMAC(const bytes::ByteVec& key, const bytes::ByteVec& data, const bytes::ByteVec& expected) {
    static std::size_t testNum{1};
    static hashes::SHA256 sha256{};
    static prfs::HMAC<64, 32> hmac{sha256};
    bytes::ByteArr<32> output{hmac.compute(key, data)};

    if (areEqual(output, expected)) {
        std::cout << "HMAC passed test " << testNum << '\n';
    } else {
        std::cerr << "HMAC failed test " << testNum << '\n';
    }

    ++testNum;
}

int main() {
    testSHA256(
        {},
        parseHexString("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")
    );

    testSHA256(
        toBytes("abc"),
        parseHexString("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")
    );

    testSHA256(
        toBytes(std::string(1000000, 'a')),
        parseHexString("cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0")
    );

    testSHA256(
        toBytes("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
        parseHexString("248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1")
    );

    testSHA256(
        toBytes("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"),
        parseHexString("cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1")
    );

    testSHA256(
        toBytes("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno", 16777216),
        parseHexString("50e72a0e26442fe2552dc3938ac58658228c0cbfb1d2ca872ae435266fcd055e")
    );

    testHMAC(
        bytes::ByteVec(20, 0x0b),
        toBytes("Hi There"),
        parseHexString("b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7")
    );

    testHMAC(
        toBytes("Jefe"),
        toBytes("what do ya want for nothing?"),
        parseHexString("5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843")
    );

    testHMAC(
        bytes::ByteVec(20, 0xaa),
        bytes::ByteVec(50, 0xdd),
        parseHexString("773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe")
    );

    testHMAC(
        parseHexString("0102030405060708090a0b0c0d0e0f10111213141516171819"),
        bytes::ByteVec(50, 0xcd),
        parseHexString("82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b")
    );

    testHMAC(
        bytes::ByteVec(131, 0xaa),
        toBytes("Test Using Larger Than Block-Size Key - Hash Key First"),
        parseHexString("60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54")
    );

    testHMAC(
        bytes::ByteVec(131, 0xaa),
        toBytes("This is a test using a larger than block-size key and a larger than block-size data. The key needs to be hashed before being used by the HMAC algorithm."),
        parseHexString("9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2")
    );

    return 0;
}
