#pragma once

#include "bytes.hpp"

#include <filesystem>

namespace io {
    bytes::ByteVec getInput();

    bytes::ByteVec readFile(const std::filesystem::path& filePath);

    void writeToFile(const std::filesystem::path& filePath, std::initializer_list<bytes::ByteView> data);
    void writeToFile(const std::filesystem::path& filePath, bytes::ByteView data);

    void deleteFile(const std::filesystem::path& filePath);
}
