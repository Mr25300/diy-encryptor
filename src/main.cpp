#include <hashes/sha256.hpp>
#include <prfs/hmac.hpp>
#include <kdfs/pbkdf2.hpp>

#include <ciphers/rijndael/substitution_box.hpp>
#include <ciphers/rijndael/key_schedule.hpp>
#include <ciphers/rijndael/rijndael.hpp>

#include <padding/pkcs7.hpp>
#include <ciphers/modes/cbc.hpp>

#include <utils/bytes.hpp>
#include <utils/io.hpp>

#include <iostream>
#include <filesystem>
#include <string>

using AESPol = ciphers::rij::AES128;
using ciphers::rij::SubstitutionBox;
using ciphers::rij::KeySchedule;
using ciphers::rij::Rijndael;
using ciphers::modes::CBC;

const std::size_t kdfIterations{600000};
const std::size_t kdfSaltSize{32}; // In bytes

const std::size_t headerSize{AESPol::bSize + kdfSaltSize};

const std::string encryptedExtension{".enc"};
const std::string decryptedExtension{".dec"};

enum EncryptionMode { ENCRYPT, DECRYPT, UNDEFINED };

int main(int argc, char* argv[]) {
    EncryptionMode encryptionMode{UNDEFINED};
    std::string filePathStr;
    std::string outputDirStr;
    bool deletePrev{false};

    for (int i{1}; i < argc; ++i) {
        std::string arg{argv[i]};

        if (arg == "--output" || arg == "-o") {
            if (i >= argc - 1) {
                std::cerr << "--output flag value not specified.\n";
                return 1;
            }

            outputDirStr = std::string{argv[i + 1]};
            ++i;

            continue;
        }

        if (arg == "--delete" || arg == "-d") {
            deletePrev = true;

            continue;
        }

        if (encryptionMode == UNDEFINED) {
            if (arg == "encrypt") encryptionMode = ENCRYPT;
            else if (arg == "decrypt") encryptionMode = DECRYPT;
            else {
                std::cerr << "Invalid encryption mode provided.\n";
                return 1;
            }

        } else if (filePathStr.empty()) {
            filePathStr = arg;

        } else {
            std::cerr << "Invalid argument provided.\n";
            return 1;
        }
    }

    if (encryptionMode == UNDEFINED) {
        std::cerr << "Encryption mode unspecified.\n";
        return 1;
    }

    if (filePathStr.empty()) {
        std::cerr << "Input file path unspecified.\n";
        return 1;
    }

    std::filesystem::path inputPath{filePathStr};

    if (!std::filesystem::exists(inputPath)) {
        std::cerr << "File " << inputPath << " does not exist.\n";
        return 1;
    }

    bytes::ByteVec data{io::readFile(inputPath)};
    bytes::ByteArr<AESPol::bSize> cbcIV;
    bytes::ByteArr<kdfSaltSize> kdfSalt;

    std::filesystem::path outputPath;

    if (outputDirStr.empty()) outputPath = inputPath;
    else outputPath = std::filesystem::path{outputDirStr} / inputPath.filename();

    if (encryptionMode == ENCRYPT) {
        if (inputPath.extension() == decryptedExtension) {
            outputPath.replace_extension("");
        } else {
            outputPath += encryptedExtension;
        }

        cbcIV = bytes::getRandBytes<AESPol::bSize>();
        kdfSalt = bytes::getRandBytes<kdfSaltSize>();

    } else if (encryptionMode == DECRYPT) {
        if (inputPath.extension() == encryptedExtension) {
            outputPath.replace_extension("");
        } else {
            outputPath += decryptedExtension;
        }

        if (data.size() < AESPol::bSize + kdfSaltSize) {
            std::cerr << "Initialization vector and/or KDF salt corrupted.\n";
            return 1;
        }

        std::copy(data.begin(), data.begin() + AESPol::bSize, cbcIV.begin());
        std::copy(data.begin() + AESPol::bSize, data.begin() + headerSize, kdfSalt.begin());

        // TODO: Try and get rid of this inefficiency by using a span
        data.erase(data.begin(), data.begin() + headerSize);
    }

    std::cout << (encryptionMode == ENCRYPT ? "Encrypting" : "Decrypting")
        << " file, input password key: ";

    bytes::ByteVec password{io::getInput()};

    hashes::SHA256 sha256{};
    prfs::HMAC<64, 32> hmac{sha256};
    kdfs::PBKDF2<32, AESPol::kSize> pbkdf2{hmac, kdfIterations};

    bytes::ByteArr<AESPol::kSize> key{pbkdf2.compute(password, kdfSalt)};

    SubstitutionBox subBox{};
    KeySchedule<AESPol> keySchedule{subBox, key};
    Rijndael<AESPol> aesCipher{subBox, keySchedule};

    padding::PKCS7<AESPol::bSize> padder{};
    CBC<AESPol::bSize> cbcCipher{aesCipher, cbcIV};

    if (encryptionMode == ENCRYPT) {
        padder.pad(data);
        cbcCipher.encrypt(data);

    } else {
        if (!cbcCipher.decrypt(data) || !padder.unpad(data)) {
            std::cerr << "Failed to decrypt file due to corruption.\n";
            return 1;
        }
    }

    if (encryptionMode == ENCRYPT) {
        io::writeToFile(outputPath, {cbcIV, kdfSalt, data});
    } else {
        io::writeToFile(outputPath, data);
    }

    if (deletePrev) io::deleteFile(inputPath);

    return 0;
}
