#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <memory>
#include <array>

namespace Input {
    using Mouse = 
        struct Mouse {
            double mouseXo{};
            double mouseYo{};
        };

    using MouseState =
        struct MouseState {
            Mouse prev{};
            Mouse curr{};

            double deltaX() const { return curr.mouseXo - prev.mouseXo; }
            double deltaY() const { return curr.mouseYo - prev.mouseYo; }
        };

    using mouse_callback_fn = std::function<void(const MouseState& mouse)>;

    class MouseManager {
    public:
        size_t  subscribe(mouse_callback_fn func);
        void    unsubscribe(size_t index);
        void    update(double xpos, double ypos);
    private:
        std::vector<mouse_callback_fn>  m_callbacks{};
        MouseState                      m_mState{};
    };
}
