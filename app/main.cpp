
#include <CLI/CLI.hpp>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <string>

#include "EnigmaCore.hpp"
#include "config.hpp"

namespace fs = std::filesystem;

/**
 * @brief Resolves the default asset path based on execution context.
 * Checks for a local 'assets/' folder first, otherwise falls back to the
 * path defined during installation.
 */
std::string resolveDefaultAssetPath() {
    // Check for local 'assets' directory
    if (fs::exists("assets") && fs::is_directory("assets")) {
        return "assets/";
    }

#ifdef ENIGMA_INSTALL_ASSETS_PATH
    // Fallback to the installed assets path if defined via CMake
    if (fs::exists(ENIGMA_INSTALL_ASSETS_PATH) && fs::is_directory(ENIGMA_INSTALL_ASSETS_PATH)) {
        return ENIGMA_INSTALL_ASSETS_PATH;
    }
#endif

    // Default to the header-defined constant if everything else fails
    return std::string(assetsDir);
}

struct AppConfig {
    std::string assetPath = resolveDefaultAssetPath();
    std::string configPath = "";  // Will be set after assetPath is finalized
    std::string message = "HELLOWORLD";
    bool debug = false;
    bool encode = false;
    bool decode = false;
};

/**
 * @brief Observer that logs Enigma machine events to the console.
 */
class ConsoleObserver : public IEnigmaObserver {
public:
    void onRotorStepped(int rotorIndex, int position) override {
        std::cout << "[Event] Rotor " << rotorIndex << " stepped to position " << position << "\n";
    }

    void onCharEncrypted(char input, char output) override {
        std::cout << "[Event] Encrypted: " << input << " -> " << output << "\n";
    }
};

/**
 * @brief Processes a message through the Enigma Machine.
 * Transforms each character and optionally prints debug info.
 */
std::string processMessage(EnigmaMachine& machine, const std::string& input, bool /*debug*/) {
    std::string output = "";
    for (char c : input) {
        if (!std::isalpha(c)) {
            continue;  // Skip non-alphabetic characters
        }
        char upperC = std::toupper(c);
        char res = machine.keyTransform(upperC - 'A') + 'A';
        output += res;
    }
    return output;
}

/**
 * @brief Parses command line arguments and populates the AppConfig struct.
 * Exits the program (via CLI11::Exit) if help is requested or parsing fails.
 */
AppConfig parseArguments(int argc, char** argv) {
    CLI::App app{"Enigma Machine CLI"};
    AppConfig config;

    app.add_option("-c,--config", config.configPath, "Path to the TOML configuration file");
    app.add_option("-a,--assets", config.assetPath, "Base directory for assets (rotors/reflectors)");
    app.add_option("-m,--message", config.message, "Message to process");
    app.add_flag("-d,--debug", config.debug, "Enable verbose event logging (rotor steps, encryption details)");
    app.add_flag("--encode", config.encode, "Encode the message");
    app.add_flag("--decode", config.decode, "Decode the message");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        std::exit(app.exit(e));
    }

    // Default to a standard config file if none provided, relative to finalized assetPath
    if (config.configPath.empty()) {
        config.configPath = (fs::path(config.assetPath) / "EnigmaMachineConfig1.toml").string();
    }

    // Default to round-trip if neither is specified
    if (!config.encode && !config.decode) {
        config.encode = true;
        config.decode = true;
    }

    return config;
}

void runApplication(const AppConfig& config) {
    EnigmaMachine machine(config.configPath, config.assetPath);

    // Create and register observer if debug is enabled
    ConsoleObserver observer;
    if (config.debug) {
        machine.registerObserver(&observer);
        std::cout << "Debug mode enabled: Observer registered.\n";
    }

    std::string currentMessage = config.message;

    if (config.encode) {
        std::cout << "Encoding message: " << currentMessage << "\n";
        currentMessage = processMessage(machine, currentMessage, config.debug);
        std::cout << "Result (Ciphertext): " << currentMessage << "\n";
    }

    if (config.decode) {
        // Re-initialize for decryption (symmetric cipher starting from same state)
        EnigmaMachine decodeMachine(config.configPath, config.assetPath);

        // Register observer for the decode machine as well
        if (config.debug) {
            decodeMachine.registerObserver(&observer);
        }

        std::cout << "Decoding message: " << currentMessage << "\n";
        std::string decoded = processMessage(decodeMachine, currentMessage, config.debug);
        std::cout << "Result (Plaintext): " << decoded << "\n";

        if (config.encode) {
            // Verify round-trip success against normalized input
            std::string cleanOriginal = "";
            for (char c : config.message)
                if (std::isalpha(c)) cleanOriginal += std::toupper(c);

            if (decoded == cleanOriginal) {
                std::cout << "Success: Decoded message matches original!" << "\n";
            } else {
                std::cout << "Failure: Decoded message mismatch." << "\n";
            }
        }
    }
}

/**
 * @brief Main entry point.
 */
int main(int argc, char** argv) {
    try {
        AppConfig config = parseArguments(argc, argv);
        runApplication(config);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    std::cout << "End of program." << "\n";
    return 0;
}
