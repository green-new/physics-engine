#include "config.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

void
Config::readConfig(const std::string& filename) {
    std::ifstream configFile(filename);

    // Check if file was opened successfully
    if (!configFile.is_open()) {
        std::cerr << "Error: Could not open config file " << filename << std::endl;
        return;
    }

    std::string line;

    // Read the file line by line
    while (std::getline(configFile, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Find the position of the '=' character
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            continue;  // Skip invalid lines
        }

        // Extract key and value
        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);

        // Trim whitespace from key and value
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        // Process the key-value pair
        if (key == "WIDTH") {
            try {
                m_width = static_cast<uint16_t>(std::stoi(value));
            } catch (const std::exception& e) {
                std::cerr << "Error: Invalid value for WIDTH: " << value << std::endl;
            }
        } else if (key == "HEIGHT") {
            try {
                m_height = static_cast<uint16_t>(std::stoi(value));
            } catch (const std::exception& e) {
                std::cerr << "Error: Invalid value for HEIGHT: " << value << std::endl;
            }
        }
        // Additional configuration parameters can be added here
    }

    configFile.close();
}