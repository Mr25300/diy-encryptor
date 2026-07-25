#include <hashes/sha256.hpp>
#include <prfs/hmac.hpp>
#include <kdfs/pbkdf2.hpp>
#include <utils/bytes.hpp>

#include <string>
#include <string_view>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <iomanip>

void runTest(std::string_view suite, std::string_view name)

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

template <typename C>
void printBytes(const C& bytes, std::ostream& output) {
    output << std::hex << std::setw(2) << std::setfill('0');

    for (std::uint8_t b : bytes) {
        output << static_cast<int>(b);
    }

    output << std::dec;
}

void testSHA256(const bytes::ByteVec& input, const bytes::ByteVec& expected) {
    static std::size_t testNum{1};

    hashes::SHA256 sha256{};
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

    prfs::HMAC<64, 32> hmac{hashes::SHA256{}};
    bytes::ByteArr<32> output{hmac.compute(key, data)};

    if (areEqual(output, expected)) {
        std::cout << "HMAC passed test " << testNum << '\n';
    } else {
        std::cerr << "HMAC failed test " << testNum << '\n';
    }

    ++testNum;
}

template <std::size_t DKeySize>
void testPBKDF2(const bytes::ByteVec& key, const bytes::ByteVec& salt, std::size_t iterations, const bytes::ByteVec& expected) {
    static std::size_t testNum{1};

    kdfs::PBKDF2<32, DKeySize> pbkdf2{prfs::HMAC<64, 32>{hashes::SHA256{}}, iterations};
    bytes::ByteArr<DKeySize> output{pbkdf2.compute(key, salt)};

    if (areEqual(output, expected)) {
        std::cout << "PBKDF2 passed test " << testNum << '\n';
    } else {
        std::cerr << "PBKDF2 failed test " << testNum << '\n';
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

    // testSHA256(
    //     toBytes("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno", 16777216),
    //     parseHexString("50e72a0e26442fe2552dc3938ac58658228c0cbfb1d2ca872ae435266fcd055e")
    // );

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

    testPBKDF2<20>(
        toBytes("password"),
        toBytes("salt"),
        1,
        parseHexString("120fb6cffcf8b32c43e7225256c4f837a86548c9")
    );

    testPBKDF2<20>(
        toBytes("password"),
        toBytes("salt"),
        2,
        parseHexString("ae4d0c95af6b46d32d0adff928f06dd02a303f8e")
    );

    testPBKDF2<20>(
        toBytes("password"),
        toBytes("salt"),
        4096,
        parseHexString("c5e478d59288c841aa530db6845c4c8d962893a0")
    );

    // testPBKDF2<20>(
    //     toBytes("password"),
    //     toBytes("salt"),
    //     16777216,
    //     parseHexString("cf81c66fe8cfc04d1f31ecb65dab4089f7f179e8")
    // );

    testPBKDF2<25>(
        toBytes("passwordPASSWORDpassword"),
        toBytes("saltSALTsaltSALTsaltSALTsaltSALTsalt"),
        4096,
        parseHexString("348c89dbcbd32b2f32d814b8116e84cf2b17347ebc1800181c")
    );

    testPBKDF2<16>(
        toBytes(std::string("pass\0word", 9)),
        toBytes(std::string("sa\0lt", 5)),
        4096,
        parseHexString("89b69d0516f829893c696226650a8687")
    );

    testPBKDF2<128>(
        toBytes("passwd"),
        toBytes("salt"),
        1,
        parseHexString("55ac046e56e3089fec1691c22544b605f94185216dde0465e68b9d57c20dacbc49ca9cccf179b645991664b39d77ef317c71b845b1e30bd509112041d3a19783c294e850150390e1160c34d62e9665d659ae49d314510fc98274cc79681968104b8f89237e69b2d549111868658be62f59bd715cac44a1147ed5317c9bae6b2a")
    );

    testPBKDF2<128>(
        toBytes("Password"),
        toBytes("NaCl"),
        80000,
        parseHexString("4ddcd8f60b98be21830cee5ef22701f9641a4418d04c0414aeff08876b34ab56a1d425a1225833549adb841b51c9b3176a272bdebba1d078478f62b397f33c8d62aae85a11cdde829d89cb6ffd1ab0e63a981f8747d2f2f9fe5874165c83c168d2eed1d2d5ca4052dec2be5715623da019b8c0ec87dc36aa751c38f9893d15c3")
    );

    testPBKDF2<256>(
        toBytes("Password"),
        toBytes(std::string("sa\0lt", 5)),
        4096,
        parseHexString("436c82c6af9010bb0fdb274791934ac7dee21745dd11fb57bb90112ab187c495ad82df776ad7cefb606f34fedca59baa5922a57f3e91bc0e11960da7ec87ed0471b456a0808b60dff757b7d313d4068bf8d337a99caede24f3248f87d1bf16892b70b076a07dd163a8a09db788ae34300ff2f2d0a92c9e678186183622a636f4cbce15680dfea46f6d224e51c299d4946aa2471133a649288eef3e4227b609cf203dba65e9fa69e63d35b6ff435ff51664cbd6773d72ebc341d239f0084b004388d6afa504eee6719a7ae1bb9daf6b7628d851fab335f1d13948e8ee6f7ab033a32df447f8d0950809a70066605d6960847ed436fa52cdfbcf261b44d2a87061")
    );

    return 0;
}
