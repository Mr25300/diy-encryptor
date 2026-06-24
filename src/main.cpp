#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <ostream>
#include <random>

#include <hashes/sha256.hpp>
#include <prfs/hmac.hpp>
#include <kdfs/pbkdf2.hpp>

#include <ciphers/aes/substitution_box.hpp>
#include <ciphers/aes/key_schedule.hpp>
#include <ciphers/aes/aes.hpp>

#include <padding/pkcs7.hpp>
#include <ciphers/modes/cbc.hpp>

constexpr int kdfIterations{6000}; // Should be 600000

constexpr hashes::SHA256 sha256{};
constexpr prfs::HMAC hmac{sha256};
constexpr kdfs::PBKDF2 pbkdf2{hmac, kdfIterations};

constexpr ciphers::aes::SubstitutionBox subBox;
constexpr ciphers::aes::KeySchedule keySchedule{subBox};
constexpr ciphers::aes::AES aesCipher{subBox, keySchedule};

constexpr padding::PKCS7<ciphers::aes::constants::blockSize> padder{};
constexpr ciphers::modes::CBC cbcCipher{padder, aesCipher};

const std::string encryptedExtension = ".enc";
const std::string decryptedExtension = ".dec";
const std::string metadataExtension = ".enc.meta"; // Change to ".enc.meta" for storing salt from PBKDF2

std::vector<std::uint8_t> generateIV(std::size_t length) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(0, 255);

    std::vector<std::uint8_t> iv(length, '\0');

    for (std::size_t i{}; i < length; ++i) {
        iv[i] = dist(gen);
    }

    return iv;
}

std::vector<std::uint8_t> readFile(const std::filesystem::path& filePath) {
    std::ifstream inFile(filePath, std::ios::binary | std::ios::ate);

    if (!inFile) {
        throw std::ios_base::failure("Failed to read from file: " + filePath.string());
    }

    std::streamsize fileSize{inFile.tellg()};
    inFile.seekg(0);

    std::vector<std::uint8_t> fileData(fileSize);
    inFile.read(reinterpret_cast<char*>(fileData.data()), fileSize);
    inFile.close();

    return fileData;
}

void writeToFile(const std::filesystem::path& filePath, const std::vector<std::uint8_t>& data) {
    std::ofstream outFile(filePath, std::ios::binary);

    if (!outFile) {
        throw std::ios_base::failure("Failed to write to file: " + filePath.string());
    }

    outFile.seekp(0);
    outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
    outFile.close();
}

void deleteFile(const std::filesystem::path& filePath) {
    if (!std::filesystem::remove(filePath)) {
        std::cerr << "Failed to delete file: " + filePath.string();
    }
}

enum EncryptionMode { ENCRYPT, DECRYPT, UNDEFINED };

int main(int argc, char *argv[]) {
    std::string prevArg;
    std::string filePathStr;
    std::string outputDirStr;
    EncryptionMode encryptionMode = UNDEFINED;
    bool deletePrev = false;

    for (std::size_t i{1}; i < argc; ++i) {
        char *arg = argv[i];
        std::string argStr = std::string(arg);

        if (argStr == "encrypt") encryptionMode = ENCRYPT;
        else if (argStr == "decrypt") encryptionMode = DECRYPT;
        else if (argStr == "--delete" || argStr == "-d") deletePrev = true;
        else {
            if (prevArg == "--output" || prevArg == "-o") outputDirStr = arg;
            else if (filePathStr.empty()) filePathStr = arg;
        }

        prevArg = argStr;
    }

    if (encryptionMode == UNDEFINED) {
        std::cerr << "Encryption mode unspecified.";

        return 1;
    }

    std::filesystem::path inputPath(filePathStr); // Make sure this exists first before doing stuff below
    std::filesystem::path outputPath;
    std::filesystem::path metadataPath;
    std::vector<std::uint8_t> iv;

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

        iv = generateIV();

    } else if (encryptionMode == DECRYPT) {
        if (inputPath.extension() == encryptedExtension) {
            outputPath.replace_extension("");
            metadataPath.replace_extension(metadataExtension);
        } else {
            outputPath += decryptedExtension;
            metadataPath += metadataExtension;
        }

        iv = readFile(metadataPath);
    }

    std::vector<std::uint8_t> data{readFile(inputPath)};

    std::string passwordStr;
    std::cout << (encryptionMode == ENCRYPT ? "Encrypting" : "Decrypting")
        << " file, input password key: ";
    std::cin >> passwordStr;

    std::vector<std::uint8_t> password(passwordStr.begin(), passwordStr.end());

    keySchedule.generate(password);

    std::vector<std::uint8_t> keyBytes{pbkdf2.compute(passwordBytes, {}, keySize)}; // TODO: Add password salt to file
    std::vector<std::uint8_t> initVecBytes(initVecStr.begin(), initVecStr.end());

    Block<cols, rows> initVec{Block<cols, rows>::fromBytes(initVecBytes)};
    Block<keyWordCount, rows> key{Block<keyWordCount, rows>::fromBytes(keyBytes)};

    KeySchedule<cols, rows, rounds> keySchedule = KeySchedule<cols, rows, rounds>(key, subBox, roundConstants);
    CBC<cols, rows> blockString = CBC<cols, rows>(inputData, encryptionMode == DECRYPT);

    if (encryptionMode == ENCRYPT) blockString.encrypt(keySchedule, subBox, mixColMatrix, initVec);
    else if (encryptionMode == DECRYPT) blockString.decrypt(keySchedule, subBox, mixColMatrixInv, initVec);

    std::string outputData = blockString.getText(encryptionMode == DECRYPT);

    writeToFile(outputPath, outputData);

    if (encryptionMode == ENCRYPT) writeToFile(metadataPath, initVecStr);
    else if (encryptionMode == DECRYPT) deleteFile(metadataPath);

    if (deletePrev) deleteFile(inputPath);

    return 0;
}
