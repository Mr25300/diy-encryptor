#include "io.hpp"

#include <string>
#include <iostream>
#include <fstream>

namespace io {
    bytes::ByteVec getInput() {
        std::string input;
        std::cin >> input;

        return bytes::ByteVec(input.begin(), input.end());
    }

    bytes::ByteVec readFile(const std::filesystem::path& filePath) {
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file) throw std::ios_base::failure("Failed to read from file: " + filePath.string());

        std::streamsize fileSize{file.tellg()};

        bytes::ByteVec fileData(fileSize);
        file.seekg(0);
        file.read(reinterpret_cast<char*>(fileData.data()), fileSize);
        file.close();

        return fileData;
    }

    void writeToFile(const std::filesystem::path& filePath, const bytes::ByteVec& data) {
        std::ofstream file(filePath, std::ios::binary | std::ios::trunc);
        if (!file) throw std::ios_base::failure("Failed to write to file: " + filePath.string());

        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();
    }

    void deleteFile(const std::filesystem::path& filePath) {
        if (!std::filesystem::remove(filePath)) {
            std::cerr << "Failed to delete file: " + filePath.string();
        }
    }
}
