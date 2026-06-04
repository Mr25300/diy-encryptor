#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <array>
#include <ostream>
#include <random>

#include <math/gf256.hpp>
#include <math/word.hpp>
#include <math/matrix.hpp>
#include <aes/block.hpp>
#include <modes/cbc.hpp>
#include <aes/key_schedule.hpp>
#include <aes/substitution_box.hpp>

#include <hash/sha256.hpp>
#include <prf/hmac.hpp>
#include <kdf/pbkdf2.hpp>

constexpr size_t cols = 4;
constexpr size_t rows = 4;
constexpr size_t blockSize = cols * rows;

constexpr size_t keyWordCount = 4; // 4, 6, 8
constexpr size_t keySize = keyWordCount * rows;

constexpr size_t rounds = 10; // 10, 12, 14

constexpr int kdfIterations{6000}; // Should be 600000

constexpr std::array<math::GF256, rounds> roundConstants = []() constexpr {
    std::array<math::GF256, rounds> constants{};
    math::GF256 constant = 1;

    for (int i = 0; i < rounds; i++) {
        constants[i] = constant;
        constant *= 2;
    }

    return constants;
}();

constexpr SubstitutionBox subBox;

constexpr math::Matrix<rows> mixColMatrix = math::Matrix<rows>::createCirculantMatrix(math::Word<rows>({2, 3, 1, 1}));
constexpr math::Matrix<rows> mixColMatrixInv = mixColMatrix.inverse();

constexpr SHA256 sha256{};
constexpr HMAC hmac{sha256};
constexpr PBKDF2 pbkdf2{hmac, kdfIterations};

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
    if (mixColMatrixInv.isSingular()) { // Make this a static_assert
        throw std::runtime_error("Mix columns matrix is singular, no inverse exists.");
    }

    std::string prevArg;
    std::string filePathStr;
    std::string outputDirStr;
    EncryptionMode encryptionMode = UNDEFINED;
    bool deletePrev = false;

    for (int i{1}; i < argc; i++) {
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

    std::string inputData = readFile(inputPath);

    std::string password;
    std::cout << (encryptionMode == ENCRYPT ? "Encrypting" : "Decrypting") << " file, input password key: ";
    std::cin >> password;

    std::vector<std::uint8_t> passwordBytes(password.begin(), password.end());
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
