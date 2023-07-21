#include "window.hpp"

inline void reshapeCallback(GLFWwindow* handle, int width, int height) {
    glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* a, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
        glfwSetWindowShouldClose(a, 1);
    }
}

Window::Window(const uint16_t _width, const uint16_t _height, std::string _title) : width(_width), height(_height), title(_title) {
    // Set the window hints (version, GL profile, window)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (!handle) {
        glfwTerminate();
        exit(-1);
    }

    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowSizeCallback(handle, reshapeCallback);
    glfwSetKeyCallback(handle, keyCallback);

    /* Make the window's context current */
    glfwMakeContextCurrent(handle);

    // Prepare our input managers.
    mouseManager = Input::MouseManager();
    keyboardManager = Input::KeyboardManager();
    glfwSetWindowUserPointer(handle, this);
    auto cursorPosCallback = [](GLFWwindow* handle, double xpos, double ypos) {
        static_cast<Window*>(glfwGetWindowUserPointer(handle))->mouseManager.updateMouseState(xpos, ypos);
    };
    glfwSetCursorPosCallback(handle, cursorPosCallback);
    // We callback the keyboard manager's callbacks later.
}
Window::~Window() {
    glfwDestroyWindow(handle);
}
uint16_t Window::getWidth() {
    return width;
}
uint16_t Window::getHeight() {
    return height;
}
void Window::updateTitle(std::string _title) {
    title = _title;
    glfwSetWindowTitle(handle, title.c_str());
}
GLFWwindow* Window::getHandle() {
    return handle;
}
Input::KeyboardManager& Window::getKeyboardManager() {
    return keyboardManager;
}
Input::MouseManager& Window::getMouseManager() {
    return mouseManager;
}
void Window::updateKeyboard() {
    keyboardManager.callbackAll(handle);
}