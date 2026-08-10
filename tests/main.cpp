#include "test_framework.hpp"
#include "test_utils.hpp"

#include <utils/bytes.hpp>
#include <hashes/sha256.hpp>
#include <prfs/hmac.hpp>
#include <kdfs/pbkdf2.hpp>
#include <ciphers/rijndael/utils.hpp>
#include <ciphers/rijndael/policy.hpp>
#include <ciphers/rijndael/substitution_box.hpp>
#include <ciphers/rijndael/key_schedule.hpp>
#include <ciphers/rijndael/rijndael.hpp>
#include <ciphers/modes/cbc.hpp>
#include <padding/pkcs7.hpp>

#include <sstream>

using ciphers::rij::utils::getBlockView;
using ciphers::rij::RijPolConcept;
using ciphers::rij::RijndaelPolicy;
using ciphers::rij::AES128;
using ciphers::rij::AES192;
using ciphers::rij::AES256;
using ciphers::rij::Word;
using ciphers::rij::StateBlock;
using ciphers::rij::SubstitutionBox;
using ciphers::rij::KeySchedule;
using ciphers::rij::Rijndael;
using ciphers::modes::CBC;
using padding::PKCS7;

template <RijPolConcept Pol>
void testKeySchedule(const bytes::ByteArr<Pol::kSize>& key, const std::array<bytes::ByteArr<Pol::bSize>, Pol::rounds + 1>& expected) {
    KeySchedule<Pol> keySchedule{SubstitutionBox{}, key};

    for (std::size_t i{}; i < Pol::rounds + 1; ++i) {
        if (keySchedule[i] != getBlockView<Pol::bCols>(expected[i])) {
            std::ostringstream ss;
            ss << "Key schedule key at round index " << i << " does not match expected key.";

            throw std::runtime_error(ss.str());
        }
    }
}

template <RijPolConcept Pol>
void testRijndael(bytes::ByteArr<Pol::bSize> input, const bytes::ByteArr<Pol::kSize>& key, const bytes::ByteArr<Pol::bSize>& expected) {
    SubstitutionBox subBox{};
    KeySchedule<Pol> keySchedule{subBox, key};
    Rijndael<Pol> rijCipher{subBox, keySchedule};

    bytes::ByteArr<Pol::bSize> inputCopy{input};

    rijCipher.encrypt(input);

    if (input != expected)
        throw std::runtime_error("AES encryption output does not match expected output.");

    rijCipher.decrypt(input);

    if (input != inputCopy)
        throw std::runtime_error("AES decryption output does not match original input.");
}

template <RijPolConcept Pol>
void testCBC(
    bytes::ByteVec input, const bytes::ByteArr<Pol::kSize>& key,
    const bytes::ByteArr<Pol::bSize>& iv, const bytes::ByteVec& expected)
{
    SubstitutionBox subBox{};
    KeySchedule<Pol> keySchedule{subBox, key};
    Rijndael<Pol> aesCipher{subBox, keySchedule};
    CBC<Pol::bSize> cbcCipher{aesCipher, iv};

    bytes::ByteVec inputCopy{input};

    cbcCipher.encrypt(input);

    if (input != expected)
        throw std::runtime_error("CBC encryption output does not match expected output.");

    cbcCipher.decrypt(input);

    if (input != inputCopy)
        throw std::runtime_error("CBC decryption output does not match original input.");
}

template <std::size_t BlockSize>
void testPKCS7(bytes::ByteVec input, const bytes::ByteVec& expected) {
    PKCS7<BlockSize> padder{};

    bytes::ByteVec inputCopy{input};

    padder.pad(input);

    if (input != expected)
        throw std::runtime_error("PKCS7 pad output does not match expected output.");

    padder.unpad(input);

    if (input != inputCopy)
        throw std::runtime_error("PKCS7 unpad output does not match original input.");
}

