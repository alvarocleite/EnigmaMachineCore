#include "FileAssetProvider.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

FileAssetProvider::~FileAssetProvider() = default;

std::string FileAssetProvider::loadAsset(std::string_view assetName) const {
    std::ifstream file(std::string(assetName), std::ios::in | std::ios::binary);
    if (!file) {
        throw std::runtime_error("FileAssetProvider: Could not open file " + std::string(assetName));
    }

    std::ostringstream outputStream;
    outputStream << file.rdbuf();
    return outputStream.str();
}
