#pragma once

#include <filesystem>
#include <string_view>

#include "EnigmaError.hpp"
#include "EnigmaMachineConfig.hpp"
#include "IAssetProvider.hpp"

/**
 * @brief Factory class for loading Enigma Machine configurations.
 * Handles the parsing of TOML files and the creation of configuration objects.
 * This class is stateless (static methods) and relies on IAssetProvider for IO.
 */
namespace fs = std::filesystem;
class EnigmaConfigLoader {
public:
    /**
     * @brief Strongly typed wrapper for file names.
     * Used to distinguish between general file names and base asset directories.
     * @note This helps prevent accidental mix-ups between different types of paths.
     * @see AssetPath
     */
    struct FileName : public fs::path {
        explicit FileName(fs::path path) : fs::path(std::move(path)) {}
    };

    /**
     * @brief Strongly typed wrapper for asset paths.
     * Used to distinguish between general file names and base asset directories.
     * @note This helps prevent accidental mix-ups between different types of paths.
     * @see FileName
     * @note Defaults to an empty path if not provided.
     */
    struct AssetPath : public fs::path {
        explicit AssetPath(fs::path path) : fs::path(std::move(path)) {}
        explicit AssetPath() : fs::path("") {}
    };

    /**
     * @brief Loads the complete Enigma Machine configuration from a TOML file.
     *
     * @param provider The asset provider to load files from.
     * @param fileName The path to the main configuration TOML file.
     * @param assetPath An optional base directory path to prepend to relative paths found in the config file.
     * @return enigma::Result<EnigmaMachineConfig> A fully populated configuration object, or an error code.
     */
    static enigma::Result<EnigmaMachineConfig> load(const IAssetProvider& provider, const FileName& fileName,
                                                    const AssetPath& assetPath = AssetPath());

    /**
     * @brief Loads a single rotor configuration from a TOML file.
     *
     * @param provider The asset provider to load files from.
     * @param fileName The path to the rotor configuration file.
     * @return enigma::Result<RotorConfig> The parsed rotor configuration, or an error code.
     */
    static enigma::Result<RotorConfig> loadRotor(const IAssetProvider& provider, const FileName& fileName);

    /**
     * @brief Loads a single reflector configuration from a TOML file.
     *
     * @param provider The asset provider to load files from.
     * @param fileName The path to the reflector configuration file.
     * @return enigma::Result<ReflectorConfig> The parsed reflector configuration, or an error code.
     */
    static enigma::Result<ReflectorConfig> loadReflector(const IAssetProvider& provider, const FileName& fileName);
};
