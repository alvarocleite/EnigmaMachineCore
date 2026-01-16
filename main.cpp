
#include <CLI/CLI.hpp>
#include <cctype>
#include <iostream>
#include <string>
#include "./EnigmaMachine/include/EnigmaMachine.hpp"

#include "config/config.hpp"

struct AppConfig {
    std::string configPath = std::string(assetsDir) + "EnigmaMachineConfig1.toml";
    std::string assetPath = std::string(assetsDir);
    std::string message = "HELLOWORLD";
    bool debug = false;
    bool encode = false;
    bool decode = false;
};

/**
 * @brief Processes a message through the Enigma Machine.
 * Transforms each character and optionally prints debug info.
 */
std::string processMessage(EnigmaMachine& machine, const std::string& input, bool debug) {
    std::string output = "";
    for (char c : input) {
        if (!std::isalpha(c)) {
            continue;  // Skip non-alphabetic characters
        }
        char upperC = std::toupper(c);
        char res = machine.keyTransform(upperC - 'A') + 'A';
        output += res;

        if (debug) {
            std::cout << "Input: " << upperC << ", Output: " << res << "\n";
        }
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
    app.add_flag("-d,--debug", config.debug, "Enable character-by-character transformation output");
    app.add_flag("--encode", config.encode, "Encode the message");
    app.add_flag("--decode", config.decode, "Decode the message");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        std::exit(app.exit(e));
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
    std::string currentMessage = config.message;

    if (config.encode) {
        std::cout << "Encoding message: " << currentMessage << "\n";
        currentMessage = processMessage(machine, currentMessage, config.debug);
        std::cout << "Result (Ciphertext): " << currentMessage << "\n";
    }

    if (config.decode) {
        // Re-initialize for decryption (symmetric cipher starting from same state)
        EnigmaMachine decodeMachine(config.configPath, config.assetPath);

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
