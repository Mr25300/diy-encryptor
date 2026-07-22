#include "bytes.hpp"

#include <filesystem>

namespace io {
    bytes::ByteVec readFile(const std::filesystem::path& filePath);
    std::vector<bytes::ByteVec> readFileLines(const std::filesystem::path& filePath);

    void writeToFile(const std::filesystem::path& filePath, const bytes::ByteVec& data);
    template <std::size_t N>
    void writeLinesToFile(const std::filesystem::path& filePath, const std::array<bytes::ByteVec, N>& lines);

    void deleteFile(const std::filesystem::path& filePath);

    bytes::ByteVec getInput();
}
