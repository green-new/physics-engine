#pragma once
#include "types.hpp"
#include <GLFW/glfw3.h>
#include <array>
#include <algorithm>
#include <unordered_map>
#include <format>
#include <assert.h>

namespace Input {
    // Defines the maximum number of keys a KeySubscription can subscribe to.
    constexpr size_t MAX_KEYS = 8;

    // Defines the maximum number of callback functions that can be stored in the KeyboardManager.
    constexpr size_t MAX_KEY_CALLBACKS = 64;

    // Enumeration of the GLFW keys used in this program.
    enum GLFWKey {
        Space = GLFW_KEY_SPACE,
        A = GLFW_KEY_A,
        D = GLFW_KEY_D,
        F = GLFW_KEY_F,
        Q = GLFW_KEY_Q,
        S = GLFW_KEY_S,
        W = GLFW_KEY_W,
        Escape = GLFW_KEY_ESCAPE
    };

    class KeySubscription {
    public:
        KeySubscription();
        KeySubscription(std::array<GLFWKey, MAX_KEYS> _keysList, size_t _keySize) {
            keySize = _keySize;
            assert(keySize <= MAX_KEYS && std::format("Maximum number of keys is {}, provided {}", MAX_KEYS, keySize).c_str());

            std::copy(std::begin(_keysList), std::end(_keysList), std::begin(keysList));

            for (size_t i = 0; i < keySize; i++) {
                keyStates.insert({ keysList[i], GLFW_RELEASE });
            }
        }

        // Get the keys list.
        const std::array<GLFWKey, MAX_KEYS>& getKeyList() const {
            return keysList;
        }

        // Get the key states.
        std::unordered_map<GLFWKey, bool> getKeyStates() const {
            return keyStates;
        }

        // Sets the key state.
        void setKeyState(GLFWKey key, bool state);

        // Get the number of keys used.
        size_t size() const { return keySize; }
    private:
        // The list of keys subscribed.
        std::array<GLFWKey, MAX_KEYS> keysList{};

        // The key states.
        std::unordered_map<GLFWKey, bool> keyStates{};

        // The number of keys subscribed.
        size_t keySize;
    };
}