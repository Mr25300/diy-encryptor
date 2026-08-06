#include "bytes.hpp"

#include <filesystem>

namespace io {
    bytes::ByteVec getInput();

    bytes::ByteVec readFile(const std::filesystem::path& filePath);
    void writeToFile(const std::filesystem::path& filePath, const bytes::ByteVec& data);

    void deleteFile(const std::filesystem::path& filePath);
}
