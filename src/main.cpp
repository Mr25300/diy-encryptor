#include <iostream>
#include <filesystem>
#include <string>

#include <hashes/sha256.hpp>
#include <prfs/hmac.hpp>
#include <kdfs/pbkdf2.hpp>

#include <ciphers/aes/substitution_box.hpp>
#include <ciphers/aes/key_schedule.hpp>
#include <ciphers/aes/aes.hpp>

#include <padding/pkcs7.hpp>
#include <ciphers/modes/cbc.hpp>

#include <utils/bytes.hpp>
#include <utils/io.hpp>

namespace aes = ciphers::aes;

using AESPol = ciphers::aes::AES128;

const std::size_t kdfIterations{600000}; // Should be 600000
const std::size_t kdfSaltSize{32}; // In bytes

const std::string encryptedExtension{".enc"};
const std::string decryptedExtension{".dec"};
const std::string metadataExtension{".enc.meta"};

enum EncryptionMode { ENCRYPT, DECRYPT, UNDEFINED };

int main(int argc, char* argv[]) {
    std::string prevArg;
    std::string filePathStr;
    std::string outputDirStr;
    EncryptionMode encryptionMode{UNDEFINED};
    bool deletePrev{false};

    for (int i{1}; i < argc; ++i) {
        char* arg{argv[i]};
        std::string argStr{std::string(arg)};

        if (prevArg == "--mode" || prevArg == "-m") {
            if (argStr == "encrypt") encryptionMode = ENCRYPT;
            else if (argStr == "decrypt") encryptionMode = DECRYPT;
            else {
                std::cerr << "Invalid encryption mode provided.";

                return 1;
            }
        }
        else if (prevArg == "--input" || prevArg == "-i") filePathStr = arg;
        else if (prevArg == "--output" || prevArg == "-o") outputDirStr = arg;
        else if (argStr == "--delete" || argStr == "-d") deletePrev = true;
        else if (
            argStr != "--mode" && argStr != "-m" &&
            argStr != "--input" && argStr != "-i" &&
            argStr != "--output" && argStr != "-o"
        ) {
            std::cerr << "Invalid argument provided.";

            return 1;
        }

        prevArg = argStr;
    }

    if (filePathStr.empty()) {
        std::cerr << "File path unspecified.";

        return 1;
    }

    if (encryptionMode == UNDEFINED) {
        std::cerr << "Encryption mode unspecified.";

        return 1;
    }

    std::filesystem::path inputPath(filePathStr); // TODO: Make sure this exists first before doing stuff below
    std::filesystem::path outputPath;
    std::filesystem::path metadataPath;

    bytes::ByteArr<aes::constants::blockSize> cbcIV;
    bytes::ByteVec kdfSalt; // TODO: Switch to ByteArr by using span for KDF.compute, PRF.compute and io functions
    kdfSalt.reserve(kdfSaltSize);

    if (outputDirStr.empty()) {
        outputPath = inputPath;
        metadataPath = inputPath;
    } else {
        outputPath = outputDirStr / inputPath.filename();
        metadataPath = outputDirStr / inputPath.filename();
    }

    if (encryptionMode == ENCRYPT) {
        if (inputPath.extension() == decryptedExtension) {
            inputPath.replace_extension("");
            metadataPath.replace_extension(metadataExtension);
        } else {
            outputPath += encryptedExtension;
            metadataPath += metadataExtension;
        }

        cbcIV = bytes::getRandBytes<aes::constants::blockSize>();
        kdfSalt = bytes::getRandBytes(kdfSaltSize);

    } else if (encryptionMode == DECRYPT) {
        if (inputPath.extension() == encryptedExtension) {
            outputPath.replace_extension("");
            metadataPath.replace_extension(metadataExtension);
        } else {
            outputPath += decryptedExtension;
            metadataPath += metadataExtension;
        }

        bytes::ByteVec metaData{io::readFile(metadataPath)};

        std::copy(metaData.begin(), metaData.begin() + cbcIV.size(), cbcIV.begin());
        kdfSalt.insert(kdfSalt.end(), metaData.end() - kdfSaltSize, metaData.end());
        // TODO: Add error handling for corruption here too
    }

    std::vector<std::uint8_t> data{io::readFile(inputPath)};

    std::cout << (encryptionMode == ENCRYPT ? "Encrypting" : "Decrypting")
        << " file, input password key: ";

    bytes::ByteVec password{io::getInput()};

    hashes::SHA256 sha256{};
    prfs::HMAC<64, 32> hmac{sha256};
    kdfs::PBKDF2<32, AESPol::keySize> pbkdf2{hmac, kdfIterations};

    bytes::ByteArr<AESPol::keySize> key{pbkdf2.compute(password, kdfSalt)};

    aes::SubstitutionBox subBox{};
    aes::KeySchedule<AESPol> keySchedule{subBox, key};
    aes::AES<AESPol> aesCipher{subBox, keySchedule};

    padding::PKCS7<aes::constants::blockSize> padder{};
    ciphers::modes::CBC<aes::constants::blockSize> cbcCipher{aesCipher, cbcIV};

    if (encryptionMode == ENCRYPT) {
        padder.pad(data);
        cbcCipher.encrypt(data);

    } else {
        if (!cbcCipher.decrypt(data) || !padder.unpad(data)) {
            std::cerr << "Failed to decrypt file due to corruption.\n";

            return 1;
        }
    }

    io::writeToFile(outputPath, data);

    if (encryptionMode == ENCRYPT) io::writeToFile(metadataPath, bytes::getAppendBytes(cbcIV, kdfSalt));
    else if (encryptionMode == DECRYPT) io::deleteFile(metadataPath);

    if (deletePrev) io::deleteFile(inputPath);

    return 0;
}
