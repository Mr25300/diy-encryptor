#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <array>
#include <vector>

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
constexpr size_t rounds = 10;

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

const Block<cols, rows> ivBlock = Block<cols, rows>({
    Vector<rows>({0x01, 0x23, 0x45, 0x67}),
    Vector<rows>({0x89, 0xAB, 0xCD, 0xEF}),
    Vector<rows>({0xFE, 0xDC, 0xBA, 0x98}),
    Vector<rows>({0x76, 0x54, 0x32, 0x10})
});

Block<cols, rows> getKey(std::string password) { // Change this to static constructor in block class
    Block<cols, rows> key;

    for (int i = 0; i < blockSize; i++) {
        key[i / rows][i % rows] = password[i % password.length()];
    }

    return key;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Error";

        return 1;
    }

    std::string filePath = argv[1];
    std::ifstream inFile(filePath, std::ios::binary | std::ios::ate);

    if (!inFile) {
        std::cerr << "Failed to read from file: " + filePath;

        return 1;
    }

    bool encrypted = std::filesystem::path(filePath).extension() == ".enc";

    std::streamsize fileSize = inFile.tellg();
    inFile.seekg(0);

    std::string fileData(fileSize, '\0');
    inFile.read(fileData.data(), fileSize);
    inFile.close();

    std::string password;
    std::cout << (encrypted ? "Decrypting" : "Encrypting") << " file, input password key: ";
    std::cin >> password;

    Block<cols, rows> key = getKey(password);
    KeySchedule<cols, rows, rounds> keySchedule = KeySchedule<cols, rows, rounds>(key, subBox, roundConstants);
    BlockString<cols, rows> blockString = BlockString<cols, rows>(fileData, encrypted);

    if (encrypted) blockString.cbcDecrypt(keySchedule, subBox, mixColMatrixInv, ivBlock);
    else blockString.cbcEncrypt(keySchedule, subBox, mixColMatrix, ivBlock);

    std::string newData = blockString.getText(encrypted);

    std::ofstream outFile(filePath, std::ios::binary);

    if (!outFile) {
        std::cerr << "Failed to write to file: " << filePath;

        return 1;
    }

    outFile.seekp(0);
    outFile.write(newData.data(), newData.size());
    outFile.close();

    std::string newPath = encrypted ? filePath.substr(0, filePath.length() - 4) : filePath + ".enc";

    if (std::rename(filePath.c_str(), newPath.c_str()) != 0) {
        std::cerr << "Failed to rename file: " << filePath;

        return 1;
    }

    return 0;
}