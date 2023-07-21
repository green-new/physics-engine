#include "keyboard_manager.hpp"

using namespace Input;

KeyboardManager::KeyboardManager() : keyCallbacks(), usedCallbacks(0) {
}

size_t KeyboardManager::subscribe(key_callback_fn Function, std::array<GLFWKey, MAX_KEYS> Keys, size_t keysLen) {
    size_t newIndex = usedCallbacks;
    keyCallbacks[newIndex] = Function;
    callbackToSubscription[newIndex] = std::make_unique<KeySubscription>(Keys, keysLen);
    ++usedCallbacks;
    return newIndex;
}

void KeyboardManager::unsubscribe(size_t indexToRemove) {
    size_t lastIndex = usedCallbacks;
    keyCallbacks[indexToRemove] = std::move(keyCallbacks[lastIndex]);
    callbackToSubscription[indexToRemove] = std::move(callbackToSubscription[lastIndex]);
    callbackToSubscription[lastIndex].release();
    --usedCallbacks;
}

void KeyboardManager::callbackAll(GLFWwindow* handle) {
    for (size_t i = 0; i < usedCallbacks; i++) {
        const auto& Function = keyCallbacks[i];
        auto& sub = *callbackToSubscription[i].get();
        const auto& list = sub.getKeyList();
        for (size_t j = 0; j < sub.size(); j++) {
            const auto& key = list[j];
            const auto& state = glfwGetKey(handle, key);
            sub.setKeyState(key, state);
        }
        for (const auto& key_state : sub.getKeyStates()) {
            Function(key_state.first, key_state.second);
        }
    }
}