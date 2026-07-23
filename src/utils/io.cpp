#include "io.hpp"

#include <string>
#include <iostream>
#include <fstream>

namespace io {
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

    std::vector<bytes::ByteVec> readFileLines(const std::filesystem::path& filePath) {
        bytes::ByteVec data{readFile(filePath)};

        std::vector<bytes::ByteVec> lines;
        lines.emplace_back();

        for (std::uint8_t b : data) {
            lines.back().push_back(b);

            if (static_cast<char>(b) == '\n') lines.emplace_back();
        }

        return lines;
    }

    void writeToFile(const std::filesystem::path& filePath, const bytes::ByteVec& data) {
        std::ofstream file(filePath, std::ios::binary | std::ios::trunc);
        if (!file) throw std::ios_base::failure("Failed to write to file: " + filePath.string());

        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();
    }

    void writeLinesToFile(const std::filesystem::path& filePath, const std::vector<bytes::ByteVec>& lines) {
        // Clear file first
        std::ofstream file(filePath, std::ios::binary | std::ios::app);
        if (!file) throw std::ios_base::failure("Failed to write lines to file: " + filePath.string());

        for (const bytes::ByteVec& line : lines) {
            file.write(reinterpret_cast<const char*>(line.data()), line.size());
        }

        file.close();
    }

    void deleteFile(const std::filesystem::path& filePath) {
        if (!std::filesystem::remove(filePath)) {
            std::cerr << "Failed to delete file: " + filePath.string();
        }
    }

    bytes::ByteVec getInput() {
        std::string input;
        std::cin >> input;

        return bytes::ByteVec(input.begin(), input.end());
    }
}
