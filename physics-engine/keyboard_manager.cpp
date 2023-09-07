#include "keyboard_manager.hpp"

namespace Input {
    size_t KeyboardManager::subscribe(key_callback_fn func, const std::vector<int>& keys) {
        m_callbacks.push_back(func);
        m_keys.push_back(keys);
        return m_callbacks.size();
    }

    void KeyboardManager::unsubscribe(size_t index) {
        m_callbacks.erase(m_callbacks.begin() + index);
        m_keys.erase(m_keys.begin() + index);
    }

    void KeyboardManager::update(int key, int scancode, int action, int mods) {
        for (size_t index = 0; index < m_keys.size(); index++) {
            const auto& keylist = m_keys.at(index);
            for (int keytest : keylist) {
                if (keytest == key) {
                    m_callbacks[index](key, scancode, action, mods);
                }
            }
        }
    }
}