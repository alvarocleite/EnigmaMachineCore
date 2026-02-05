#pragma once

#include <string_view>
#include <filesystem>

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
    struct FileName : public fs::path {
        explicit FileName(fs::path path) : fs::path(std::move(path)) {}
    };

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
     * @return EnigmaMachineConfig A fully populated configuration object.
     * @throws std::runtime_error If parsing fails or validation checks fail.
     */
    static EnigmaMachineConfig load(IAssetProvider& provider, const FileName& fileName,
                                    const AssetPath& assetPath = AssetPath());

    /**
     * @brief Loads a single rotor configuration from a TOML file.
     *
     * @param provider The asset provider to load files from.
     * @param fileName The path to the rotor configuration file.
     * @return RotorConfig The parsed rotor configuration.
     */
    static RotorConfig loadRotor(IAssetProvider& provider, const FileName& fileName);

    /**
     * @brief Loads a single reflector configuration from a TOML file.
     *
     * @param provider The asset provider to load files from.
     * @param fileName The path to the reflector configuration file.
     * @return ReflectorConfig The parsed reflector configuration.
     */
    static ReflectorConfig loadReflector(IAssetProvider& provider, const FileName& fileName);
};
