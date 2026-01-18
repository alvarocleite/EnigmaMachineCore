#pragma once

#include "IAssetProvider.hpp"

/**
 * @brief Implementation of IAssetProvider that reads from the local filesystem.
 * This is the standard provider for CLI and Desktop applications.
 */
class FileAssetProvider : public IAssetProvider {
public:
    /**
     * @brief Loads the content of a file from the disk.
     *
     * @param assetName The path to the file.
     * @return std::string The file content.
     * @throws std::runtime_error If the file cannot be opened.
     */
    std::string loadAsset(std::string_view assetName) override;
};
