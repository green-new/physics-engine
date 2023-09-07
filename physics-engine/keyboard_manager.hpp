#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <functional>
#include "key_subscription.hpp"

namespace Input {
    using key_callback_fn = std::function<void(const int key, const int scancode, const int action, const int mods)>;

    class KeyboardManager {
    public:
        size_t  subscribe(key_callback_fn func, const std::vector<int>& keys);
        void    unsubscribe(size_t index);
        void    update(int key, int scancode, int action, int mods);
    private:
        std::vector<key_callback_fn>    m_callbacks{};
        std::vector<std::vector<int>>   m_keys{};
    };
}
