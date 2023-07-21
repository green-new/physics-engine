#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <memory>
#include <array>

namespace Input {
    // The mouse state.
    using MouseState =
        struct {
        double mouseXo;
        double mouseYo;
        double lastMouseX;
        double lastMouseY;
        double deltaX;
        double deltaY;
    };

    // Function pointer to a mouse callback function.
    using mouse_callback_fn = std::function<void(const MouseState& mouse)>;

    // Defines the maximum number of callback functions that can be stored in the MouseManager.
    constexpr size_t MAX_MOUSE_CALLBACKS = 64;

    // Singleton class.
    class MouseManager {
    public:
        MouseManager();
        // Subscribe to the mouse manager with a function.
        void subscribe(mouse_callback_fn Function);

        void unsubscribe(mouse_callback_fn Function);

        // Updates the mouse state. Called automatically by glfw.
        void updateMouseState(double xpos, double ypos);
    private:
        // Array of mouse callback functions.
        std::array<std::shared_ptr<mouse_callback_fn>, MAX_MOUSE_CALLBACKS> mouseCallbacks{};
        size_t usedCallbacks{};

        // Map of mouse callback functions to their indices.
        std::unordered_map<std::shared_ptr<mouse_callback_fn>, size_t> mouseCallbackToIndex{};

        // The internal mouse state.
        MouseState mouse{};

        void callbackAll();
    };
}
