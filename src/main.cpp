#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <array>
#include <vector>
#include <ostream>

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
const std::string ivExtension = ".iv";

std::string generateIV(size_t length) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(0, 255);

    std::string iv(length, '\0');

    for (size_t i = 0; i < length; i++) {
        iv[i] = static_cast<char>(dist(gen));
    }

    return iv;
}

std::string readFile(std::string filePath) {
    std::ifstream inFile(filePath, std::ios::binary | std::ios::ate);

    if (!inFile) {
        throw std::ios_base::failure("Failed to read from file: " + filePath);
    }

    std::streamsize fileSize = inFile.tellg();
    inFile.seekg(0);

    std::string fileData(fileSize, '\0');
    inFile.read(fileData.data(), fileSize);
    inFile.close();

    return fileData;
}

void writeToFile(std::string filePath, std::string data) {
    std::ofstream outFile(filePath, std::ios::binary);

    if (!outFile) {
        throw std::ios_base::failure("Failed to write to file: " + filePath);
    }

    outFile.seekp(0);
    outFile.write(data.data(), data.size());
    outFile.close();
}

void renameFile(std::string filePath, std::string newPath) {
    if (std::rename(filePath.c_str(), newPath.c_str()) != 0) {
        throw std::runtime_error("Failed to rename file: " + filePath);
    }
}

void deleteFile(std::string filePath) {
    if (std::remove(filePath.c_str()) != 0) {
        throw std::runtime_error("Failed to delete file: " + filePath);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Error";

        return 1;
    }

    std::string filePath = argv[1];
    std::string fileData = readFile(filePath);

    bool encrypted = std::filesystem::path(filePath).extension() == encryptedExtension;

    std::string rootFilePath = encrypted ? filePath.substr(0, filePath.length() - encryptedExtension.size()) : filePath;
    std::string ivPath = rootFilePath + ivExtension;

    std::string iv = encrypted ? readFile(ivPath) : generateIV(blockSize);

    std::string password;
    std::cout << (encrypted ? "Decrypting" : "Encrypting") << " file, input password key: ";
    std::cin >> password;

    if (password.length() != keySize) {
        throw std::invalid_argument("Key does not match required length of " + std::to_string(keySize));
    }

    Block<cols, rows> ivBlock = Block<cols, rows>::fromString(iv);
    Block<keyWordCount, rows> key = Block<keyWordCount, rows>::fromString(password);

    KeySchedule<cols, rows, rounds> keySchedule = KeySchedule<cols, rows, rounds>(key, subBox, roundConstants);
    BlockString<cols, rows> blockString = BlockString<cols, rows>(fileData, encrypted);

    if (encrypted) blockString.cbcDecrypt(keySchedule, subBox, mixColMatrixInv, ivBlock);
    else blockString.cbcEncrypt(keySchedule, subBox, mixColMatrix, ivBlock);

    std::string newData = blockString.getText(encrypted);

    writeToFile(filePath, newData);
    renameFile(filePath, encrypted ? rootFilePath : rootFilePath + encryptedExtension);

    if (encrypted) {
        deleteFile(ivPath);
    } else {
        writeToFile(ivPath, iv);
    }

    return 0;
}