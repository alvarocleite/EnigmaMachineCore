#pragma once

#include <string>
#include <string_view>

#include "EnigmaCore_EXPORT.hpp"
#include "EnigmaError.hpp"

/**
 * @brief Interface for providing configuration/asset data.
 * Abstracts the source of assets (filesystem, memory, embedded) to allow for
 * decoupling the Enigma Engine from the physical filesystem.
 */
class ENIGMACORE_EXPORT IAssetProvider {
public:
    virtual ~IAssetProvider();

    /**
     * @brief Loads the content of an asset.
     *
     * @param assetName The name or path of the asset to load (e.g., "Rotor1.toml").
     * @return enigma::Result<std::string> The content of the asset, or an error code.
     */
    virtual enigma::Result<std::string> loadAsset(std::string_view assetName) const = 0;
};
