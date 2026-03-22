#pragma once

#include "IAssetProvider.hpp"

/**
 * @brief Implementation of IAssetProvider that reads from the local filesystem.
 * This is the standard provider for CLI and Desktop applications.
 */
class FileAssetProvider : public IAssetProvider {
public:
    ~FileAssetProvider() override;
    /**
     * @brief Loads the content of a file from the disk.
     *
     * @param assetName The path to the file.
     * @return enigma::Result<std::string> The file content, or an error code.
     */
    enigma::Result<std::string> loadAsset(std::string_view assetName) const override;
};
