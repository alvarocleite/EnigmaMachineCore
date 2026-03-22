#include "FileAssetProvider.hpp"
#include <fstream>
#include <sstream>

#include "EnigmaError.hpp"

FileAssetProvider::~FileAssetProvider() = default;

enigma::Result<std::string> FileAssetProvider::loadAsset(std::string_view assetName) const {
    std::ifstream file(std::string(assetName), std::ios::in | std::ios::binary);
    if (!file) {
        return nonstd::make_unexpected(enigma::EnigmaError::FileNotFound);
    }

    std::ostringstream outputStream;
    outputStream << file.rdbuf();
    return outputStream.str();
}
