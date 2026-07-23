#include "bytes.hpp"

#include <filesystem>

namespace io {
    bytes::ByteVec readFile(const std::filesystem::path& filePath);
    std::vector<bytes::ByteVec> readFileLines(const std::filesystem::path& filePath);

    void writeToFile(const std::filesystem::path& filePath, const bytes::ByteVec& data);
    void writeLinesToFile(const std::filesystem::path& filePath, const std::vector<bytes::ByteVec>& lines);

    void deleteFile(const std::filesystem::path& filePath);

    bytes::ByteVec getInput();
}
