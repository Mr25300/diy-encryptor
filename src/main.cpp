#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <array>
#include <ostream>
#include <random>

#include "gf256.hpp"
#include "vector.hpp"
#include "matrix.hpp"
#include "block.hpp"
#include "block_string.hpp"
#include "key_schedule.hpp"
#include "substitution_box.hpp"

constexpr size_t cols = 4;
constexpr size_t rows = 4;
constexpr size_t blockSize = cols * rows;

constexpr size_t keyWordCount = 4; // 4, 6, 8
constexpr size_t keySize = keyWordCount * rows;

constexpr size_t rounds = 10; // 10, 12, 14

constexpr size_t iterations = 1000;

constexpr std::array<GF256, rounds> roundConstants = []() constexpr {
    std::array<GF256, rounds> constants{};
    GF256 constant = 1;

    for (int i = 0; i < rounds; i++) {
        constants[i] = constant;
        constant *= 2;
    }

    return constants;
}();

constexpr SubstitutionBox subBox;

constexpr Matrix<rows> mixColMatrix = Matrix<rows>::createCirculantMatrix(Vector<rows>({2, 3, 1, 1}));
constexpr Matrix<rows> mixColMatrixInv = mixColMatrix.inverse();

const std::string encryptedExtension = ".enc";
const std::string decryptedExtension = ".dec";
const std::string metadataExtension = ".enc.meta"; // Change to ".enc.meta" for storing salt from PBKDF2

std::string generateIV(size_t length) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(0, 255);

    std::string iv(length, '\0');

    for (size_t i = 0; i < length; i++) {
        iv[i] = static_cast<char>(dist(gen));
    }

    return iv;
}

std::string readFile(const std::filesystem::path& filePath) {
    std::ifstream inFile(filePath, std::ios::binary | std::ios::ate);

    if (!inFile) {
        throw std::ios_base::failure("Failed to read from file: " + filePath.string());
    }

    std::streamsize fileSize = inFile.tellg();
    inFile.seekg(0);

    std::string fileData(fileSize, '\0');
    inFile.read(fileData.data(), fileSize);
    inFile.close();

    return fileData;
}

void writeToFile(const std::filesystem::path& filePath, const std::string& data) {
    std::ofstream outFile(filePath, std::ios::binary);

    if (!outFile) {
        throw std::ios_base::failure("Failed to write to file: " + filePath.string());
    }

    outFile.seekp(0);
    outFile.write(data.data(), data.size());
    outFile.close();
}

void deleteFile(const std::filesystem::path& filePath) {
    if (!std::filesystem::remove(filePath)) {
        std::cerr << "Failed to delete file: " + filePath.string();
    }
}

enum EncryptionMode { ENCRYPT, DECRYPT, UNDEFINED };

int main(int argc, char *argv[]) {
    if (mixColMatrixInv.isSingular()) {
        throw std::runtime_error("Mix columns matrix is singular, no inverse exists.");
    }

    std::string prevArg;
    std::string filePathStr;
    std::string outputDirStr;
    EncryptionMode encryptionMode = UNDEFINED;
    bool deletePrev = false;

    for (int i = 1; i < argc; i++) {
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
    std::string initVecStr;

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

        initVecStr = generateIV(blockSize);

    } else if (encryptionMode == DECRYPT) {
        if (inputPath.extension() == encryptedExtension) {
            outputPath.replace_extension("");
            metadataPath.replace_extension(metadataExtension);
        } else {
            outputPath += decryptedExtension;
            metadataPath += metadataExtension;
        }

        initVecStr = readFile(metadataPath);
    }

    std::cout << inputPath << '\n' << metadataPath << '\n' << outputPath << '\n';

    std::string inputData = readFile(inputPath);

    std::string password;
    std::cout << (encryptionMode == ENCRYPT ? "Encrypting" : "Decrypting") << " file, input password key: ";
    std::cin >> password;

    if (password.length() != keySize) {
        throw std::invalid_argument("Key does not match required length of " + std::to_string(keySize));
    }

    Block<cols, rows> initVec = Block<cols, rows>::fromString(initVecStr);
    Block<keyWordCount, rows> key = Block<keyWordCount, rows>::fromString(password);

    KeySchedule<cols, rows, rounds> keySchedule = KeySchedule<cols, rows, rounds>(key, subBox, roundConstants);
    BlockString<cols, rows> blockString = BlockString<cols, rows>(inputData, encryptionMode == DECRYPT);

    if (encryptionMode == ENCRYPT) blockString.cbcEncrypt(keySchedule, subBox, mixColMatrix, initVec);
    else if (encryptionMode == DECRYPT) blockString.cbcDecrypt(keySchedule, subBox, mixColMatrixInv, initVec);

    std::string outputData = blockString.getText(encryptionMode == DECRYPT);

    writeToFile(outputPath, outputData);

    if (encryptionMode == ENCRYPT) writeToFile(metadataPath, initVecStr);
    else if (encryptionMode == DECRYPT) deleteFile(metadataPath);

    if (deletePrev) deleteFile(inputPath);

    return 0;
}