template <RijPolConcept Pol>
void testFullEncryption(
    bytes::ByteVec input, const bytes::ByteArr<Pol::kSize>& key,
    const bytes::ByteArr<Pol::bSize>& iv, const bytes::ByteVec& expected)
{
    SubstitutionBox subBox{};
    KeySchedule<Pol> keySchedule{subBox, key};
    Rijndael<Pol> aesCipher{subBox, keySchedule};
    PKCS7<Pol::bSize> padder{};
    CBC<Pol::bSize> cbcCipher{aesCipher, iv};

    bytes::ByteVec inputCopy{input};

    padder.pad(input);
    cbcCipher.encrypt(input);

    if (input != expected)
        throw std::runtime_error("Full encryption output does not match expected output.");

    cbcCipher.decrypt(input);
    padder.unpad(input);

    if (input != inputCopy)
        throw std::runtime_error("Full decryption output does not match original input.");
}

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
    using namespace test;

    test::TestFramework tests{};

    test::TestSuite& keyScheduleTests{tests.createSuite("Rijndael Key Schedule")};

    keyScheduleTests.addCase("AES128", [] {
        testKeySchedule<AES128>(
            parseHexStr<16>("2b7e151628aed2a6abf7158809cf4f3c"),
            {
                parseHexStr<16>("2b7e151628aed2a6abf7158809cf4f3c"),
                parseHexStr<16>("a0fafe1788542cb123a339392a6c7605"),
                parseHexStr<16>("f2c295f27a96b9435935807a7359f67f"),
                parseHexStr<16>("3d80477d4716fe3e1e237e446d7a883b"),
                parseHexStr<16>("ef44a541a8525b7fb671253bdb0bad00"),
                parseHexStr<16>("d4d1c6f87c839d87caf2b8bc11f915bc"),
                parseHexStr<16>("6d88a37a110b3efddbf98641ca0093fd"),
                parseHexStr<16>("4e54f70e5f5fc9f384a64fb24ea6dc4f"),
                parseHexStr<16>("ead27321b58dbad2312bf5607f8d292f"),
                parseHexStr<16>("ac7766f319fadc2128d12941575c006e"),
                parseHexStr<16>("d014f9a8c9ee2589e13f0cc8b6630ca6")
            }
        );
    });

    keyScheduleTests.addCase("AES192", [] {
        testKeySchedule<AES192>(
            parseHexStr<24>("8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b"),
            {
                parseHexStr<16>("8e73b0f7da0e6452c810f32b809079e5"),
                parseHexStr<16>("62f8ead2522c6b7bfe0c91f72402f5a5"),
                parseHexStr<16>("ec12068e6c827f6b0e7a95b95c56fec2"),
                parseHexStr<16>("4db7b4bd69b5411885a74796e92538fd"),
                parseHexStr<16>("e75fad44bb095386485af05721efb14f"),
                parseHexStr<16>("a448f6d94d6dce24aa326360113b30e6"),
                parseHexStr<16>("a25e7ed583b1cf9a27f939436a94f767"),
                parseHexStr<16>("c0a69407d19da4e1ec1786eb6fa64971"),
                parseHexStr<16>("485f703222cb8755e26d135233f0b7b3"),
                parseHexStr<16>("40beeb282f18a2596747d26b458c553e"),
                parseHexStr<16>("a7e1466c9411f1df821f750aad07d753"),
                parseHexStr<16>("ca4005388fcc5006282d166abc3ce7b5"),
                parseHexStr<16>("e98ba06f448c773c8ecc720401002202")
            }
        );
    });

    keyScheduleTests.addCase("AES256", [] {
        testKeySchedule<AES256>(
            parseHexStr<32>("603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4"),
            {
                parseHexStr<16>("603deb1015ca71be2b73aef0857d7781"),
                parseHexStr<16>("1f352c073b6108d72d9810a30914dff4"),
                parseHexStr<16>("9ba354118e6925afa51a8b5f2067fcde"),
                parseHexStr<16>("a8b09c1a93d194cdbe49846eb75d5b9a"),
                parseHexStr<16>("d59aecb85bf3c917fee94248de8ebe96"),
                parseHexStr<16>("b5a9328a2678a647983122292f6c79b3"),
                parseHexStr<16>("812c81addadf48ba24360af2fab8b464"),
                parseHexStr<16>("98c5bfc9bebd198e268c3ba709e04214"),
                parseHexStr<16>("68007bacb2df331696e939e46c518d80"),
                parseHexStr<16>("c814e20476a9fb8a5025c02d59c58239"),
                parseHexStr<16>("de1369676ccc5a71fa2563959674ee15"),
                parseHexStr<16>("5886ca5d2e2f31d77e0af1fa27cf73c3"),
                parseHexStr<16>("749c47ab18501ddae2757e4f7401905a"),
                parseHexStr<16>("cafaaae3e4d59b349adf6acebd10190d"),
                parseHexStr<16>("fe4890d1e6188d0b046df344706c631e")
            }
        );
    });

    TestSuite& rijTests{tests.createSuite("Rijndael Cipher")};

    rijTests.addCase("AES128", [] {
        testRijndael<AES128>(
            parseHexStr<16>("3243f6a8885a308d313198a2e0370734"),
            parseHexStr<16>("2b7e151628aed2a6abf7158809cf4f3c"),
            parseHexStr<16>("3925841d02dc09fbdc118597196a0b32")
        );
    });

    rijTests.addCase("AES128 again", [] {
        testRijndael<AES128>(
            parseHexStr<16>("00112233445566778899aabbccddeeff"),
            parseHexStr<16>("000102030405060708090a0b0c0d0e0f"),
            parseHexStr<16>("69c4e0d86a7b0430d8cdb78070b4c55a")
        );
    });

    rijTests.addCase("AES192", [] {
        testRijndael<AES192>(
            parseHexStr<16>("00112233445566778899aabbccddeeff"),
            parseHexStr<24>("000102030405060708090a0b0c0d0e0f1011121314151617"),
            parseHexStr<16>("dda97ca4864cdfe06eaf70a0ec0d7191")
        );
    });

    rijTests.addCase("AES256", [] {
        testRijndael<AES256>(
            parseHexStr<16>("00112233445566778899aabbccddeeff"),
            parseHexStr<32>("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"),
            parseHexStr<16>("8ea2b7ca516745bfeafc49904b496089")
        );
    });

    rijTests.addCase("160 bit block, 128 bit key", [] {
        testRijndael<RijndaelPolicy<5, 4>>(
            parseHexStr<20>("9E38B8EB1D2025A1665AD4B1F5438BB5CAE1AC3F"),
            bytes::ByteArr<16>{},
            parseHexStr<20>("939C167E7F916D45670EE21BFC939E1055054A96")
        );
    });

    rijTests.addCase("192 bit block, 128 bit key", [] {
        testRijndael<RijndaelPolicy<6, 4>>(
            parseHexStr<24>("A92732EB488D8BB98ECD8D95DC9C02E052F250AD369B3849"),
            bytes::ByteArr<16>{},
            parseHexStr<24>("106F34179C3982DDC6750AA01936B7A180E6B0B9D8D690EC")
        );
    });

    rijTests.addCase("224 bit block, 128 bit key", [] {
        testRijndael<RijndaelPolicy<7, 4>>(
            parseHexStr<28>("0623522D88F7B9C63437537157F625DD5697AB628A3B9BE2549895C8"),
            bytes::ByteArr<16>{},
            parseHexStr<28>("93F93CBDABE23415620E6990B0443D621F6AFBD6EDEFD6990A1965A8")
        );
    });

    rijTests.addCase("256 bit block, 128 bit key", [] {
        testRijndael<RijndaelPolicy<8, 4>>(
            parseHexStr<32>("A693B288DF7DAE5B1757640276439230DB77C4CD7A871E24D6162E54AF434891"),
            bytes::ByteArr<16>{},
            parseHexStr<32>("5F05857C80B68EA42CCBC759D42C28D5CD490F1D180C7A9397EE585BEA770391")
        );
    });

    rijTests.addCase("128 bit block, 160 bit key", [] {
        testRijndael<RijndaelPolicy<4, 5>>(
            parseHexStr<16>("94B434F8F57B9780F0EFF1A9EC4C112C"),
            bytes::ByteArr<20>{},
            parseHexStr<16>("35A00EC955DF43417CEAC2AB2B3F3E76")
        );
    });

    rijTests.addCase("160 bit block, 160 bit key", [] {
        testRijndael<RijndaelPolicy<5, 5>>(
            parseHexStr<20>("33B12AB81DB7972E8FDC529DDA46FCB529B31826"),
            bytes::ByteArr<20>{},
            parseHexStr<20>("97F03EB018C0BB9195BF37C6A0AECE8E4CB8DE5F")
        );
    });

    rijTests.addCase("192 bit block, 160 bit key", [] {
        testRijndael<RijndaelPolicy<6, 5>>(
            parseHexStr<24>("528E2FFF6005427B67BB1ED31ECC09A69EF41531DF5BA5B2"),
            bytes::ByteArr<20>{},
            parseHexStr<24>("71C7687A4C93EBC35601E3662256E10115BEED56A410D7AC")
        );
    });

    rijTests.addCase("224 bit block, 160 bit key", [] {
        testRijndael<RijndaelPolicy<7, 5>>(
            parseHexStr<28>("58A0C53F3822A32464704D409C2FD0521F3A93E1F6FCFD4C87F1C551"),
            bytes::ByteArr<20>{},
            parseHexStr<28>("D8E93EF2EB49857049D6F6E0F40B67516D2696F94013C065283F7F01")
        );
    });

    rijTests.addCase("256 bit block, 160 bit key", [] {
        testRijndael<RijndaelPolicy<8, 5>>(
            parseHexStr<32>("938D36E0CB6B7937841DAB7F1668E47B485D3ACD6B3F6D598B0A9F923823331D"),
            bytes::ByteArr<20>{},
            parseHexStr<32>("7B44491D1B24A93B904D171F074AD69669C2B70B134A4D2D773250A4414D78BE")
        );
    });

    rijTests.addCase("160 bit block, 192 bit key", [] {
        testRijndael<RijndaelPolicy<5, 6>>(
            parseHexStr<20>("33060F9D4705DDD2C7675F0099140E5A98729257"),
            bytes::ByteArr<24>{},
            parseHexStr<20>("012CAB64982156A5710E790F85EC442CE13C520F")
        );
    });

    rijTests.addCase("192 bit block, 192 bit key", [] {
        testRijndael<RijndaelPolicy<6, 6>>(
            parseHexStr<24>("C6348BE20007BAC4A8BD62890C8147A2432E760E9A9F9AB8"),
            bytes::ByteArr<24>{},
            parseHexStr<24>("EB9DEF13C253F81C1FC2829426ED166A65A105C6A04CA33D")
        );
    });

    rijTests.addCase("224 bit block, 192 bit key", [] {
        testRijndael<RijndaelPolicy<7, 6>>(
            parseHexStr<28>("3856B17BEA77C4611E3397066828AADDA004706A2C8009DF40A811FE"),
            bytes::ByteArr<24>{},
            parseHexStr<28>("160AD76A97AE2C1E05942FDE3DA2962684A92CCC74B8DC23BDE4F469")
        );
    });

    rijTests.addCase("256 bit block, 192 bit key", [] {
        testRijndael<RijndaelPolicy<8, 6>>(
            parseHexStr<32>("F927363EF5B3B4984A9EB9109844152EC167F08102644E3F9028070433DF9F2A"),
            bytes::ByteArr<24>{},
            parseHexStr<32>("4E03389C68B2E3F623AD8F7F6BFC88613B86F334F4148029AE25F50DB144B80C")
        );
    });

    rijTests.addCase("128 bit block, 224 bit key", [] {
        testRijndael<RijndaelPolicy<4, 7>>(
            parseHexStr<16>("73F8DFF62A36F3EBF31D6F73A56FF279"),
            bytes::ByteArr<28>{},
            parseHexStr<16>("3A72F21E10B6473EA9FF14A232E675B4")
        );
    });

    rijTests.addCase("160 bit block, 224 bit key", [] {
        testRijndael<RijndaelPolicy<5, 7>>(
            parseHexStr<20>("E9F5EA0FA39BB6AD7339F28E58E2E7535F261827"),
            bytes::ByteArr<28>{},
            parseHexStr<20>("06EF9BC82905306D45810E12D0807796A3D338F9")
        );
    });

    rijTests.addCase("192 bit block, 224 bit key", [] {
        testRijndael<RijndaelPolicy<6, 7>>(
            parseHexStr<24>("ECBE9942CD6703E16D358A829D542456D71BD3408EB23C56"),
            bytes::ByteArr<28>{},
            parseHexStr<24>("FD10458ED034368A34047905165B78A6F0591FFEEBF47CC7")
        );
    });

    rijTests.addCase("224 bit block, 224 bit key", [] {
        testRijndael<RijndaelPolicy<7, 7>>(
            parseHexStr<28>("FE1CF0C8DDAD24E3D751933100E8E89B61CD5D31C96ABFF7209C495C"),
            bytes::ByteArr<28>{},
            parseHexStr<28>("515D8E2F2B9C5708F112C6DE31CACA47AFB86838B716975A24A09CD4")
        );
    });

    rijTests.addCase("256 bit block, 224 bit key", [] {
        testRijndael<RijndaelPolicy<8, 7>>(
            parseHexStr<32>("BC18BF6D369C955BBB271CBCDD66C368356DBA5B33C0005550D2320B1C617E21"),
            bytes::ByteArr<28>{},
            parseHexStr<32>("60ABA1D2BE45D8ABFDCF97BCB39F6C17DF29985CF321BAB75E26A26100AC00AF")
        );
    });

    rijTests.addCase("160 bit block, 256 bit key", [] {
        testRijndael<RijndaelPolicy<5, 8>>(
            parseHexStr<20>("30991844F72973B3B2161F1F11E7F8D9863C5118"),
            bytes::ByteArr<32>{},
            parseHexStr<20>("EEF8B7CC9DBE0F03A1FE9D82E9A759FD281C67E0")
        );
    });

    rijTests.addCase("192 bit block, 256 bit key", [] {
        testRijndael<RijndaelPolicy<6, 8>>(
            parseHexStr<24>("17004E806FAEF168FC9CD56F98F070982075C70C8132B945"),
            bytes::ByteArr<32>{},
            parseHexStr<24>("BED33B0AF364DBF15F9C2F3FB24FBDF1D36129C586EEA6B7")
        );
    });

    rijTests.addCase("224 bit block, 256 bit key", [] {
        testRijndael<RijndaelPolicy<7, 8>>(
            parseHexStr<28>("9BF26FAD5680D56B572067EC2FE162F449404C86303F8BE38FAB6E02"),
            bytes::ByteArr<32>{},
            parseHexStr<28>("658F144A34AF44AAE66CFDDAB955C483DFBCB4EE9A19A6701F158A66")
        );
    });

    rijTests.addCase("256 bit block, 256 bit key", [] {
        testRijndael<RijndaelPolicy<8, 8>>(
            parseHexStr<32>("C6227E7740B7E53B5CB77865278EAB0726F62366D9AABAD908936123A1FC8AF3"),
            bytes::ByteArr<32>{},
            parseHexStr<32>("9843E807319C32AD1EA3935EF56A2BA96E4BF19C30E47D88A2B97CBBF2E159E7")
        );
    });

    TestSuite& cbcTests{tests.createSuite("Cipher Block Chaining")};

    cbcTests.addCase("Encrypting 16 bytes with AES128", [] {
        testCBC<AES128>(
            toBytes("Single block msg"),
            parseHexStr<16>("06a9214036b8a15b512e03d534120006"),
            parseHexStr<16>("3dafba429d9eb430b422da802c9fac41"),
            parseHexStr("e353779c1079aeb82708942dbe77181a")
        );
    });

    cbcTests.addCase("Encrypting 32 bytes with AES128", [] {
        testCBC<AES128>(
            parseHexStr("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"),
            parseHexStr<16>("c286696d887c9aa0611bbb3e2025a45a"),
            parseHexStr<16>("562e17996d093d28ddb3ba695a2e6f58"),
            parseHexStr("d296cd94c2cccf8a3a863028b5e1dc0a7586602d253cfff91b8266bea6d61ab1")
        );
    });

    cbcTests.addCase("Encrypting 48 bytes with AES128", [] {
        testCBC<AES128>(
            toBytes("This is a 48-byte message (exactly 3 AES blocks)"),
            parseHexStr<16>("6c3ea0477630ce21a2ce334aa746c2cd"),
            parseHexStr<16>("c782dc4c098c66cbd9cd27d825682c81"),
            parseHexStr("d0a02b3836451753d493665d33f0e8862dea54cdb293abc7506939276772f8d5021c19216bad525c8579695d83ba2684")
        );
    });

    cbcTests.addCase("Encrypting 64 bytes with AES128", [] {
        testCBC<AES128>(
            parseHexStr("a0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedf"),
            parseHexStr<16>("56e47a38c5598974bc46903dba290349"),
            parseHexStr<16>("8ce82eefbea0da3c44699ed7db51b7d9"),
            parseHexStr("c30e32ffedc0774e6aff6af0869f71aa0f3af07a9a31a9c684db207eb0ef8e4e35907aa632c3ffdf868bb7b29d3d46ad83ce9f9a102ee99d49a53e87f4c3da55")
        );
    });

    TestSuite& pkcs7Tests{tests.createSuite("PKCS7 Padding")};

    pkcs7Tests.addCase("Small string and block size", [] {
        testPKCS7<8>(
            parseHexStr("58b3a9"),
            parseHexStr("58b3a90505050505")
        );
    });

    pkcs7Tests.addCase("Larger string and block size", [] {
        testPKCS7<16>(
            parseHexStr("c3074bb2b49f5ba9a60b61306d2c"),
            parseHexStr("c3074bb2b49f5ba9a60b61306d2c0202")
        );
    });

    pkcs7Tests.addCase("Edge case", [] {
        testPKCS7<8>(
            parseHexStr("58b3a932078cfd19"),
            parseHexStr("58b3a932078cfd190808080808080808")
        );
    });

    test::TestSuite& fullTests{tests.createSuite("Rijndael + Cipher Block Chaining + PKCS7")};

    fullTests.addCase("Using AES128", [] {
        testFullEncryption<AES128>(
            toBytes("Here is my plaintext.", 10),
            toBytes<16>("secretkeyyyyyyyy"),
            toBytes<16>("initvectorrrrrrr"),
            parseHexStr("BBFFD8670FAB11BA76BB69D9E276BAFAB2BD00EFADE2351D2A862077C2188868337274BB98B6199EF8C1DB30CB4C464C68961C50C70B87DC8F5CFBB8520942900EFCC33E1E68895650A10A52DAB13A0A7C7EEE90CC29C3F35580752D89CAD8F4AA8EE5EF263CB00E9B385B7B1874293AF6DC780F64209C3C2F47758DCBF7B7C728EDA9A3766203D9C349787B8E2D99FB50BA376D8281477AFA8F15C320ADA2A5D5C76892464EE38FBCCD59E1B986B69841E9725B0285FC0EF3609FD9A461A40C2F873341AFBFC113B464B432D9056A4A963F936B62018FD06899411AC24CBD6F")
        );
    });

    fullTests.addCase("Using 256 bit block and 256 bit key Rijndael", [] {
        testFullEncryption<RijndaelPolicy<8, 8>>(
            toBytes("Here is my plaintext.", 10),
            toBytes<32>("secretttttttttttkeyyyyyyyyyyyyyy"),
            toBytes<32>("initializationnnvectorrrrrrrrrrr"),
            parseHexStr("56B7228C9CA269C3B17CC39B198516EF5CA910B89910A0A97C1CC0C45EC69D8E8EADC53BAE8D34AE1F83BBDE557636CF6CBBEC2F906F214CCD6C8030D5C3C729D7FFAC65D7B6FD2E3AD88490B11ADE28ECC6EBF10821DD111FE1A53E72DA9C9CFFEF3EA95F8C3B6F5472F33966438EE89F6EB6DB33AC12FF430D9705B516A4171C2D828117A6C0DE084DE71F1D771B0A691922087EFDDB48A1CFE18AB1E6E19048516B0B41B29CB9290B15C26A33AA2BBF9A8BC0E0092853CBAB9AB46084DD46372F00BEC035AD88BD5F51EDCA850916DB626A4B232A621E00ED1187EBCD33F3")
        );
    });

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

    sha256Tests.addCase("Extremely long string", [] {
        testSHA256(
            toBytes("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno", 16777216),
            parseHexStr<32>("50e72a0e26442fe2552dc3938ac58658228c0cbfb1d2ca872ae435266fcd055e")
        );
    });

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

    pbkdf2Tests.addCase("Huge number of iterations", [] {
        testPBKDF2<20>(
            toBytes("password"),
            toBytes("salt"),
            16777216,
            parseHexStr<20>("cf81c66fe8cfc04d1f31ecb65dab4089f7f179e8")
        );
    });

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

    return !tests.run();
}
