#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <functional>
#include "key_subscription.hpp"

namespace Input {
    // Function pointer to a keyboard callback function.
    using key_callback_fn = std::function<void(const GLFWKey key, const bool state)>;

    class KeyboardManager {
    public:
        KeyboardManager();
        size_t subscribe(key_callback_fn Function, std::array<GLFWKey, MAX_KEYS> Keys, size_t keysLen);
        void unsubscribe(size_t indexToRemove);
        void callbackAll(GLFWwindow* handle);
    private:
        // Array of key callbacks.
        std::array<key_callback_fn, MAX_KEY_CALLBACKS> keyCallbacks{};

        // Map of callback functions to their key subscriptions.
        std::array<std::unique_ptr<KeySubscription>, MAX_KEY_CALLBACKS> callbackToSubscription{};
        size_t usedCallbacks{};
    };
}
