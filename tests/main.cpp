#include "test_framework.hpp"
#include "test_utils.hpp"

#include <hashes/sha256.hpp>
#include <prfs/hmac.hpp>
#include <kdfs/pbkdf2.hpp>
#include <utils/bytes.hpp>

void testSHA256(const bytes::ByteVec& input, const bytes::ByteArr<32>& expected) {
    if (hashes::SHA256{}.compute(input) != expected)
        throw std::runtime_error("SHA256 output does not match expected output.");
}

void testHMAC(const bytes::ByteVec& key, const bytes::ByteVec& data, const bytes::ByteArr<32>& expected) {
    if (prfs::HMAC{hashes::SHA256{}}.compute(key, data) != expected)
        throw std::runtime_error("HMAC output does not match expected output.");
}

template <std::size_t DKeyLen>
void testPBKDF2(const bytes::ByteVec& key, const bytes::ByteVec& salt, std::size_t iters, const bytes::ByteArr<DKeyLen>& expected) {
    if (kdfs::PBKDF2<32, DKeyLen>(prfs::HMAC{hashes::SHA256{}}, iters).compute(key, salt) != expected)
        throw std::runtime_error("PBKDF2 output does not match expected output.");
}

int main() {
    test::TestFramework tests{};

    test::TestSuite& sha256Tests{tests.createSuite("SHA256")};

    sha256Tests.addCase("Empty string", [] {
        testSHA256(
            {},
            parseHexStr<32>("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")
        );
    });

    sha256Tests.addCase("ABC", [] {
        testSHA256(
            toBytes("abc"),
            parseHexStr<32>("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")
        );
    });

    sha256Tests.addCase("Repeated single character", [] {
        testSHA256(
            toBytes(std::string(1000000, 'a')),
            parseHexStr<32>("cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0")
        );
    });

    sha256Tests.addCase("Long random string", [] {
        testSHA256(
            toBytes("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
            parseHexStr<32>("248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1")
        );
    });

    sha256Tests.addCase("Longer random string", [] {
        testSHA256(
            toBytes("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"),
            parseHexStr<32>("cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1")
        );
    });

    // sha256Tests.addTestCase("Extremely long string", [] {
    //     testSHA256(
    //         toBytes("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno", 16777216),
    //         parseHexStr<32>("50e72a0e26442fe2552dc3938ac58658228c0cbfb1d2ca872ae435266fcd055e")
    //     );
    // });

    test::TestSuite& hmacTests{tests.createSuite("HMAC")};

    hmacTests.addCase("First test", [] {
        testHMAC(
            bytes::ByteVec(20, 0x0b),
            toBytes("Hi There"),
            parseHexStr<32>("b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7")
        );
    });

    hmacTests.addCase("Key shorter than output size", [] {
        testHMAC(
            toBytes("Jefe"),
            toBytes("what do ya want for nothing?"),
            parseHexStr<32>("5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843")
        );
    });

    hmacTests.addCase("Combined key and data larger than 64 bytes", [] {
        testHMAC(
            bytes::ByteVec(20, 0xaa),
            bytes::ByteVec(50, 0xdd),
            parseHexStr<32>("773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe")
        );
    });

    hmacTests.addCase("Combined key and data larger than 64 bytes again", [] {
        testHMAC(
            parseHexStr("0102030405060708090a0b0c0d0e0f10111213141516171819"),
            bytes::ByteVec(50, 0xcd),
            parseHexStr<32>("82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b")
        );
    });

    hmacTests.addCase("Key larger than block size", [] {
        testHMAC(
            bytes::ByteVec(131, 0xaa),
            toBytes("Test Using Larger Than Block-Size Key - Hash Key First"),
            parseHexStr<32>("60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54")
        );
    });

    hmacTests.addCase("Key and data larger than block size", [] {
        testHMAC(
            bytes::ByteVec(131, 0xaa),
            toBytes("This is a test using a larger than block-size key and a larger than block-size data. The key needs to be hashed before being used by the HMAC algorithm."),
            parseHexStr<32>("9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2")
        );
    });

    test::TestSuite& pbkdf2Tests{tests.createSuite("PBKDF2")};

    pbkdf2Tests.addCase("One iteration", [] {
        testPBKDF2<20>(
            toBytes("password"),
            toBytes("salt"),
            1,
            parseHexStr<20>("120fb6cffcf8b32c43e7225256c4f837a86548c9")
        );
    });

    pbkdf2Tests.addCase("Two iterations", [] {
        testPBKDF2<20>(
            toBytes("password"),
            toBytes("salt"),
            2,
            parseHexStr<20>("ae4d0c95af6b46d32d0adff928f06dd02a303f8e")
        );
    });

    pbkdf2Tests.addCase("Many iterations", [] {
        testPBKDF2<20>(
            toBytes("password"),
            toBytes("salt"),
            4096,
            parseHexStr<20>("c5e478d59288c841aa530db6845c4c8d962893a0")
        );
    });

    // pbkdf2Tests.addTestCase("Huge number of iterations", [] {
    //     testPBKDF2<20>(
    //         toBytes("password"),
    //         toBytes("salt"),
    //         16777216,
    //         parseHexStr<20>("cf81c66fe8cfc04d1f31ecb65dab4089f7f179e8")
    //     );
    // });

    pbkdf2Tests.addCase("Longer derived key", [] {
        testPBKDF2<25>(
            toBytes("passwordPASSWORDpassword"),
            toBytes("saltSALTsaltSALTsaltSALTsaltSALTsalt"),
            4096,
            parseHexStr<25>("348c89dbcbd32b2f32d814b8116e84cf2b17347ebc1800181c")
        );
    });

    pbkdf2Tests.addCase("Shorter derived key", [] {
        testPBKDF2<16>(
            toBytes(std::string("pass\0word", 9)),
            toBytes(std::string("sa\0lt", 5)),
            4096,
            parseHexStr<16>("89b69d0516f829893c696226650a8687")
        );
    });

    pbkdf2Tests.addCase("Very long derived key", [] {
        testPBKDF2<128>(
            toBytes("passwd"),
            toBytes("salt"),
            1,
            parseHexStr<128>("55ac046e56e3089fec1691c22544b605f94185216dde0465e68b9d57c20dacbc49ca9cccf179b645991664b39d77ef317c71b845b1e30bd509112041d3a19783c294e850150390e1160c34d62e9665d659ae49d314510fc98274cc79681968104b8f89237e69b2d549111868658be62f59bd715cac44a1147ed5317c9bae6b2a")
        );
    });

    pbkdf2Tests.addCase("Very long derived key with many iterations", [] {
        testPBKDF2<128>(
            toBytes("Password"),
            toBytes("NaCl"),
            80000,
            parseHexStr<128>("4ddcd8f60b98be21830cee5ef22701f9641a4418d04c0414aeff08876b34ab56a1d425a1225833549adb841b51c9b3176a272bdebba1d078478f62b397f33c8d62aae85a11cdde829d89cb6ffd1ab0e63a981f8747d2f2f9fe5874165c83c168d2eed1d2d5ca4052dec2be5715623da019b8c0ec87dc36aa751c38f9893d15c3")
        );
    });

    pbkdf2Tests.addCase("Extremely long derived key", [] {
        testPBKDF2<256>(
            toBytes("Password"),
            toBytes(std::string("sa\0lt", 5)),
            4096,
            parseHexStr<256>("436c82c6af9010bb0fdb274791934ac7dee21745dd11fb57bb90112ab187c495ad82df776ad7cefb606f34fedca59baa5922a57f3e91bc0e11960da7ec87ed0471b456a0808b60dff757b7d313d4068bf8d337a99caede24f3248f87d1bf16892b70b076a07dd163a8a09db788ae34300ff2f2d0a92c9e678186183622a636f4cbce15680dfea46f6d224e51c299d4946aa2471133a649288eef3e4227b609cf203dba65e9fa69e63d35b6ff435ff51664cbd6773d72ebc341d239f0084b004388d6afa504eee6719a7ae1bb9daf6b7628d851fab335f1d13948e8ee6f7ab033a32df447f8d0950809a70066605d6960847ed436fa52cdfbcf261b44d2a87061")
        );
    });

    if (tests.run()) return 0;
    else return 1;
}
